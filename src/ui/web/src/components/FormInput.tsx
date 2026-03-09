import * as React from "react"
import { Input } from "./ui/Input";
import type { InputProps } from "./ui/Input"
import { FormField } from "./FormField"

interface FormInputProps extends InputProps {
  label?: string
  error?: string
}

const FormInput = React.forwardRef<HTMLInputElement, FormInputProps>(
  ({ label, error, className, ...props }, ref) => {
    return (
      <FormField label={label} error={error} required={props.required}>
        <Input ref={ref} className={className} {...props} />
      </FormField>
    )
  }
)
FormInput.displayName = "FormInput"

export { FormInput }
