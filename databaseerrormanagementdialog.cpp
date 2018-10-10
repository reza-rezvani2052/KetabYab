#include "databaseerrormanagementdialog.h"
#include "ui_databaseerrormanagementdialog.h"

#include <QMessageBox>
#include <QFileDialog>

#include "dbconnection.h"

DatabaseErrorManagementDialog::DatabaseErrorManagementDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DatabaseErrorManagementDialog)
{
    ui->setupUi(this);
    m_isDatabasePathChanged = false;
}

DatabaseErrorManagementDialog::~DatabaseErrorManagementDialog()
{
    delete ui;
}

void DatabaseErrorManagementDialog::on_btnOk_clicked()
{
    if (m_isDatabasePathChanged) {
        saveDatabasePath();
        accept();
    } else {
        on_btnCancel_clicked();
    }
}

void DatabaseErrorManagementDialog::on_btnCancel_clicked()
{
    reject();
}

void DatabaseErrorManagementDialog::saveDatabasePath()
{
    QSettings settings;
    settings.beginGroup("Main");
    //...
    settings.setValue("DatabasePath", appInfo.databasePath);
    //...
    settings.endGroup();
}

void DatabaseErrorManagementDialog::on_btnCreateNewDb_clicked()
{
    if( createNewDatabase() )
        m_isDatabasePathChanged = true;
}

void DatabaseErrorManagementDialog::on_btnOpenExistsDb_clicked()
{    
    QString backupPath = QFileDialog::getOpenFileName(
                this, "انتخاب فایل پایگاه داده" ,
                qApp->applicationDirPath()  + QString("/db/"),
                "فایل پایگاه داده اسکیولایت (*.db);;هر فایلی (*.*)"
                );
    if( backupPath.isEmpty() ) // کاربر کنسل کرده است
        return ;
    //...

    if ( appInfo.databasePath.trimmed() == backupPath.trimmed())
    {
        QMessageBox::warning(this, "خطا",
                             "پایگاه داده‌ای که در برنامه اجرا شده است با نسخه انتخابی شما یکسان می‌باشد.");
        return ;
    }

    //...

    appInfo.databasePath = backupPath;
    m_isDatabasePathChanged = true;

    saveDatabasePath();
}
