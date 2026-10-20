// auto-vendor-trash: sells looted grey items through the core's module actions.
//
// Usage: auto-vendor-trash <tier1-endpoint>. The core passes the endpoint when it spawns the module.

#include "vendor_policy.h"
#include "vendor_settings.h"

#include "forever_module/module_client.h"

#include <chrono>
#include <iostream>
#include <unordered_map>
#include <vector>

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cerr << "usage: auto-vendor-trash <tier1-endpoint>\n";
        return 1;
    }

    forever_module::ModuleManifest manifest;
    manifest.module_name = "auto-vendor-trash";
    manifest.module_version = FOREVER_MODULE_VERSION; // from module.json, see CMakeLists.txt
    manifest.tier1_hooks = {"PlayerScript.OnLootItem"};

    forever_module::ModuleClient client(std::move(manifest));
    if (!client.Connect(argv[1]) || !client.Handshake())
    {
        std::cerr << "auto-vendor-trash: could not connect to the core: " << client.rejection_reason() << "\n";
        return 1;
    }

    // Config arrives right after the handshake, so the policy is set before the hook is registered. A later
    // re-delivery (on the Run() thread) updates it for the next loot. Warnings go to the module's stderr.
    auto_vendor::Policy policy;
    client.OnConfig([&policy](std::unordered_map<std::string, std::string> const& config)
    {
        std::vector<std::string> warnings;
        policy = auto_vendor::ParseVendorSettings(config, warnings);
        for (std::string const& warning : warnings)
            std::cerr << "auto-vendor-trash: config: " << warning << "\n";
    });

    // What was sold since the last summary line. Sale replies and the timer both run on the Run() thread, so no lock is needed.
    auto_vendor::SaleSummary summary;
    if (policy.SummaryMinutes != 0)
    {
        std::uint32_t const minutes = policy.SummaryMinutes;
        client.AddTimer(std::chrono::minutes(minutes), [&client, &summary, minutes]
        {
            std::string const line = summary.Take(minutes);
            if (line.empty())
                return;
            std::cerr << "auto-vendor-trash: " << line << "\n";
            client.Log(0, line); // and to the core's Server.log, tagged with the module's name (an older core refuses it harmlessly)
        });
    }

    client.OnNotify("PlayerScript.OnLootItem", [&client, &policy, &summary](forever_module::EventNotify const& notify)
    {
        forever_module::PlayerItemLootedEvent const& loot = notify.player_item_looted();

        auto_vendor::LootedItem item;
        item.ItemEntry = loot.item_entry();
        item.Quality = loot.quality();
        item.ItemClass = loot.item_class();
        item.SellPrice = loot.sell_price();
        item.Count = loot.count();
        if (!auto_vendor::ShouldSell(item, policy))
            return;

        std::uint64_t const playerGuid = loot.player_guid();
        std::string const itemName = loot.item_name();
        std::uint32_t const soldCount = loot.count();
        client.SellItem(playerGuid, loot.item_guid(), loot.item_entry(), loot.count(),
                        [&client, &summary, playerGuid, itemName, soldCount](forever_module::ActionReply const& reply)
        {
            if (!reply.success())
            {
                // The item stays in the player's bags; the core says why.
                std::cerr << "auto-vendor-trash: could not sell " << itemName << ": " << reply.reason() << "\n";
                return;
            }
            summary.Add(soldCount, reply.copper_gained());
            client.SendPlayerMessage(playerGuid, auto_vendor::FormatSaleMessage(itemName, reply.copper_gained()));
        });
    });

    client.Run();
    return 0;
}
