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
    QString defaultPath = qApp->applicationDirPath() + "/db/books.db";

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

    qApp->setOrganizationName("HosFa");
    qApp->setApplicationName("KetabYab");
    qApp->setOrganizationDomain("http://www.hosfa.ir");

    //NOTE: با توجه باینکه در دیالوگ ؛درباره برنامه؛ بررسی بروزرسانی انجام میشود
    // در صورت ارتقاء برنامه حتما این را افزایش دهم
    // و در وبسایت کتابخانه این بروزرسانی را اعمال کنم
    qApp->setApplicationVersion("1.0.0");

    //...

    //TODO: بعدا فونت های بیشتری به برنامه اضافه کنم
    // و در تنظیمات برنامه اجازه تغییر فونت را به کاربر بدهم
    int id = QFontDatabase::addApplicationFont(":/fonts/Yekan.ttf");
    QString family = QFontDatabase::applicationFontFamilies(id).at(0);
    QFont appFont(family);
    appFont.setPointSize( qApp->font().pointSize() + 1 );
    qApp->setFont(appFont);

    //...
    readSettings();
    //...

    QString strMsg = QString();
    DatabaseErrors errCode = createConnection(appInfo.db, appInfo.databasePath);
    switch (errCode)
    {
    case DatabaseErrors::FileNotFound:
        strMsg  = "برنامه موفق به یافتن مسیر فایل پایگاه داده نشده است:";
        strMsg += "\n\n" + appInfo.databasePath ;
        break;
    case DatabaseErrors::ConnectionError:
        strMsg  = "هنگام باز کردن پایگاه داده یک خطا به شرح زیر رخ داد" ;
        strMsg += "\n" + appInfo.db.lastError().text() ;
        break;
    case DatabaseErrors::FileIsCorrupted :
        strMsg  = "فرمت فایل پایگاه داده نادرست است." ;
        break;
    case DatabaseErrors::NoError :
        ; // OK!
        break;
    }

    //if (!strMsg.isEmpty()) { // اگر خطایی رخ داده است
    if (errCode != DatabaseErrors::NoError) {
        DatabaseErrorManagementDialog *errDialog = new DatabaseErrorManagementDialog(0);
        errDialog->setLabelErrorDescriptionText(strMsg);

        if ( errDialog->exec() == QDialog::Accepted ) {
            //delete errDialog;
            restartApp();
        } else {
            //delete errDialog;
            return -1;   // !یعنی کاربر برنامه را بسته و بیخیال شده
        }
    }

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
