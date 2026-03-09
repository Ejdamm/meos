import { Trash2, ArrowUp, ArrowDown } from 'lucide-react';
import { Button } from './ui/Button';
import { SearchableSelect } from './SearchableSelect';
import { FormField } from './FormField';
import type { Control } from '../api/types';

interface ControlSequenceBuilderProps {
  value: number[];
  onChange: (value: number[]) => void;
  availableControls: Control[];
  error?: string;
  label?: string;
  required?: boolean;
}

export function ControlSequenceBuilder({
  value = [],
  onChange,
  availableControls,
  error,
  label = "Control Sequence",
  required,
}: ControlSequenceBuilderProps) {
  const handleAddControl = (controlId: string) => {
    onChange([...value, Number(controlId)]);
  };

  const handleRemoveControl = (index: number) => {
    const newValue = [...value];
    newValue.splice(index, 1);
    onChange(newValue);
  };

  const handleMoveUp = (index: number) => {
    if (index === 0) return;
    const newValue = [...value];
    [newValue[index - 1], newValue[index]] = [newValue[index], newValue[index - 1]];
    onChange(newValue);
  };

  const handleMoveDown = (index: number) => {
    if (index === value.length - 1) return;
    const newValue = [...value];
    [newValue[index], newValue[index + 1]] = [newValue[index + 1], newValue[index]];
    onChange(newValue);
  };

  const controlOptions = availableControls.map(c => ({
    label: `${c.code} - ${c.name}`,
    value: String(c.id),
  }));

  return (
    <FormField label={label} error={error} required={required}>
      <div className="space-y-3 p-4 border rounded-md bg-muted/30">
        {value.length === 0 ? (
          <p className="text-sm text-muted-foreground text-center py-4 italic">
            No controls added yet. Add controls from the dropdown below.
          </p>
        ) : (
          <div className="space-y-2 max-h-60 overflow-y-auto pr-2">
            {value.map((controlId, index) => {
              const control = availableControls.find(c => c.id === controlId);
              return (
                <div 
                  key={`${controlId}-${index}`} 
                  className="flex items-center justify-between p-2 bg-background border rounded-sm group"
                >
                  <div className="flex items-center gap-3">
                    <span className="text-xs font-bold text-muted-foreground w-6">
                      {index + 1}
                    </span>
                    <span className="font-medium">
                      {control ? `${control.code} - ${control.name}` : `Unknown (${controlId})`}
                    </span>
                  </div>
                  <div className="flex items-center gap-1 opacity-0 group-hover:opacity-100 transition-opacity">
                    <Button
                      type="button"
                      variant="ghost"
                      size="icon"
                      className="h-7 w-7"
                      onClick={() => handleMoveUp(index)}
                      disabled={index === 0}
                    >
                      <ArrowUp className="h-4 w-4" />
                    </Button>
                    <Button
                      type="button"
                      variant="ghost"
                      size="icon"
                      className="h-7 w-7"
                      onClick={() => handleMoveDown(index)}
                      disabled={index === value.length - 1}
                    >
                      <ArrowDown className="h-4 w-4" />
                    </Button>
                    <Button
                      type="button"
                      variant="ghost"
                      size="icon"
                      className="h-7 w-7"
                      onClick={() => handleRemoveControl(index)}
                    >
                      <Trash2 className="h-4 w-4 text-destructive" />
                    </Button>
                  </div>
                </div>
              );
            })}
          </div>
        )}

        <div className="pt-2 border-t">
          <SearchableSelect
            placeholder="Add control..."
            searchPlaceholder="Search controls by name or code..."
            options={controlOptions}
            onValueChange={handleAddControl}
          />
        </div>
      </div>
    </FormField>
  );
}
