#include "vendor_settings.h"

#include <charconv>
#include <system_error>

namespace auto_vendor {

namespace {

// Parses a whole decimal number. Returns false, with the text that failed, if it is not one.
template <typename T>
bool ParseNumber(std::string const& text, T& value)
{
    T parsed = 0;
    auto const [end, error] = std::from_chars(text.data(), text.data() + text.size(), parsed);
    if (error != std::errc() || end != text.data() + text.size() || text.empty())
        return false;
    value = parsed;
    return true;
}

} // namespace

Policy ParseVendorSettings(std::unordered_map<std::string, std::string> const& config, std::vector<std::string>& warnings)
{
    Policy policy;

    if (auto const it = config.find("MinValueCopper"); it != config.end())
    {
        std::uint64_t value = 0;
        if (ParseNumber(it->second, value))
            policy.MinValueCopper = value;
        else
            warnings.push_back("Module.auto-vendor-trash.MinValueCopper: '" + it->second + "' is not a whole number; using 0.");
    }

    if (auto const it = config.find("SummaryMinutes"); it != config.end())
    {
        std::uint64_t value = 0;
        if (ParseNumber(it->second, value) && value <= 1440)
            policy.SummaryMinutes = static_cast<std::uint32_t>(value);
        else
            warnings.push_back("Module.auto-vendor-trash.SummaryMinutes: '" + it->second + "' is not a number of minutes from 0 to 1440; using 10.");
    }

    if (auto const it = config.find("ExcludedItemIds"); it != config.end())
    {
        // Split on commas; spaces around entries are allowed, empty entries are ignored.
        std::string const& list = it->second;
        std::size_t start = 0;
        while (start <= list.size())
        {
            std::size_t comma = list.find(',', start);
            if (comma == std::string::npos)
                comma = list.size();
            std::string entry = list.substr(start, comma - start);
            entry.erase(0, entry.find_first_not_of(' '));
            entry.erase(entry.find_last_not_of(' ') + 1);

            if (!entry.empty())
            {
                std::uint32_t id = 0;
                if (ParseNumber(entry, id) && id != 0)
                    policy.ExcludedItemIds.push_back(id);
                else
                    warnings.push_back("Module.auto-vendor-trash.ExcludedItemIds: '" + entry + "' is not an item entry; skipped.");
            }
            start = comma + 1;
        }
    }

    return policy;
}

} // namespace auto_vendor
