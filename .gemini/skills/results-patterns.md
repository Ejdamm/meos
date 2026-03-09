# Results and Start List Patterns

This skill documents patterns for displaying competition results and start lists in the MeOS Web Frontend.

## Results View Patterns

- **Grouping by Class**: Results should be grouped by class. If no class is selected, display headers for each class and a table for each.
- **Filtering by Class**: Use a `FormSelect` to allow the user to filter results to a single class.
- **Split Times**: Results must include split times. Use an expandable row pattern to show splits without cluttering the main list.
- **Orienteering Formatting**:
  - `Place`: Show the numerical place for `OK` status, otherwise show status code (MP, DNF, etc.).
  - `Time`: Show running time for `OK` status.
  - `After`: Show time behind the leader as `+mm:ss` (omit for the leader).

### Expandable Splits Implementation

Use a `React.Fragment` to inject an extra row below the runner's main row.

```tsx
const renderRow = (r: Result) => {
  const isExpanded = expandedRunners.has(r.runnerId);
  return (
    <React.Fragment key={r.runnerId}>
      <tr onClick={() => toggleRunner(r.runnerId)}>
        {/* Main result cells */}
      </tr>
      {isExpanded && (
        <tr>
          <td colSpan={columns.length}>
            <div className="splits-table">
              {/* Split time rows */}
            </div>
          </td>
        </tr>
      )}
    </React.Fragment>
  );
};
```

## Start List View Patterns

- **Grouping by Class**: Similar to results, start lists should be grouped by class.
- **Sorting**: Default sort order should be by `startTime` (ascending).
- **Filtering**: Support filtering by class using a standard `FormSelect`.

## Print Styles

Use `@media print` to ensure clean printouts for physical distribution at competitions.

- Hide navigation, headers, and interactive elements (buttons, filters).
- Ensure tables don't break across pages where possible (`break-inside-avoid`).
- Remove shadows and use simple borders for better readability on paper.
- Adjust font sizes (e.g., `10pt`) for standard A4/Letter printing.

```css
@media print {
  body { color: black; background: white; font-size: 10pt; }
  .print\:hidden { display: none !important; }
  .shadow-md { box-shadow: none !important; }
  tr { page-break-inside: avoid; border-bottom: 1px solid #eee; }
}
```
