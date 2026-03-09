import * as React from 'react';
import { useForm } from 'react-hook-form';
import { zodResolver } from '@hookform/resolvers/zod';
import { z } from 'zod';
import { Plus, Edit, Trash2 } from 'lucide-react';
import { DataTable } from '../components/DataTable';
import type { Column } from '../components/DataTable';
import { FormDialog } from '../components/FormDialog';
import { ConfirmDialog } from '../components/ConfirmDialog';
import { FormInput } from '../components/FormInput';
import { Button } from '../components/ui/Button';
import { useControls, useCreateControl, useUpdateControl, useDeleteControl } from '../hooks/useControls';
import type { Control } from '../api/types';

const controlSchema = z.object({
  name: z.string().min(1, 'Name is required'),
  code: z.string().min(1, 'Code is required'),
});

type ControlFormValues = z.infer<typeof controlSchema>;

export default function ControlsPage() {
  const { data: controlsData, isLoading, refetch } = useControls();
  const controls = controlsData || [];

  const [isFormOpen, setIsFormOpen] = React.useState(false);
  const [isConfirmOpen, setIsConfirmOpen] = React.useState(false);
  const [editingControl, setEditingControl] = React.useState<Control | null>(null);
  const [deletingControl, setDeletingControl] = React.useState<Control | null>(null);

  const {
    register,
    handleSubmit,
    reset,
    formState: { errors },
  } = useForm<ControlFormValues>({
    resolver: zodResolver(controlSchema),
    defaultValues: {
      name: '',
      code: '',
    },
  });

  const createControlMutation = useCreateControl();
  const updateControlMutation = useUpdateControl();
  const deleteControlMutation = useDeleteControl();

  const onSubmit = async (values: ControlFormValues) => {
    try {
      if (editingControl) {
        await updateControlMutation.mutate({ id: editingControl.id, data: values });
      } else {
        await createControlMutation.mutate(values);
      }
      setIsFormOpen(false);
      refetch();
    } catch (err: any) {
      alert(`Error: ${err.message}`);
    }
  };

  const handleCreate = () => {
    setEditingControl(null);
    reset({
      name: '',
      code: '',
    });
    setIsFormOpen(true);
  };

  const handleEdit = (ctrl: Control) => {
    setEditingControl(ctrl);
    reset({
      name: ctrl.name,
      code: ctrl.code,
    });
    setIsFormOpen(true);
  };

  const handleDelete = (ctrl: Control) => {
    setDeletingControl(ctrl);
    setIsConfirmOpen(true);
  };

  const confirmDelete = async () => {
    if (deletingControl) {
      try {
        await deleteControlMutation.mutate(deletingControl.id);
        setIsConfirmOpen(false);
        refetch();
      } catch (err: any) {
        alert(`Error: ${err.message}`);
      }
    }
  };

  const columns: Column<Control>[] = [
    { header: 'Name', accessorKey: 'name', sortable: true },
    { header: 'Code', accessorKey: 'code', sortable: true },
    {
      header: 'Actions',
      accessorKey: 'id',
      cell: (ctrl) => (
        <div className="flex space-x-2" onClick={(e) => e.stopPropagation()}>
          <Button variant="ghost" size="icon" onClick={() => handleEdit(ctrl)}>
            <Edit className="w-4 h-4" />
          </Button>
          <Button variant="ghost" size="icon" onClick={() => handleDelete(ctrl)}>
            <Trash2 className="w-4 h-4 text-destructive" />
          </Button>
        </div>
      )
    }
  ];

  return (
    <div className="container mx-auto py-8">
      <div className="flex justify-between items-center mb-6">
        <h1 className="text-3xl font-bold">Controls</h1>
        <Button onClick={handleCreate}>
          <Plus className="w-4 h-4 mr-2" />
          Add Control
        </Button>
      </div>

      <DataTable
        columns={columns}
        data={controls}
        onRowClick={handleEdit}
        isLoading={isLoading}
      />

      <FormDialog
        title={editingControl ? 'Edit Control' : 'Add Control'}
        open={isFormOpen}
        onOpenChange={setIsFormOpen}
        onSave={() => handleSubmit(onSubmit)()}
        onCancel={() => setIsFormOpen(false)}
        loading={createControlMutation.isLoading || updateControlMutation.isLoading}
      >
        <form className="space-y-4" onSubmit={handleSubmit(onSubmit)}>
          <FormInput
            label="Name (e.g. S1)"
            {...register('name')}
            error={errors.name?.message}
            required
          />

          <FormInput
            label="Code (e.g. 31)"
            {...register('code')}
            error={errors.code?.message}
            required
          />
        </form>
      </FormDialog>

      <ConfirmDialog
        title="Delete Control"
        description={`Are you sure you want to delete the control "${deletingControl?.name}" (${deletingControl?.code})? This action cannot be undone.`}
        open={isConfirmOpen}
        onOpenChange={setIsConfirmOpen}
        onConfirm={confirmDelete}
        onCancel={() => setIsConfirmOpen(false)}
        loading={deleteControlMutation.isLoading}
      />
    </div>
  );
}
