#include "changepassworddialog.h"
#include "ui_changepassworddialog.h"

#include "utility.h"
#include "dbconnection.h"

ChangePasswordDialog::ChangePasswordDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChangePasswordDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::SplashScreen);
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
    QString strLedCurrentPass = ui->ledCurrentPass->text();
    if (strLedCurrentPass.trimmed().isEmpty()) {
        qApp->beep();
        Utility::createPopupDialog( QString(" کلمه عبور جاری نمی‌تواند خالی باشد. ") ,
                                    QString() ,QPoint(), true, 2000, this )->show();
        ui->ledCurrentPass->setFocus();
        return;
    }

    int ret = strLedCurrentPass.compare(
                getUserPassword(userInfo.userName) /* userInfo.password */ );
    if (ret) {
        qApp->beep();
        Utility::createPopupDialog( QString(" کلمه عبور فعلی نادرست است. ") ,
                                    QString() ,QPoint(), true, 2000, this )->show();
        ui->ledCurrentPass->setFocus();
        return;
    }

    //...

    QString newPassword = ui->ledNewPassword->text();
    QString newPasswordAgain = ui->ledNewPasswordAgain->text();

    if (newPassword.isEmpty() || newPasswordAgain.isEmpty()) {
        qApp->beep();
        Utility::createPopupDialog( QString(" کلمه عبور جدید یا تکرار آن نمی‌تواند خالی باشد. ") ,
                                    QString() ,QPoint(), true, 2000, this )->show();
        if (newPasswordAgain.trimmed().isEmpty())
            ui->ledNewPasswordAgain->setFocus();
        if (newPassword.trimmed().isEmpty())
            ui->ledNewPassword->setFocus();
        return;
    }

    if ( newPassword.compare(newPasswordAgain) != 0 ) {
        qApp->beep();
        Utility::createPopupDialog( QString(" کلمه عبور و تکرا آن متفاوت است. ") ,
                                    QString() ,QPoint(), true, 2000, this )->show();
        ui->ledNewPassword->setFocus();
        return;
    }

    //...

    // اگر رمز جدید با رمز قبلی یکی بود
    if (strLedCurrentPass == newPassword) {
        qApp->beep();
        Utility::createPopupDialog( QString(" رمز عبور جدید با قبلی یکسان است! ") ,
                                    QString() ,QPoint(), true, 2000, this )->show();
        return;
    }

    //...
    QString passHint = ui->ledPassHint->text().trimmed();

    if ( !setUsersPass(newPassword, passHint) ) {
        qApp->beep();
        Utility::createPopupDialog( QString(" خطایی رخ داده است! ") ,
                                    QString() ,QPoint(), true, 2000, this )->show();
    }

    accept();
}
