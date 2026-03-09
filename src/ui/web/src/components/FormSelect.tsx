import * as React from "react"
import {
  Select,
  SelectContent,
  SelectItem,
  SelectTrigger,
  SelectValue,
} from "./ui/Select"
import { FormField } from "./FormField"

interface Option {
  label: string
  value: string
}

interface FormSelectProps {
  label?: string
  error?: string
  options: Option[]
  placeholder?: string
  value?: string
  onValueChange?: (value: string) => void
  disabled?: boolean
  required?: boolean
  className?: string
}

const FormSelect = React.forwardRef<HTMLButtonElement, FormSelectProps>(
  (
    {
      label,
      error,
      options,
      placeholder,
      value,
      onValueChange,
      disabled,
      required,
      className,
    },
    ref
  ) => {
    return (
      <FormField label={label} error={error} required={required} className={className}>
        <Select
          value={value}
          onValueChange={onValueChange}
          disabled={disabled}
          required={required}
        >
          <SelectTrigger ref={ref}>
            <SelectValue placeholder={placeholder} />
          </SelectTrigger>
          <SelectContent>
            {options.map((option) => (
              <SelectItem key={option.value} value={option.value}>
                {option.label}
              </SelectItem>
            ))}
          </SelectContent>
        </Select>
      </FormField>
    )
  }
)
FormSelect.displayName = "FormSelect"

export { FormSelect }
