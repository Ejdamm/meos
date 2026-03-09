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
import { Button } from '../components/ui/Button';
import { ControlSequenceBuilder } from '../components/ControlSequenceBuilder';
import { useCourses, useCreateCourse, useUpdateCourse, useDeleteCourse } from '../hooks/useCourses';
import { useControls } from '../hooks/useControls';
import type { Course } from '../api/types';

const courseSchema = z.object({
  name: z.string().min(1, 'Name is required'),
  length: z.number().optional(),
  climb: z.number().optional(),
  controls: z.array(z.number()),
});

type CourseFormValues = z.infer<typeof courseSchema>;

export default function CoursesPage() {
  const { data: coursesData, isLoading: isLoadingCourses, refetch: refetchCourses } = useCourses();
  const { data: controlsData, isLoading: isLoadingControls } = useControls();
  
  const courses = coursesData || [];
  const controls = controlsData || [];

  const [isFormOpen, setIsFormOpen] = React.useState(false);
  const [isConfirmOpen, setIsConfirmOpen] = React.useState(false);
  const [editingCourse, setEditingCourse] = React.useState<Course | null>(null);
  const [deletingCourse, setDeletingCourse] = React.useState<Course | null>(null);

  const {
    register,
    handleSubmit,
    reset,
    control,
    formState: { errors },
  } = useForm<CourseFormValues>({
    resolver: zodResolver(courseSchema),
    defaultValues: {
      name: '',
      length: 0,
      climb: 0,
      controls: [],
    },
  });

  const createCourseMutation = useCreateCourse();
  const updateCourseMutation = useUpdateCourse();
  const deleteCourseMutation = useDeleteCourse();

  const onSubmit = async (values: CourseFormValues) => {
    try {
      if (editingCourse) {
        await updateCourseMutation.mutate({ id: editingCourse.id, data: values });
      } else {
        await createCourseMutation.mutate(values);
      }
      setIsFormOpen(false);
      refetchCourses();
    } catch (err: any) {
      alert(`Error: ${err.message}`);
    }
  };

  const handleCreate = () => {
    setEditingCourse(null);
    reset({
      name: '',
      length: 0,
      climb: 0,
      controls: [],
    });
    setIsFormOpen(true);
  };

  const handleEdit = (course: Course) => {
    setEditingCourse(course);
    reset({
      name: course.name,
      length: course.length || 0,
      climb: course.climb || 0,
      controls: course.controls || [],
    });
    setIsFormOpen(true);
  };

  const handleDelete = (course: Course) => {
    setDeletingCourse(course);
    setIsConfirmOpen(true);
  };

  const confirmDelete = async () => {
    if (deletingCourse) {
      try {
        await deleteCourseMutation.mutate(deletingCourse.id);
        setIsConfirmOpen(false);
        refetchCourses();
      } catch (err: any) {
        alert(`Error: ${err.message}`);
      }
    }
  };

  const columns: Column<Course>[] = [
    { header: 'Name', accessorKey: 'name', sortable: true },
    { 
      header: 'Length (m)', 
      accessorKey: 'length', 
      cell: (course) => course.length ? `${course.length}m` : '-',
      sortable: true 
    },
    { 
      header: 'Climb (m)', 
      accessorKey: 'climb', 
      cell: (course) => course.climb ? `${course.climb}m` : '-',
      sortable: true 
    },
    { 
      header: 'Controls', 
      accessorKey: 'controls', 
      cell: (course) => (course.controls?.length || 0),
      sortable: true 
    },
    {
      header: 'Actions',
      accessorKey: 'id',
      cell: (course) => (
        <div className="flex space-x-2" onClick={(e) => e.stopPropagation()}>
          <Button variant="ghost" size="icon" onClick={() => handleEdit(course)}>
            <Edit className="w-4 h-4" />
          </Button>
          <Button variant="ghost" size="icon" onClick={() => handleDelete(course)}>
            <Trash2 className="w-4 h-4 text-destructive" />
          </Button>
        </div>
      )
    }
  ];

  return (
    <div className="container mx-auto py-8">
      <div className="flex justify-between items-center mb-6">
        <h1 className="text-3xl font-bold">Courses</h1>
        <Button onClick={handleCreate}>
          <Plus className="w-4 h-4 mr-2" />
          Add Course
        </Button>
      </div>

      <DataTable
        columns={columns}
        data={courses}
        onRowClick={handleEdit}
        isLoading={isLoadingCourses || isLoadingControls}
      />

      <FormDialog
        title={editingCourse ? 'Edit Course' : 'Add Course'}
        open={isFormOpen}
        onOpenChange={setIsFormOpen}
        onSave={() => handleSubmit(onSubmit)()}
        onCancel={() => setIsFormOpen(false)}
        loading={createCourseMutation.isLoading || updateCourseMutation.isLoading}
        size="lg"
      >
        <form className="space-y-4" onSubmit={handleSubmit(onSubmit)}>
          <FormInput
            label="Course Name"
            {...register('name')}
            error={errors.name?.message}
            required
          />

          <div className="grid grid-cols-2 gap-4">
            <FormInput
              label="Length (m)"
              type="number"
              {...register('length', { valueAsNumber: true })}
              error={errors.length?.message}
            />
            <FormInput
              label="Climb (m)"
              type="number"
              {...register('climb', { valueAsNumber: true })}
              error={errors.climb?.message}
            />
          </div>

          <Controller
            name="controls"
            control={control}
            render={({ field }) => (
              <ControlSequenceBuilder
                value={field.value}
                onChange={field.onChange}
                availableControls={controls}
                error={errors.controls?.message}
              />
            )}
          />
        </form>
      </FormDialog>

      <ConfirmDialog
        title="Delete Course"
        description={`Are you sure you want to delete the course "${deletingCourse?.name}"? This action cannot be undone.`}
        open={isConfirmOpen}
        onOpenChange={setIsConfirmOpen}
        onConfirm={confirmDelete}
        onCancel={() => setIsConfirmOpen(false)}
        loading={deleteCourseMutation.isLoading}
      />
    </div>
  );
}
