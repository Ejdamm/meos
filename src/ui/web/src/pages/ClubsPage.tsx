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
import { useClubs, useCreateClub, useUpdateClub, useDeleteClub } from '../hooks/useClubs';
import type { Club } from '../api/types';

const clubSchema = z.object({
  name: z.string().min(1, 'Name is required'),
  nationality: z.string().optional().or(z.literal('')),
});

type ClubFormValues = z.infer<typeof clubSchema>;

export default function ClubsPage() {
  const { data: clubsData, isLoading, refetch } = useClubs();
  const clubs = clubsData || [];

  const [isFormOpen, setIsFormOpen] = React.useState(false);
  const [isConfirmOpen, setIsConfirmOpen] = React.useState(false);
  const [editingClub, setEditingClub] = React.useState<Club | null>(null);
  const [deletingClub, setDeletingClub] = React.useState<Club | null>(null);

  const {
    register,
    handleSubmit,
    reset,
    formState: { errors },
  } = useForm<ClubFormValues>({
    resolver: zodResolver(clubSchema),
    defaultValues: {
      name: '',
      nationality: '',
    },
  });

  const createClubMutation = useCreateClub();
  const updateClubMutation = useUpdateClub();
  const deleteClubMutation = useDeleteClub();

  const onSubmit = async (values: ClubFormValues) => {
    try {
      if (editingClub) {
        await updateClubMutation.mutate({ id: editingClub.id, data: values });
      } else {
        await createClubMutation.mutate(values);
      }
      setIsFormOpen(false);
      refetch();
    } catch (err: any) {
      alert(`Error: ${err.message}`);
    }
  };

  const handleCreate = () => {
    setEditingClub(null);
    reset({
      name: '',
      nationality: '',
    });
    setIsFormOpen(true);
  };

  const handleEdit = (club: Club) => {
    setEditingClub(club);
    reset({
      name: club.name,
      nationality: club.nationality || '',
    });
    setIsFormOpen(true);
  };

  const handleDelete = (club: Club) => {
    setDeletingClub(club);
    setIsConfirmOpen(true);
  };

  const confirmDelete = async () => {
    if (deletingClub) {
      try {
        await deleteClubMutation.mutate(deletingClub.id);
        setIsConfirmOpen(false);
        refetch();
      } catch (err: any) {
        alert(`Error: ${err.message}`);
      }
    }
  };

  const columns: Column<Club>[] = [
    { header: 'Name', accessorKey: 'name', sortable: true },
    { header: 'Nationality', accessorKey: 'nationality', sortable: true },
    {
      header: 'Actions',
      accessorKey: 'id',
      cell: (club) => (
        <div className="flex space-x-2" onClick={(e) => e.stopPropagation()}>
          <Button variant="ghost" size="icon" onClick={() => handleEdit(club)}>
            <Edit className="w-4 h-4" />
          </Button>
          <Button variant="ghost" size="icon" onClick={() => handleDelete(club)}>
            <Trash2 className="w-4 h-4 text-destructive" />
          </Button>
        </div>
      )
    }
  ];

  return (
    <div className="container mx-auto py-8">
      <div className="flex justify-between items-center mb-6">
        <h1 className="text-3xl font-bold">Clubs</h1>
        <Button onClick={handleCreate}>
          <Plus className="w-4 h-4 mr-2" />
          Add Club
        </Button>
      </div>

      <DataTable
        columns={columns}
        data={clubs}
        onRowClick={handleEdit}
        isLoading={isLoading}
      />

      <FormDialog
        title={editingClub ? 'Edit Club' : 'Add Club'}
        open={isFormOpen}
        onOpenChange={setIsFormOpen}
        onSave={() => handleSubmit(onSubmit)()}
        onCancel={() => setIsFormOpen(false)}
        loading={createClubMutation.isLoading || updateClubMutation.isLoading}
      >
        <form className="space-y-4" onSubmit={handleSubmit(onSubmit)}>
          <FormInput
            label="Name"
            {...register('name')}
            error={errors.name?.message}
            required
          />

          <FormInput
            label="Nationality"
            {...register('nationality')}
            error={errors.nationality?.message}
          />
        </form>
      </FormDialog>

      <ConfirmDialog
        title="Delete Club"
        description={`Are you sure you want to delete the club "${deletingClub?.name}"? This action cannot be undone.`}
        open={isConfirmOpen}
        onOpenChange={setIsConfirmOpen}
        onConfirm={confirmDelete}
        onCancel={() => setIsConfirmOpen(false)}
        loading={deleteClubMutation.isLoading}
      />
    </div>
  );
}
