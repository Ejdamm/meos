import { useState, useMemo } from 'react';
import { Plus, Edit, Trash2 } from 'lucide-react';
import { useForm, type SubmitHandler } from 'react-hook-form';
import { zodResolver } from '@hookform/resolvers/zod';
import * as z from 'zod';

import { Button } from '../components/ui/Button';
import { DataTable, type Column } from '../components/DataTable';
import { FormDialog } from '../components/FormDialog';
import { ConfirmDialog } from '../components/ConfirmDialog';
import { FormInput } from '../components/FormInput';
import { SearchableSelect } from '../components/SearchableSelect';
import { TeamMemberManager } from '../components/TeamMemberManager';

import { useTeams, useCreateTeam, useUpdateTeam, useDeleteTeam } from '../hooks/useTeams';
import { useClubs } from '../hooks/useClubs';
import { useClasses } from '../hooks/useClasses';
import { useRunners } from '../hooks/useRunners';
import type { Team } from '../api/types';
import { RunnerStatus } from '../api/types';

const teamSchema = z.object({
  name: z.string().min(1, 'Name is required'),
  clubId: z.number().optional(),
  classId: z.number().optional(),
  members: z.array(z.number()).default([]),
  status: z.number().default(RunnerStatus.OK),
});

type TeamFormValues = z.infer<typeof teamSchema>;

export default function TeamsPage() {
  const { data: teams, isLoading: isLoadingTeams, refetch: refetchTeams } = useTeams();
  const { data: clubs } = useClubs();
  const { data: classes } = useClasses();
  const { data: runners } = useRunners();

  const createTeam = useCreateTeam();
  const updateTeam = useUpdateTeam();
  const deleteTeam = useDeleteTeam();

  const [isCreateOpen, setIsCreateOpen] = useState(false);
  const [editingTeam, setEditingTeam] = useState<Team | null>(null);
  const [deletingTeam, setDeletingTeam] = useState<Team | null>(null);

  const {
    register,
    handleSubmit,
    reset,
    setValue,
    watch,
    formState: { errors },
  } = useForm<TeamFormValues>({
    resolver: zodResolver(teamSchema) as any, // Use any to bypass strict resolver mismatch for now
    defaultValues: {
      name: '',
      members: [],
      status: RunnerStatus.OK,
    },
  });

  const membersValue = watch('members');

  const handleCreateOpen = () => {
    reset({
      name: '',
      members: [],
      status: RunnerStatus.OK,
    });
    setIsCreateOpen(true);
  };

  const handleEditOpen = (team: Team) => {
    reset({
      name: team.name,
      clubId: team.clubId,
      classId: team.classId,
      members: team.members || [],
      status: team.status,
    });
    setEditingTeam(team);
  };

  const onCreateSubmit: SubmitHandler<TeamFormValues> = async (data) => {
    try {
      await createTeam.mutate(data as Partial<Team>);
      setIsCreateOpen(false);
      refetchTeams();
    } catch (error) {
      console.error('Failed to create team:', error);
    }
  };

  const onEditSubmit: SubmitHandler<TeamFormValues> = async (data) => {
    if (!editingTeam) return;
    try {
      await updateTeam.mutate({ id: editingTeam.id, data: data as Partial<Team> });
      setEditingTeam(null);
      refetchTeams();
    } catch (error) {
      console.error('Failed to update team:', error);
    }
  };

  const onDeleteConfirm = async () => {
    if (!deletingTeam) return;
    try {
      await deleteTeam.mutate(deletingTeam.id);
      setDeletingTeam(null);
      refetchTeams();
    } catch (error) {
      console.error('Failed to delete team:', error);
    }
  };

  const clubOptions = useMemo(() => 
    clubs?.map(c => ({ label: c.name, value: String(c.id) })) || [], 
    [clubs]
  );

  const classOptions = useMemo(() => 
    classes?.map(c => ({ label: c.name, value: String(c.id) })) || [], 
    [classes]
  );

  const columns: Column<Team>[] = [
    {
      header: 'Name',
      accessorKey: 'name',
    },
    {
      header: 'Club',
      accessorKey: 'clubId',
      cell: (team: Team) => clubs?.find(c => c.id === team.clubId)?.name || '-',
    },
    {
      header: 'Class',
      accessorKey: 'classId',
      cell: (team: Team) => classes?.find(c => c.id === team.classId)?.name || '-',
    },
    {
      header: 'Members',
      accessorKey: 'members',
      cell: (team: Team) => team.members?.length || 0,
    },
    {
      header: 'Actions',
      accessorKey: 'id',
      cell: (team: Team) => (
        <div className="flex items-center gap-2" onClick={(e) => e.stopPropagation()}>
          <Button
            variant="ghost"
            size="icon"
            onClick={() => handleEditOpen(team)}
          >
            <Edit className="h-4 w-4" />
          </Button>
          <Button
            variant="ghost"
            size="icon"
            onClick={() => setDeletingTeam(team)}
          >
            <Trash2 className="h-4 w-4 text-destructive" />
          </Button>
        </div>
      ),
    },
  ];

  return (
    <div className="space-y-6">
      <div className="flex items-center justify-between">
        <div>
          <h1 className="text-3xl font-bold tracking-tight">Teams</h1>
          <p className="text-muted-foreground">
            Manage relay and team entries.
          </p>
        </div>
        <Button onClick={handleCreateOpen}>
          <Plus className="mr-2 h-4 w-4" /> Add Team
        </Button>
      </div>

      <DataTable
        columns={columns}
        data={teams || []}
        isLoading={isLoadingTeams}
        onRowClick={handleEditOpen}
      />

      {/* Create Dialog */}
      <FormDialog
        title="Add New Team"
        description="Enter the team details and add members."
        open={isCreateOpen}
        onOpenChange={setIsCreateOpen}
        onSave={() => handleSubmit(onCreateSubmit)()}
        onCancel={() => setIsCreateOpen(false)}
        loading={createTeam.isLoading}
        size="lg"
      >
        <div className="grid grid-cols-2 gap-4">
          <div className="space-y-4">
            <FormInput
              label="Team Name"
              {...register('name')}
              error={errors.name?.message}
              required
            />
            <SearchableSelect
              label="Club"
              options={clubOptions}
              value={watch('clubId')?.toString()}
              onValueChange={(val) => setValue('clubId', Number(val))}
              placeholder="Select club..."
              error={errors.clubId?.message}
            />
            <SearchableSelect
              label="Class"
              options={classOptions}
              value={watch('classId')?.toString()}
              onValueChange={(val) => setValue('classId', Number(val))}
              placeholder="Select class..."
              error={errors.classId?.message}
            />
          </div>
          <div>
            <TeamMemberManager
              value={membersValue}
              onChange={(val) => setValue('members', val)}
              availableRunners={runners || []}
              error={errors.members?.message}
            />
          </div>
        </div>
      </FormDialog>

      {/* Edit Dialog */}
      <FormDialog
        title="Edit Team"
        description="Update the team details and member list."
        open={!!editingTeam}
        onOpenChange={(open) => !open && setEditingTeam(null)}
        onSave={() => handleSubmit(onEditSubmit)()}
        onCancel={() => setEditingTeam(null)}
        loading={updateTeam.isLoading}
        size="lg"
      >
        <div className="grid grid-cols-2 gap-4">
          <div className="space-y-4">
            <FormInput
              label="Team Name"
              {...register('name')}
              error={errors.name?.message}
              required
            />
            <SearchableSelect
              label="Club"
              options={clubOptions}
              value={watch('clubId')?.toString()}
              onValueChange={(val) => setValue('clubId', Number(val))}
              placeholder="Select club..."
              error={errors.clubId?.message}
            />
            <SearchableSelect
              label="Class"
              options={classOptions}
              value={watch('classId')?.toString()}
              onValueChange={(val) => setValue('classId', Number(val))}
              placeholder="Select class..."
              error={errors.classId?.message}
            />
          </div>
          <div>
            <TeamMemberManager
              value={membersValue}
              onChange={(val) => setValue('members', val)}
              availableRunners={runners || []}
              error={errors.members?.message}
            />
          </div>
        </div>
      </FormDialog>

      {/* Delete Confirmation */}
      <ConfirmDialog
        open={!!deletingTeam}
        onOpenChange={(open) => !open && setDeletingTeam(null)}
        title="Delete Team"
        description={`Are you sure you want to delete "${deletingTeam?.name}"? This action cannot be undone.`}
        onConfirm={onDeleteConfirm}
        onCancel={() => setDeletingTeam(null)}
        loading={deleteTeam.isLoading}
        variant="destructive"
      />
    </div>
  );
}
