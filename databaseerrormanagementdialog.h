#ifndef DATABASEERRORMANAGEMENTDIALOG_H
#define DATABASEERRORMANAGEMENTDIALOG_H

#include <QDialog>

namespace Ui {
class DatabaseErrorManagementDialog;
}

class DatabaseErrorManagementDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DatabaseErrorManagementDialog(QWidget *parent = 0);
    ~DatabaseErrorManagementDialog();

private slots:
    void on_btnOk_clicked();
    void on_btnCancel_clicked();

    void on_btnCreateNewDb_clicked();
    void on_btnOpenExistsDb_clicked();        

private:
    Ui::DatabaseErrorManagementDialog *ui;

    bool m_isDatabasePathChanged;

    void saveDatabasePath();
};

#endif // DATABASEERRORMANAGEMENTDIALOG_H
