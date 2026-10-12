# MOD-auto-vendor-trash
Init
## Configuration

The core sends the `Module.auto-vendor-trash.<Key>` entries from `modules.d/auto-vendor-trash.conf` (next to
`worldserver.conf`), with the prefix removed. See `config/modules.d/auto-vendor-trash.conf.example`:

- `MinValueCopper` (default 0): grey stacks worth less than this, in copper (sell price times count), are kept.
- `ExcludedItemIds` (default none): comma-separated item entries that are never sold.

A bad value keeps the default and writes a line to the module's stderr. A re-delivered config applies to the next loot.
