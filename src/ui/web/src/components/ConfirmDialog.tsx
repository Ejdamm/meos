import * as React from "react"
import {
  Dialog,
  DialogContent,
  DialogDescription,
  DialogFooter,
  DialogHeader,
  DialogTitle,
} from "./ui/Dialog"
import { Button } from "./ui/Button"

interface ConfirmDialogProps {
  title: string
  description: string
  open: boolean
  onOpenChange: (open: boolean) => void
  onConfirm: () => void
  onCancel: () => void
  loading?: boolean
  confirmText?: string
  cancelText?: string
  variant?: "default" | "destructive"
}

const ConfirmDialog: React.FC<ConfirmDialogProps> = ({
  title,
  description,
  open,
  onOpenChange,
  onConfirm,
  onCancel,
  loading = false,
  confirmText = "Confirm",
  cancelText = "Cancel",
  variant = "destructive",
}) => {
  return (
    <Dialog open={open} onOpenChange={onOpenChange}>
      <DialogContent className="sm:max-w-[425px]">
        <DialogHeader>
          <DialogTitle>{title}</DialogTitle>
          <DialogDescription>{description}</DialogDescription>
        </DialogHeader>
        <DialogFooter>
          <Button variant="outline" onClick={onCancel} disabled={loading}>
            {cancelText}
          </Button>
          <Button variant={variant} onClick={onConfirm} disabled={loading}>
            {loading ? "Confirming..." : confirmText}
          </Button>
        </DialogFooter>
      </DialogContent>
    </Dialog>
  )
}

export { ConfirmDialog }
