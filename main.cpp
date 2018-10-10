#include <QtSql>
#include <QProcess>
#include <QMessageBox>
#include <QApplication>
#include <QFontDatabase>

#include "mainwindow.h"
#include "logindialog.h"
#include "dbconnection.h"
#include "databaseerrormanagementdialog.h"

//-----------------------------------------------------------------

AppInfo  appInfo;
UserInfo userInfo;

//-----------------------------------------------------------------

void readSettings()
{
    QSettings settings;;
    settings.beginGroup("Main");
    //...
    QString defaultPath = qApp->applicationDirPath() + "/db/thesis.db";

    appInfo.databasePath = settings.value("DatabasePath", defaultPath).toString();
    //...
    settings.endGroup();
}

void restartApp()
{
    qApp->quit();
    QProcess::startDetached( qApp->arguments()[0], qApp->arguments() );
}

//-----------------------------------------------------------------

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE( rc );

    //...

    QApplication a(argc, argv);
    qApp->setLayoutDirection(Qt::RightToLeft);

    qApp->setApplicationName("PaYab");
    qApp->setOrganizationName("ChalusplSoft");
    qApp->setOrganizationDomain("http://www.chaluspl.com");

    //...

    int id = QFontDatabase::addApplicationFont(":/fonts/BYekan.ttf");
    QString family = QFontDatabase::applicationFontFamilies(id).at(0);
    QFont BYekan(family);
    BYekan.setPointSize( qApp->font().pointSize()+1 );
    qApp->setFont(BYekan);

    //...
    readSettings();
    //...

    QString strMsg;

    DatabaseErrorManagementDialog *errDialog = new DatabaseErrorManagementDialog(0);

    QMessageBox *msgBox = new QMessageBox();
    msgBox->setButtonText(QMessageBox::Ok, "تایید");
    msgBox->setIcon(QMessageBox::Critical);


    switch (createConnection(appInfo.db, appInfo.databasePath)) {
    case DatabaseErrors::FileNotFound:
        strMsg  = "برنامه موفق به یافتن پایگاه داده نشده است.";
        strMsg += "\n\n" + appInfo.databasePath + "\n";
        msgBox->setText(strMsg);
        msgBox->setWindowTitle("عدم دسترسی به پایگاه داده");
        msgBox->setInformativeText(" "); // این باعث میشود که متن پیام اگر طولانی بود نشکند
        msgBox->exec();

        if ( errDialog->exec() == QDialog::Accepted ) {
            restartApp();
        } else {
            return -1;
        }

        break;
    case DatabaseErrors::ConnectionError:
        strMsg  = "هنگام باز کردن پایگاه داده یک خطا به شرح زیر رخ داد" ;
        strMsg += "\n" + appInfo.db.lastError().text() ;
        msgBox->setText(strMsg);
        msgBox->setWindowTitle("خطا در باز کردن پایگاه داده");
        msgBox->setInformativeText(" ");
        msgBox->exec();

        if ( errDialog->exec() == QDialog::Accepted ) {
            restartApp();
        } else {
            return -1;
        }
        break;
    case DatabaseErrors::FileIsCorrupted :
        msgBox->setText("فرمت فایل پایگاه داده نادرست است.");
        msgBox->setWindowTitle("خطا");
        msgBox->exec();

        if ( errDialog->exec() == QDialog::Accepted ) {
            restartApp();
        } else {
            return -1;
        }
        break;
    }

    delete errDialog;
    delete msgBox;

    //...

    LoginDialog *loginDialog = new LoginDialog();
    if ( loginDialog->exec() == QDialog::Accepted )
    {
        MainWindow *w = new MainWindow();
        w->show();
    } else {
        appInfo.db.close();
        qApp->quit();
        exit(0);
    }

    delete loginDialog;
    //...

    return a.exec();
}
