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
import { Button } from '../components/ui/Button';
import { useClasses, useCreateClass, useUpdateClass, useDeleteClass } from '../hooks/useClasses';
import { useCourses } from '../hooks/useCourses';
import { StartMethod } from '../api/types';
import type { Class } from '../api/types';

const classSchema = z.object({
  name: z.string().min(1, 'Name is required'),
  courseId: z.string().optional(),
  startMethod: z.string().optional(),
  sortOrder: z.number().optional(),
});

type ClassFormValues = z.infer<typeof classSchema>;

export default function ClassesPage() {
  const { data: classesData, isLoading: isLoadingClasses, refetch: refetchClasses } = useClasses();
  const { data: coursesData, isLoading: isLoadingCourses } = useCourses();
  
  const classes = classesData || [];
  const courses = coursesData || [];

  const [isFormOpen, setIsFormOpen] = React.useState(false);
  const [isConfirmOpen, setIsConfirmOpen] = React.useState(false);
  const [editingClass, setEditingClass] = React.useState<Class | null>(null);
  const [deletingClass, setDeletingClass] = React.useState<Class | null>(null);

  const {
    register,
    handleSubmit,
    reset,
    control,
    formState: { errors },
  } = useForm<ClassFormValues>({
    resolver: zodResolver(classSchema),
    defaultValues: {
      name: '',
      courseId: '',
      startMethod: String(StartMethod.Time),
      sortOrder: 0,
    },
  });

  const createClassMutation = useCreateClass();
  const updateClassMutation = useUpdateClass();
  const deleteClassMutation = useDeleteClass();

  const onSubmit = async (values: ClassFormValues) => {
    const data: Partial<Class> = {
      name: values.name,
      courseId: values.courseId ? Number(values.courseId) : undefined,
      startMethod: values.startMethod ? (Number(values.startMethod) as StartMethod) : undefined,
      sortOrder: values.sortOrder,
    };

    try {
      if (editingClass) {
        await updateClassMutation.mutate({ id: editingClass.id, data });
      } else {
        await createClassMutation.mutate(data);
      }
      setIsFormOpen(false);
      refetchClasses();
    } catch (err: any) {
      alert(`Error: ${err.message}`);
    }
  };

  const handleCreate = () => {
    setEditingClass(null);
    reset({
      name: '',
      courseId: '',
      startMethod: String(StartMethod.Time),
      sortOrder: classes.length + 1,
    });
    setIsFormOpen(true);
  };

  const handleEdit = (cls: Class) => {
    setEditingClass(cls);
    reset({
      name: cls.name,
      courseId: cls.courseId ? String(cls.courseId) : '',
      startMethod: cls.startMethod !== undefined ? String(cls.startMethod) : '',
      sortOrder: cls.sortOrder || 0,
    });
    setIsFormOpen(true);
  };

  const handleDelete = (cls: Class) => {
    setDeletingClass(cls);
    setIsConfirmOpen(true);
  };

  const confirmDelete = async () => {
    if (deletingClass) {
      try {
        await deleteClassMutation.mutate(deletingClass.id);
        setIsConfirmOpen(false);
        refetchClasses();
      } catch (err: any) {
        alert(`Error: ${err.message}`);
      }
    }
  };

  const startMethodOptions = [
    { label: 'Timed', value: String(StartMethod.Time) },
    { label: 'Change', value: String(StartMethod.Change) },
    { label: 'Drawn', value: String(StartMethod.Drawn) },
    { label: 'Pursuit', value: String(StartMethod.Pursuit) },
  ];

  const courseOptions = [
    { label: 'No Course', value: '' },
    ...courses.map(c => ({ label: c.name, value: String(c.id) }))
  ];

  const columns: Column<Class>[] = [
    { header: 'Name', accessorKey: 'name', sortable: true },
    { 
      header: 'Course', 
      accessorKey: 'courseId', 
      cell: (cls) => courses.find(c => c.id === cls.courseId)?.name || 'None',
      sortable: true 
    },
    { 
      header: 'Start Method', 
      accessorKey: 'startMethod', 
      cell: (cls) => startMethodOptions.find(o => o.value === String(cls.startMethod))?.label || 'Unknown',
      sortable: true 
    },
    {
      header: 'Actions',
      accessorKey: 'id',
      cell: (cls) => (
        <div className="flex space-x-2" onClick={(e) => e.stopPropagation()}>
          <Button variant="ghost" size="icon" onClick={() => handleEdit(cls)}>
            <Edit className="w-4 h-4" />
          </Button>
          <Button variant="ghost" size="icon" onClick={() => handleDelete(cls)}>
            <Trash2 className="w-4 h-4 text-destructive" />
          </Button>
        </div>
      )
    }
  ];

  return (
    <div className="container mx-auto py-8">
      <div className="flex justify-between items-center mb-6">
        <h1 className="text-3xl font-bold">Classes</h1>
        <Button onClick={handleCreate}>
          <Plus className="w-4 h-4 mr-2" />
          Add Class
        </Button>
      </div>

      <DataTable
        columns={columns}
        data={classes}
        onRowClick={handleEdit}
        isLoading={isLoadingClasses || isLoadingCourses}
      />

      <FormDialog
        title={editingClass ? 'Edit Class' : 'Add Class'}
        open={isFormOpen}
        onOpenChange={setIsFormOpen}
        onSave={() => handleSubmit(onSubmit)()}
        onCancel={() => setIsFormOpen(false)}
        loading={createClassMutation.isLoading || updateClassMutation.isLoading}
      >
        <form className="space-y-4" onSubmit={handleSubmit(onSubmit)}>
          <FormInput
            label="Class Name"
            {...register('name')}
            error={errors.name?.message}
            required
          />

          <Controller
            name="courseId"
            control={control}
            render={({ field }) => (
              <FormSelect
                label="Course"
                options={courseOptions}
                value={field.value}
                onValueChange={field.onChange}
                error={errors.courseId?.message}
              />
            )}
          />

          <Controller
            name="startMethod"
            control={control}
            render={({ field }) => (
              <FormSelect
                label="Start Method"
                options={startMethodOptions}
                value={field.value}
                onValueChange={field.onChange}
                error={errors.startMethod?.message}
              />
            )}
          />

          <FormInput
            label="Sort Order"
            type="number"
            {...register('sortOrder', { valueAsNumber: true })}
            error={errors.sortOrder?.message}
          />
        </form>
      </FormDialog>

      <ConfirmDialog
        title="Delete Class"
        description={`Are you sure you want to delete the class "${deletingClass?.name}"? This action cannot be undone.`}
        open={isConfirmOpen}
        onOpenChange={setIsConfirmOpen}
        onConfirm={confirmDelete}
        onCancel={() => setIsConfirmOpen(false)}
        loading={deleteClassMutation.isLoading}
      />
    </div>
  );
}
