#include "vendor_policy.h"

#include <algorithm>

namespace auto_vendor {

bool ShouldSell(LootedItem const& item, Policy const& policy)
{
    if (item.Quality != kPoorQuality)
        return false;
    if (item.ItemClass == kQuestItemClass)
        return false;
    if (item.SellPrice == 0 || item.Count == 0)
        return false;
    if (std::find(policy.ExcludedItemIds.begin(), policy.ExcludedItemIds.end(), item.ItemEntry) != policy.ExcludedItemIds.end())
        return false;

    std::uint64_t const total = static_cast<std::uint64_t>(item.SellPrice) * item.Count;
    return total >= policy.MinValueCopper;
}

std::string FormatMoney(std::uint64_t copper)
{
    std::uint64_t const gold = copper / 10000;
    std::uint64_t const silver = (copper / 100) % 100;
    std::uint64_t const copperPart = copper % 100;

    std::string out;
    auto append = [&out](std::uint64_t value, char const* unit)
    {
        if (!out.empty())
            out += ' ';
        out += std::to_string(value) + unit;
    };

    if (gold)
        append(gold, "g");
    if (silver)
        append(silver, "s");
    if (copperPart || out.empty())
        append(copperPart, "c");
    return out;
}

std::string FormatSaleMessage(std::string const& itemName, std::uint64_t copperGained)
{
    return "[" + itemName + "] was sold automatically, got " + FormatMoney(copperGained) + ".";
}

} // namespace auto_vendor
