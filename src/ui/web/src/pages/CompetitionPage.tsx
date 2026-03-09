import * as React from 'react';
import { useForm } from 'react-hook-form';
import { zodResolver } from '@hookform/resolvers/zod';
import { z } from 'zod';
import { api } from '../api/client';
import { useQuery, useMutation } from '../hooks/useApi';
import { FormInput } from '../components/FormInput';
import { Button } from '../components/ui/Button';

const competitionSchema = z.object({
  name: z.string().min(1, 'Name is required'),
  date: z.string().min(1, 'Date is required'),
  organizer: z.string().min(1, 'Organizer is required'),
  homepage: z.string().url().optional().or(z.literal('')),
  zeroTime: z.string().regex(/^\d{2}:\d{2}:\d{2}$/, 'Invalid time format (HH:MM:SS)').optional().or(z.literal('')),
});

type CompetitionFormValues = z.infer<typeof competitionSchema>;

export default function CompetitionPage() {
  const { data: competitions, isLoading, error, refetch } = useQuery(() => api.getCompetitions());
  
  const competition = competitions?.[0];

  const {
    register,
    handleSubmit,
    reset,
    formState: { errors },
  } = useForm<CompetitionFormValues>({
    resolver: zodResolver(competitionSchema),
    defaultValues: {
      name: '',
      date: '',
      organizer: '',
      homepage: '',
      zeroTime: '',
    },
  });

  React.useEffect(() => {
    if (competition) {
      reset({
        name: competition.name,
        date: competition.date,
        organizer: competition.organizer,
        homepage: competition.homepage || '',
        zeroTime: competition.zeroTime || '',
      });
    }
  }, [competition, reset]);

  const updateMutation = useMutation((values: CompetitionFormValues) => {
    if (!competition) throw new Error('No competition to update');
    return api.updateCompetition(competition.id, values);
  }, {
    onSuccess: () => {
      alert('Competition settings updated successfully!');
      refetch();
    },
    onError: (err) => {
      alert(`Error updating competition: ${err.message}`);
    },
  });

  const onSubmit = (values: CompetitionFormValues) => {
    updateMutation.mutate(values);
  };

  if (isLoading) return <div className="p-4">Loading competition...</div>;
  if (error) return <div className="p-4 text-destructive">Error: {error.message}</div>;
  if (!competition) return <div className="p-4">No competition found.</div>;

  return (
    <div className="container max-w-2xl mx-auto py-8">
      <div className="flex justify-between items-center mb-6">
        <h1 className="text-3xl font-bold">Competition Settings</h1>
      </div>

      <div className="bg-card text-card-foreground rounded-lg border shadow-sm p-6">
        <form onSubmit={handleSubmit(onSubmit)} className="space-y-6">
          <FormInput
            label="Competition Name"
            {...register('name')}
            error={errors.name?.message}
            required
          />

          <FormInput
            label="Date"
            type="date"
            {...register('date')}
            error={errors.date?.message}
            required
          />

          <FormInput
            label="Organizer"
            {...register('organizer')}
            error={errors.organizer?.message}
            required
          />

          <FormInput
            label="Homepage (URL)"
            {...register('homepage')}
            error={errors.homepage?.message}
            placeholder="https://example.com"
          />

          <FormInput
            label="Zero Time (HH:MM:SS)"
            {...register('zeroTime')}
            error={errors.zeroTime?.message}
            placeholder="10:00:00"
          />

          <div className="flex justify-end pt-4">
            <Button type="submit" disabled={updateMutation.isLoading}>
              {updateMutation.isLoading ? 'Saving...' : 'Save Settings'}
            </Button>
          </div>
        </form>
      </div>
    </div>
  );
}
