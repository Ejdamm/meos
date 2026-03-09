# Data Table Patterns for MeOS Migration

This skill documents the standard patterns for implementing the `DataTable` component in the MeOS Web Frontend.

## Component Features

- **Sorting**: Enabled via the `sortable` prop on a column definition.
- **Filtering**: A built-in search bar filters all displayed columns.
- **Pagination**: Controlled via the `pageSize` prop (default is 10).
- **Selection**: Enabled via the `enableSelection` prop.

## Standard Column Definition

```tsx
const columns: Column<Runner>[] = [
  { header: 'Name', accessorKey: 'name', sortable: true },
  { 
    header: 'Club', 
    accessorKey: 'clubName',
    cell: (r) => r.clubName || 'None',
    sortable: true 
  },
  {
    header: 'Actions',
    accessorKey: 'id',
    cell: (r) => (
      <div className="flex space-x-2">
        <Button onClick={() => handleEdit(r)}>Edit</Button>
      </div>
    )
  }
];
```

## Implementing Row Selection

To enable bulk operations, use the selection features of `DataTable`.

```tsx
const [selectedItems, setSelectedItems] = useState<Runner[]>([]);

<DataTable
  columns={columns}
  data={runners}
  enableSelection
  selectedItems={selectedItems}
  onSelectionChange={setSelectedItems}
  getItemId={(item) => item.id}
/>
```

### Bulk Actions Toolbar

When items are selected, display a toolbar above the table for batch operations.

```tsx
{selectedItems.length > 0 && (
  <div className="bg-blue-50 p-4 mb-4 flex justify-between items-center">
    <span>{selectedItems.length} items selected</span>
    <div className="flex gap-2">
      <Button onClick={handleBulkUpdate}>Bulk Update</Button>
      <Button variant="ghost" onClick={() => setSelectedItems([])}>Cancel</Button>
    </div>
  </div>
)}
```

## Integration with API

When implementing bulk updates, ensure the API client and MSW handlers support batch operations (e.g., `PATCH /runners/bulk` with a list of IDs).
