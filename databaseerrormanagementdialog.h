#ifndef DATABASEERRORMANAGEMENTDIALOG_H
#define DATABASEERRORMANAGEMENTDIALOG_H

#include <QDialog>
#include <QStackedWidget>

#include "draggabledialog.h"

namespace Ui {
class DatabaseErrorManagementDialog;
}

class DatabaseErrorManagementDialog : public DraggableDialog
{
    Q_OBJECT

public:
    explicit DatabaseErrorManagementDialog(QWidget *parent = 0);
    ~DatabaseErrorManagementDialog();

    void setLabelErrorDescriptionText(const QString &str);

private slots:
    void on_btnOk_clicked();
    void on_btnClose_clicked();
    void on_btnCancel_clicked();

    void on_btnCreateNewDb_clicked();
    void on_btnOpenExistsDb_clicked();        

    void animatePageLoading();
    void animatePageErrManagement();
    void animatePageErrDescription();
    void animateStackedWidgetPages(QStackedWidget *stackedWidget, QWidget *page);

private:
    Ui::DatabaseErrorManagementDialog *ui;

};

#endif // DATABASEERRORMANAGEMENTDIALOG_H
