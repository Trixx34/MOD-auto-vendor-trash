// Unit tests for the sale policy and message format. No core or SDK needed.
#include "vendor_policy.h"

#include <cstdio>

namespace {

int failures = 0;

#define CHECK(cond)                                                    \
    do {                                                               \
        if (!(cond)) {                                                 \
            ++failures;                                                \
            std::printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        }                                                              \
    } while (0)

auto_vendor::LootedItem Grey(std::uint32_t sellPrice, std::uint32_t count)
{
    auto_vendor::LootedItem item;
    item.ItemEntry = 2589;
    item.Quality = 0;
    item.ItemClass = 7;
    item.SellPrice = sellPrice;
    item.Count = count;
    return item;
}

} // namespace

int main()
{
    auto_vendor::Policy policy;

    // Grey items with a vendor value are sold.
    CHECK(auto_vendor::ShouldSell(Grey(20, 3), policy));

    // Not grey.
    auto_vendor::LootedItem white = Grey(20, 1);
    white.Quality = 1;
    CHECK(!auto_vendor::ShouldSell(white, policy));

    // Quest class is never sold, even when grey.
    auto_vendor::LootedItem quest = Grey(20, 1);
    quest.ItemClass = 12;
    CHECK(!auto_vendor::ShouldSell(quest, policy));

    // No vendor value.
    CHECK(!auto_vendor::ShouldSell(Grey(0, 1), policy));

    // Excluded item ids.
    auto_vendor::Policy excluding;
    excluding.ExcludedItemIds = {2589};
    CHECK(!auto_vendor::ShouldSell(Grey(20, 1), excluding));

    // Minimum value is on the whole stack.
    auto_vendor::Policy minimum;
    minimum.MinValueCopper = 50;
    CHECK(!auto_vendor::ShouldSell(Grey(20, 2), minimum));
    CHECK(auto_vendor::ShouldSell(Grey(20, 3), minimum));

    // Money formatting.
    CHECK(auto_vendor::FormatMoney(0) == "0c");
    CHECK(auto_vendor::FormatMoney(5) == "5c");
    CHECK(auto_vendor::FormatMoney(120) == "1s 20c");
    CHECK(auto_vendor::FormatMoney(10000) == "1g");
    CHECK(auto_vendor::FormatMoney(10000 + 2000 + 5) == "1g 20s 5c");
    CHECK(auto_vendor::FormatMoney(10205) == "1g 2s 5c");

    // Sale message format from the brief.
    CHECK(auto_vendor::FormatSaleMessage("Tattered Cloth", 10205) ==
          "[Tattered Cloth] was sold automatically, got 1g 2s 5c.");

    if (failures == 0)
    {
        std::printf("PASS  auto-vendor policy\n");
        return 0;
    }
    std::printf("%d check(s) failed\n", failures);
    return 1;
}
