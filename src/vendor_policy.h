// SPDX-License-Identifier: GPL-2.0-or-later
// Decides which looted items are auto-sold and formats the player-facing message. No SDK or core
// dependencies, so it is unit-tested directly (tests/policy_test.cpp).
#ifndef AUTO_VENDOR_POLICY_H
#define AUTO_VENDOR_POLICY_H

#include <cstdint>
#include <string>
#include <vector>

namespace auto_vendor {

// Item quality 0 is Poor (grey). Item class 12 is Quest; quest items are never sold.
inline constexpr std::uint32_t kPoorQuality = 0;
inline constexpr std::uint32_t kQuestItemClass = 12;

struct Policy
{
    // Stacks worth less than this (copper, sell price times count) are kept.
    std::uint64_t MinValueCopper = 0;
    // Item ids that are never sold, whatever their quality.
    std::vector<std::uint32_t> ExcludedItemIds;
    // Minutes between summary lines in the module's log ("42 items sold in the last 10 minutes. Total value 1g 20s 5c."). 0 turns
    // the line off. A period with no sale writes nothing.
    std::uint32_t SummaryMinutes = 10;
};

struct LootedItem
{
    std::uint32_t ItemEntry = 0;
    std::uint32_t Quality = 0;
    std::uint32_t ItemClass = 0;
    std::uint32_t SellPrice = 0; // per item, copper
    std::uint32_t Count = 0;     // items from this loot action
};

bool ShouldSell(LootedItem const& item, Policy const& policy);

// "1g 20s 5c", omitting zero units except copper when the amount is zero.
std::string FormatMoney(std::uint64_t copper);

// "[Item Name] was sold automatically, got 1g 20s 5c."
std::string FormatSaleMessage(std::string const& itemName, std::uint64_t copperGained);

// What the module sold since the last summary line.
class SaleSummary
{
public:
    void Add(std::uint64_t items, std::uint64_t copper)
    {
        _items += items;
        _copper += copper;
    }

    // The summary line for a period of `minutes`, and the counters start again. Empty (and the counters unchanged) when nothing was sold.
    std::string Take(std::uint32_t minutes);

private:
    std::uint64_t _items = 0;
    std::uint64_t _copper = 0;
};

} // namespace auto_vendor

#endif // AUTO_VENDOR_POLICY_H
