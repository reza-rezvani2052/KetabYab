#include "changepassworddialog.h"
#include "ui_changepassworddialog.h"

ChangePasswordDialog::ChangePasswordDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChangePasswordDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::SplashScreen);
    //...
}

ChangePasswordDialog::~ChangePasswordDialog()
{
    delete ui;
}

void ChangePasswordDialog::on_btnCancel_clicked()
{
    reject();
}

void ChangePasswordDialog::on_btnChangePassword_clicked()
{
    //TODO: ***********************

}
