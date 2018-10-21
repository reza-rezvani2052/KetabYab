#ifndef CHANGEPASSWORDDIALOG_H
#define CHANGEPASSWORDDIALOG_H

#include <QDialog>

#include "draggabledialog.h"

namespace Ui {
class ChangePasswordDialog;
}

class ChangePasswordDialog : public DraggableDialog
{
    Q_OBJECT

public:
    explicit ChangePasswordDialog(QWidget *parent = 0);
    ~ChangePasswordDialog();

private slots:
    void on_btnCancel_clicked();
    void on_btnChangePassword_clicked();

private:
    Ui::ChangePasswordDialog *ui;

};

#endif // CHANGEPASSWORDDIALOG_H
