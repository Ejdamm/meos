# Card and Punch Migration Skill

## SI-Card Data Handling
- `SICard` and `SIPunch` are the primary structures for raw card data.
- Use `SICard::calculateHash()` to identify unique readout instances. This hash should be stored in `oCard::readId`.
- Always clear `SICard` objects with `clear(nullptr)` to ensure all fields (especially codes set to -1) are correctly initialized.

## oCard and oFreePunch Relationships
- `oCard` belongs to an `oRunner` or `oTeam`.
- `oFreePunch` is managed by `oEvent` and indexed via `oEvent::punchIndex`.
- Use `oFreePunch::rehashPunches(oe, cardNo, newPunch)` to maintain the lookup index when punches change.

## String Substitution in Localizer
- The migrated `Localizer` supports `#` substitution for multiple placeholders.
- Syntax: `lang.tl(L"Base String with X, Y, Z, W#ValX#ValY#ValZ#ValW")`.
- This is useful for migrating legacy `swprintf` calls that inserted values into translated strings.

## Time and Date Utilities
- Use `formatDate(yyyymmdd, iso)` for consistent date formatting.
- Use `_EmptyWString` (defined in `meos_util.h`) instead of `L""` when returning constant empty wide strings from utility-like functions to match legacy patterns.
