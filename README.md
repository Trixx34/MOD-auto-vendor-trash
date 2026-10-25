# MOD-auto-vendor-trash

Sells grey items as soon as a player loots them, and tells the player what they got.

When a player loots a grey (Poor quality) item, the module asks the core to sell it, and the player receives a chat message:

    [Item Name] was sold automatically, got 1g 20s 5c.

Units of zero are left out, except copper (`got 0c`). The money is credited by the core from the item's own sell price; the module
never sets an amount.

## What is sold

A looted item is sold when all of these hold (`src/vendor_policy.cpp`):

- its quality is Poor (grey) and it has a sell price;
- it is not a quest item (item class 12), which is never sold;
- the stack is worth at least `MinValueCopper` (sell price times count);
- its entry is not in `ExcludedItemIds`.

The core re-checks the sale (the item is still in the player's bags with the same entry and count, the gold cap would not be exceeded)
and says why when it refuses; the module writes a line to its stderr and the item stays in the bags.

## Bots

The loot event fires inside the core's item-store step, so it fires for a bot's loot too. When the bots module loots a corpse for a bot,
this module sells the bot's grey drops and the message goes to a bot with no connection, so it is simply not shown. A bot's own vendor
visit then only has leftover items to sell.

## Configuration

The core sends the `Module.auto-vendor-trash.<Key>` entries from `worldserver.conf.d/modules.d/auto-vendor-trash.conf` (under the config
directory), with the prefix removed. See `config/modules.d/auto-vendor-trash.conf.example`:

- `MinValueCopper` (default 0): grey stacks worth less than this, in copper (sell price times count), are kept. 0 sells every grey item.
- `ExcludedItemIds` (default none): comma-separated item entries that are never sold.

A bad value keeps the default and writes a line to the module's stderr. A re-delivered config applies to the next loot.

## Hooks and needs

The module registers the Tier 1 hook `PlayerScript.OnLootItem` and uses the `SellItem` and `SendPlayerMessage` actions
(`forever-module-sdk`, protocol 17). It needs a core with the module system.

## Build

    cmake -S . -B build -DFOREVER_MODULE_SDK_SOURCE=/path/to/forever-module-sdk
    cmake --build build
    ctest --test-dir build

Without `FOREVER_MODULE_SDK_SOURCE`, CMake fetches `Trixx34/forever-module-sdk` (`main`).

## Layout

- `module.json`: manifest (name, protocol version, hook).
- `src/vendor_policy.{h,cpp}`: which items are sold, and the sale message. No SDK dependency.
- `src/vendor_settings.{h,cpp}`: reads the config.
- `src/main.cpp`: connects to the core, handles the loot event, sells and reports.
- `tests/policy_test.cpp`, `tests/settings_test.cpp`: unit tests.

## Sales summary

Every `SummaryMinutes` minutes (default 10, `0` = off) the module writes one line to its log with what it sold in that time and what it was worth:
`auto-vendor-trash: 42 items sold in the last 10 minutes. Total value 1g 20s 5c.` A period with no sale writes nothing. The same line goes to the core's `Server.log` as `[auto-vendor-trash] 42 items sold ...` (needs a core with the log action).
