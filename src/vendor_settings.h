// Reads the auto-vendor policy from the module's config (Module.auto-vendor-trash.<Key>, see
// config/modules.d/auto-vendor-trash.conf.example). No SDK dependency, so it is unit-tested (tests/settings_test.cpp).
#ifndef AUTO_VENDOR_SETTINGS_H
#define AUTO_VENDOR_SETTINGS_H

#include "vendor_policy.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace auto_vendor {

// Keys: MinValueCopper (whole copper, default 0), ExcludedItemIds (comma-separated item entries, default none) and SummaryMinutes
// (0..1440, default 10; 0 turns the sales summary line off).
// A bad value keeps the default and adds one line to warnings.
Policy ParseVendorSettings(std::unordered_map<std::string, std::string> const& config, std::vector<std::string>& warnings);

} // namespace auto_vendor

#endif // AUTO_VENDOR_SETTINGS_H
