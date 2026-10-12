// auto-vendor-trash: sells looted grey items through the core's module actions.
//
// Usage: auto-vendor-trash <tier1-endpoint>. The core passes the endpoint when it spawns the module.

#include "vendor_policy.h"

#include "forever_module/module_client.h"

#include <iostream>

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cerr << "usage: auto-vendor-trash <tier1-endpoint>\n";
        return 1;
    }

    forever_module::ModuleManifest manifest;
    manifest.module_name = "auto-vendor-trash";
    manifest.module_version = "0.1.0";
    manifest.tier1_hooks = {"PlayerScript.OnLootItem"};

    forever_module::ModuleClient client(std::move(manifest));
    if (!client.Connect(argv[1]) || !client.Handshake())
    {
        std::cerr << "auto-vendor-trash: could not connect to the core: " << client.rejection_reason() << "\n";
        return 1;
    }

    // Defaults only: module config from the core is not wired up yet (core sends an empty config).
    auto_vendor::Policy policy;

    client.OnNotify("PlayerScript.OnLootItem", [&client, policy](forever_module::EventNotify const& notify)
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
        client.SellItem(playerGuid, loot.item_guid(), loot.item_entry(), loot.count(),
                        [&client, playerGuid, itemName](forever_module::ActionReply const& reply)
        {
            if (!reply.success())
            {
                // The item stays in the player's bags; the core says why.
                std::cerr << "auto-vendor-trash: could not sell " << itemName << ": " << reply.reason() << "\n";
                return;
            }
            client.SendPlayerMessage(playerGuid, auto_vendor::FormatSaleMessage(itemName, reply.copper_gained()));
        });
    });

    client.Run();
    return 0;
}
