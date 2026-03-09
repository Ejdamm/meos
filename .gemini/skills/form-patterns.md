# Form Patterns for MeOS Migration

This skill documents the standard patterns for implementing forms in the MeOS Web Frontend.

## Component Selection

| Use Case | Component |
|----------|-----------|
| Text, Number, Date inputs | `FormInput` |
| Small, static lists (e.g. Start Method) | `FormSelect` |
| Large or dynamic lists (e.g. Clubs, Runners) | `SearchableSelect` |
| Grouping related fields | `FormField` (manual wrap) |

## Form Management

Always use `react-hook-form` with `zod` for validation.

### Schema Definition

```typescript
import { z } from 'zod';

const schema = z.object({
  name: z.string().min(1, 'Name is required'),
  email: z.string().email('Invalid email address'),
  count: z.number().int().positive().optional(),
});
```

### Form Initialization

```tsx
const { register, handleSubmit, formState: { errors }, reset } = useForm({
  resolver: zodResolver(schema),
  defaultValues: { ... },
});
```

### Dynamic Options (SearchableSelect)

When using `SearchableSelect` with API data, ensure options are mapped to `{ label: string, value: string }`.

```tsx
const clubOptions = clubs.map(c => ({ label: c.name, value: c.id.toString() }));

<SearchableSelect
  label="Club"
  options={clubOptions}
  onValueChange={(val) => setValue('clubId', Number(val))}
  value={watch('clubId')?.toString()}
  error={errors.clubId?.message}
/>
```

## useMutation (Custom useApi Hook)

The `useMutation` hook provides a `mutate` function that takes a single `variables` argument. Unlike standard TanStack Query, it does not support options like `onSuccess` or `onError` as a second argument to `mutate`.

**Standard Update Pattern:**
```tsx
// hook definition
export function useUpdateEntity() {
  return useMutation(({ id, data }: { id: number; data: Partial<Entity> }) =>
    api.updateEntity(id, data)
  );
}

// component usage
const updateMutation = useUpdateEntity();
const onSubmit = async (values) => {
  await updateMutation.mutate({ id: editingItem.id, data: values });
};
```

Use async/await to handle success/error and trigger UI updates like refetching data or closing dialogs.

## FormSelect with Numeric IDs

The `FormSelect` component (built on Radix Select) uses strings for its `value` and `onValueChange` props. If your data uses numeric IDs, convert them:

```tsx
<Controller
  name="classId"
  control={control}
  render={({ field }) => (
    <FormSelect
      options={options}
      value={String(field.value)}
      onValueChange={val => field.onChange(Number(val))}
    />
  )}
/>
```

## FormInput with Numeric Values

When using `type="number"` in `FormInput`, tell `react-hook-form` to handle it as a number:

```tsx
<FormInput
  label="Sort Order"
  type="number"
  {...register('sortOrder', { valueAsNumber: true })}
/>
```

## Success/Error Feedback

For simple cases, use `alert()` or a dedicated `Toast` component (if available). Ensure `isLoading` states are handled on submit buttons.
