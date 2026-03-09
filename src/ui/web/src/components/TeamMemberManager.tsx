import { Trash2, ArrowUp, ArrowDown } from 'lucide-react';
import { Button } from './ui/Button';
import { SearchableSelect } from './SearchableSelect';
import { FormField } from './FormField';
import type { Runner } from '../api/types';

interface TeamMemberManagerProps {
  value: number[];
  onChange: (value: number[]) => void;
  availableRunners: Runner[];
  error?: string;
  label?: string;
  required?: boolean;
}

export function TeamMemberManager({
  value = [],
  onChange,
  availableRunners,
  error,
  label = "Team Members",
  required,
}: TeamMemberManagerProps) {
  const handleAddMember = (runnerId: string) => {
    onChange([...value, Number(runnerId)]);
  };

  const handleRemoveMember = (index: number) => {
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

  const runnerOptions = availableRunners.map(r => ({
    label: `${r.name} (${r.clubName || 'No club'})`,
    value: String(r.id),
  }));

  return (
    <FormField label={label} error={error} required={required}>
      <div className="space-y-3 p-4 border rounded-md bg-muted/30">
        {value.length === 0 ? (
          <p className="text-sm text-muted-foreground text-center py-4 italic">
            No members added yet. Add members from the dropdown below.
          </p>
        ) : (
          <div className="space-y-2 max-h-60 overflow-y-auto pr-2">
            {value.map((runnerId, index) => {
              const runner = availableRunners.find(r => r.id === runnerId);
              return (
                <div 
                  key={`${runnerId}-${index}`} 
                  className="flex items-center justify-between p-2 bg-background border rounded-sm group"
                >
                  <div className="flex items-center gap-3">
                    <span className="text-xs font-bold text-muted-foreground w-6">
                      {index + 1}
                    </span>
                    <span className="font-medium">
                      {runner ? `${runner.name} (${runner.clubName || 'No club'})` : `Unknown Runner (${runnerId})`}
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
                      onClick={() => handleRemoveMember(index)}
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
            placeholder="Add runner..."
            searchPlaceholder="Search runners by name..."
            options={runnerOptions}
            onValueChange={handleAddMember}
          />
        </div>
      </div>
    </FormField>
  );
}
