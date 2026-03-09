# React Form and Hook Patterns

## useMutation (Custom useApi Hook)
The `useMutation` hook provides a `mutate` function that takes a single `variables` argument. Unlike standard TanStack Query, it does not support options like `onSuccess` or `onError` as a second argument to `mutate`.

**Standard Update Pattern:**
For update operations, pass an object containing both the `id` and the `data`.

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

**Correct usage (Async/Await):**
Use async/await to handle success/error and trigger UI updates like refetching data or closing dialogs.

## FormSelect with Numeric IDs
The `FormSelect` component (built on Radix Select) uses strings for its `value` and `onValueChange` props. If your underlying data uses numeric IDs (as most orienteering entities do), you must convert them.

**Pattern:**
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
When using `type="number"` in `FormInput`, tell `react-hook-form` to handle it as a number in the state.

**Pattern:**
```tsx
<FormInput
  label="Sort Order"
  type="number"
  {...register('sortOrder', { valueAsNumber: true })}
/>
```
