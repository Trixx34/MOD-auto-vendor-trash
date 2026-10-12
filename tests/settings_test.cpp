// Unit tests for ParseVendorSettings. No core needed.
#include "vendor_settings.h"

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

void Defaults()
{
    std::vector<std::string> warnings;
    auto const p = auto_vendor::ParseVendorSettings({}, warnings);
    CHECK(p.MinValueCopper == 0);
    CHECK(p.ExcludedItemIds.empty());
    CHECK(warnings.empty());
}

void ReadsValues()
{
    std::vector<std::string> warnings;
    auto const p = auto_vendor::ParseVendorSettings(
        { { "MinValueCopper", "500" }, { "ExcludedItemIds", "2589, 6948 ,4306" } }, warnings);
    CHECK(p.MinValueCopper == 500);
    CHECK(p.ExcludedItemIds.size() == 3 && p.ExcludedItemIds[0] == 2589 && p.ExcludedItemIds[1] == 6948 && p.ExcludedItemIds[2] == 4306);
    CHECK(warnings.empty());
}

void BadValuesWarnAndKeepDefaults()
{
    std::vector<std::string> warnings;
    auto const p = auto_vendor::ParseVendorSettings({ { "MinValueCopper", "1g" } }, warnings);
    CHECK(p.MinValueCopper == 0);
    CHECK(warnings.size() == 1);

    warnings.clear();
    auto const q = auto_vendor::ParseVendorSettings({ { "ExcludedItemIds", "2589,,abc,0,6948" } }, warnings);
    CHECK(q.ExcludedItemIds.size() == 2 && q.ExcludedItemIds[0] == 2589 && q.ExcludedItemIds[1] == 6948);
    CHECK(warnings.size() == 2); // "abc" and "0"; the empty entry is silently ignored
}

void EmptyListIsNoExclusions()
{
    std::vector<std::string> warnings;
    auto const p = auto_vendor::ParseVendorSettings({ { "ExcludedItemIds", "" } }, warnings);
    CHECK(p.ExcludedItemIds.empty());
    CHECK(warnings.empty());
}

} // namespace

int main()
{
    Defaults();
    ReadsValues();
    BadValuesWarnAndKeepDefaults();
    EmptyListIsNoExclusions();

    if (failures == 0)
        std::printf("settings_test: all checks passed\n");
    return failures == 0 ? 0 : 1;
}
