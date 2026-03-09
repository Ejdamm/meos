import * as React from 'react';
import { useForm, Controller } from 'react-hook-form';
import { zodResolver } from '@hookform/resolvers/zod';
import { z } from 'zod';
import { Plus, Edit, Trash2 } from 'lucide-react';
import { DataTable } from '../components/DataTable';
import type { Column } from '../components/DataTable';
import { FormDialog } from '../components/FormDialog';
import { ConfirmDialog } from '../components/ConfirmDialog';
import { FormInput } from '../components/FormInput';
import { FormSelect } from '../components/FormSelect';
import { SearchableSelect } from '../components/SearchableSelect';
import { Button } from '../components/ui/Button';
import { useRunners, useCreateRunner, useUpdateRunner, useDeleteRunner } from '../hooks/useRunners';
import { useClasses } from '../hooks/useClasses';
import { useClubs } from '../hooks/useClubs';
import { RunnerStatus } from '../api/types';
import type { Runner } from '../api/types';

const runnerSchema = z.object({
  name: z.string().min(1, 'Name is required'),
  bib: z.string().optional().or(z.literal('')),
  clubId: z.string().optional().or(z.literal('')),
  classId: z.string().optional().or(z.literal('')),
  status: z.string(),
  startTime: z.string().optional().or(z.literal('')),
  cardNumber: z.string().optional().or(z.literal('')),
});

type RunnerFormValues = z.infer<typeof runnerSchema>;

const statusOptions = [
  { label: 'Unknown', value: String(RunnerStatus.Unknown) },
  { label: 'OK', value: String(RunnerStatus.OK) },
  { label: 'No Timing', value: String(RunnerStatus.NoTiming) },
  { label: 'MP', value: String(RunnerStatus.MP) },
  { label: 'DNF', value: String(RunnerStatus.DNF) },
  { label: 'DQ', value: String(RunnerStatus.DQ) },
  { label: 'MAX', value: String(RunnerStatus.MAX) },
  { label: 'Out Of Competition', value: String(RunnerStatus.OutOfCompetition) },
  { label: 'DNS', value: String(RunnerStatus.DNS) },
  { label: 'CANCEL', value: String(RunnerStatus.CANCEL) },
  { label: 'Not Competing', value: String(RunnerStatus.NotCompeting) },
];

export default function RunnersPage() {
  const { data: runnersData, isLoading: isLoadingRunners, refetch: refetchRunners } = useRunners();
  const { data: classesData, isLoading: isLoadingClasses } = useClasses();
  const { data: clubsData, isLoading: isLoadingClubs } = useClubs();

  const runners = runnersData || [];
  const classes = classesData || [];
  const clubs = clubsData || [];

  const [isFormOpen, setIsFormOpen] = React.useState(false);
  const [isConfirmOpen, setIsConfirmOpen] = React.useState(false);
  const [editingRunner, setEditingRunner] = React.useState<Runner | null>(null);
  const [deletingRunner, setDeletingRunner] = React.useState<Runner | null>(null);

  const {
    register,
    handleSubmit,
    reset,
    control,
    formState: { errors },
  } = useForm<RunnerFormValues>({
    resolver: zodResolver(runnerSchema),
    defaultValues: {
      name: '',
      bib: '',
      clubId: '',
      classId: '',
      status: String(RunnerStatus.OK),
      startTime: '',
      cardNumber: '',
    },
  });

  const createRunnerMutation = useCreateRunner();
  const updateRunnerMutation = useUpdateRunner();
  const deleteRunnerMutation = useDeleteRunner();

  const onSubmit = async (values: RunnerFormValues) => {
    const data: Partial<Runner> = {
      name: values.name,
      bib: values.bib || undefined,
      clubId: values.clubId ? Number(values.clubId) : undefined,
      classId: values.classId ? Number(values.classId) : undefined,
      status: Number(values.status) as RunnerStatus,
      startTime: values.startTime || undefined,
      cardNumber: values.cardNumber ? Number(values.cardNumber) : undefined,
    };

    try {
      if (editingRunner) {
        await updateRunnerMutation.mutate({ id: editingRunner.id, data });
      } else {
        await createRunnerMutation.mutate(data);
      }
      setIsFormOpen(false);
      refetchRunners();
    } catch (err: any) {
      alert(`Error: ${err.message}`);
    }
  };

  const handleCreate = () => {
    setEditingRunner(null);
    reset({
      name: '',
      bib: '',
      clubId: '',
      classId: '',
      status: String(RunnerStatus.OK),
      startTime: '',
      cardNumber: '',
    });
    setIsFormOpen(true);
  };

  const handleEdit = (runner: Runner) => {
    setEditingRunner(runner);
    reset({
      name: runner.name,
      bib: runner.bib || '',
      clubId: runner.clubId ? String(runner.clubId) : '',
      classId: runner.classId ? String(runner.classId) : '',
      status: String(runner.status),
      startTime: runner.startTime || '',
      cardNumber: runner.cardNumber ? String(runner.cardNumber) : '',
    });
    setIsFormOpen(true);
  };

  const handleDelete = (runner: Runner) => {
    setDeletingRunner(runner);
    setIsConfirmOpen(true);
  };

  const confirmDelete = async () => {
    if (deletingRunner) {
      try {
        await deleteRunnerMutation.mutate(deletingRunner.id);
        setIsConfirmOpen(false);
        refetchRunners();
      } catch (err: any) {
        alert(`Error: ${err.message}`);
      }
    }
  };

  const classOptions = classes.map(c => ({ label: c.name, value: String(c.id) }));
  const clubOptions = clubs.map(c => ({ label: c.name, value: String(c.id) }));

  const columns: Column<Runner>[] = [
    { header: 'Name', accessorKey: 'name', sortable: true },
    { 
      header: 'Club', 
      accessorKey: 'clubName',
      cell: (r) => r.clubName || clubs.find(c => c.id === r.clubId)?.name || 'None',
      sortable: true 
    },
    { 
      header: 'Class', 
      accessorKey: 'className',
      cell: (r) => r.className || classes.find(c => c.id === r.classId)?.name || 'None',
      sortable: true 
    },
    { header: 'Start Time', accessorKey: 'startTime', sortable: true },
    { header: 'Card', accessorKey: 'cardNumber', sortable: true },
    { 
      header: 'Status', 
      accessorKey: 'status',
      cell: (r) => statusOptions.find(o => o.value === String(r.status))?.label || 'Unknown',
      sortable: true 
    },
    {
      header: 'Actions',
      accessorKey: 'id',
      cell: (r) => (
        <div className="flex space-x-2" onClick={(e) => e.stopPropagation()}>
          <Button variant="ghost" size="icon" onClick={() => handleEdit(r)}>
            <Edit className="w-4 h-4" />
          </Button>
          <Button variant="ghost" size="icon" onClick={() => handleDelete(r)}>
            <Trash2 className="w-4 h-4 text-destructive" />
          </Button>
        </div>
      )
    }
  ];

  return (
    <div className="container mx-auto py-8">
      <div className="flex justify-between items-center mb-6">
        <h1 className="text-3xl font-bold">Runners</h1>
        <Button onClick={handleCreate}>
          <Plus className="w-4 h-4 mr-2" />
          Add Runner
        </Button>
      </div>

      <DataTable
        columns={columns}
        data={runners}
        onRowClick={handleEdit}
        isLoading={isLoadingRunners || isLoadingClasses || isLoadingClubs}
      />

      <FormDialog
        title={editingRunner ? 'Edit Runner' : 'Add Runner'}
        open={isFormOpen}
        onOpenChange={setIsFormOpen}
        onSave={() => handleSubmit(onSubmit)()}
        onCancel={() => setIsFormOpen(false)}
        loading={createRunnerMutation.isLoading || updateRunnerMutation.isLoading}
        size="lg"
      >
        <form className="grid grid-cols-1 md:grid-cols-2 gap-4" onSubmit={handleSubmit(onSubmit)}>
          <FormInput
            label="Name"
            {...register('name')}
            error={errors.name?.message}
            required
          />
          <FormInput
            label="Bib"
            {...register('bib')}
            error={errors.bib?.message}
          />
          
          <Controller
            name="clubId"
            control={control}
            render={({ field }) => (
              <SearchableSelect
                label="Club"
                options={clubOptions}
                value={field.value}
                onValueChange={field.onChange}
                error={errors.clubId?.message}
                placeholder="Select a club..."
              />
            )}
          />

          <Controller
            name="classId"
            control={control}
            render={({ field }) => (
              <SearchableSelect
                label="Class"
                options={classOptions}
                value={field.value}
                onValueChange={field.onChange}
                error={errors.classId?.message}
                placeholder="Select a class..."
              />
            )}
          />

          <Controller
            name="status"
            control={control}
            render={({ field }) => (
              <FormSelect
                label="Status"
                options={statusOptions}
                value={field.value}
                onValueChange={field.onChange}
                error={errors.status?.message}
              />
            )}
          />

          <FormInput
            label="Start Time"
            placeholder="HH:MM:SS"
            {...register('startTime')}
            error={errors.startTime?.message}
          />

          <FormInput
            label="Card Number"
            type="number"
            {...register('cardNumber')}
            error={errors.cardNumber?.message}
          />
        </form>
      </FormDialog>

      <ConfirmDialog
        title="Delete Runner"
        description={`Are you sure you want to delete the runner "${deletingRunner?.name}"? This action cannot be undone.`}
        open={isConfirmOpen}
        onOpenChange={setIsConfirmOpen}
        onConfirm={confirmDelete}
        onCancel={() => setIsConfirmOpen(false)}
        loading={deleteRunnerMutation.isLoading}
      />
    </div>
  );
}
