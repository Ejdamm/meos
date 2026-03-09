import * as React from "react"
import { Check, ChevronsUpDown } from "lucide-react"
import { cn } from "../lib/utils"
import { Button } from "./ui/Button"
import { Popover, PopoverContent, PopoverTrigger } from "./ui/Popover"
import { FormField } from "./FormField"
import { Input } from "./ui/Input"

interface Option {
  label: string
  value: string
}

interface SearchableSelectProps {
  label?: string
  error?: string
  options: Option[]
  placeholder?: string
  searchPlaceholder?: string
  emptyMessage?: string
  value?: string
  onValueChange?: (value: string) => void
  disabled?: boolean
  required?: boolean
  className?: string
}

const SearchableSelect = React.forwardRef<HTMLButtonElement, SearchableSelectProps>(
  (
    {
      label,
      error,
      options,
      placeholder = "Select an option...",
      searchPlaceholder = "Search...",
      emptyMessage = "No option found.",
      value,
      onValueChange,
      disabled,
      required,
      className,
    },
    ref
  ) => {
    const [open, setOpen] = React.useState(false)
    const [search, setSearch] = React.useState("")

    const filteredOptions = React.useMemo(() => {
      if (!search) return options
      return options.filter((option) =>
        option.label.toLowerCase().includes(search.toLowerCase())
      )
    }, [options, search])

    const selectedOption = React.useMemo(
      () => options.find((option) => option.value === value),
      [options, value]
    )

    return (
      <FormField label={label} error={error} required={required} className={className}>
        <Popover open={open} onOpenChange={setOpen}>
          <PopoverTrigger asChild>
            <Button
              variant="outline"
              role="combobox"
              aria-expanded={open}
              className="w-full justify-between"
              disabled={disabled}
              ref={ref}
            >
              {selectedOption ? selectedOption.label : placeholder}
              <ChevronsUpDown className="ml-2 h-4 w-4 shrink-0 opacity-50" />
            </Button>
          </PopoverTrigger>
          <PopoverContent className="w-[var(--radix-popover-trigger-width)] p-0">
            <div className="flex flex-col">
              <div className="p-2 border-b">
                <Input
                  placeholder={searchPlaceholder}
                  value={search}
                  onChange={(e) => setSearch(e.target.value)}
                  className="h-8"
                />
              </div>
              <div className="max-h-60 overflow-y-auto p-1">
                {filteredOptions.length === 0 ? (
                  <p className="p-2 text-sm text-muted-foreground text-center">
                    {emptyMessage}
                  </p>
                ) : (
                  filteredOptions.map((option) => (
                    <div
                      key={option.value}
                      className={cn(
                        "relative flex cursor-default select-none items-center rounded-sm px-2 py-1.5 text-sm outline-none hover:bg-accent hover:text-accent-foreground",
                        value === option.value && "bg-accent"
                      )}
                      onClick={() => {
                        onValueChange?.(option.value)
                        setOpen(false)
                        setSearch("")
                      }}
                    >
                      <Check
                        className={cn(
                          "mr-2 h-4 w-4",
                          value === option.value ? "opacity-100" : "opacity-0"
                        )}
                      />
                      {option.label}
                    </div>
                  ))
                )}
              </div>
            </div>
          </PopoverContent>
        </Popover>
      </FormField>
    )
  }
)
SearchableSelect.displayName = "SearchableSelect"

export { SearchableSelect }
