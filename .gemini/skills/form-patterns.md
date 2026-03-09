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

## Success/Error Feedback

For simple cases, use `alert()` or a dedicated `Toast` component (if available). Ensure `isLoading` states are handled on submit buttons.
