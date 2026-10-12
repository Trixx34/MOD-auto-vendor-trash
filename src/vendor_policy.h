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

} // namespace auto_vendor

#endif // AUTO_VENDOR_POLICY_H
