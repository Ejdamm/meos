import * as React from "react"
import { Label } from "./ui/Label"
import { cn } from "../lib/utils"

interface FormFieldProps {
  label?: string
  error?: string
  children: React.ReactNode
  className?: string
  required?: boolean
}

const FormField = React.forwardRef<HTMLDivElement, FormFieldProps>(
  ({ label, error, children, className, required }, ref) => {
    return (
      <div ref={ref} className={cn("space-y-2", className)}>
        {label && (
          <Label className={cn(error && "text-destructive")}>
            {label}
            {required && <span className="ml-1 text-destructive">*</span>}
          </Label>
        )}
        {children}
        {error && (
          <p className="text-[0.8rem] font-medium text-destructive">{error}</p>
        )}
      </div>
    )
  }
)
FormField.displayName = "FormField"

export { FormField }
