# React Form and Hook Patterns

## useMutation (Custom useApi Hook)
The `useMutation` hook provides a `mutate` function that takes a single `variables` argument. Unlike standard TanStack Query, it does not support options like `onSuccess` or `onError` as a second argument to `mutate`.

**Correct usage:**
```tsx
const mutation = useMutation(variables => api.updateEntity(id, variables));

// Use async/await to handle success/error
const onSubmit = async (data) => {
  try {
    await mutation.mutate(data);
    onSuccess();
  } catch (err) {
    onError(err);
  }
};
```

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
