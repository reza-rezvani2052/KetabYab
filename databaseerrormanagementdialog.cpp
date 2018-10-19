#include "databaseerrormanagementdialog.h"
#include "ui_databaseerrormanagementdialog.h"

#include <QMovie>
#include <QMessageBox>
#include <QFileDialog>

#include "utility.h"
#include "dbconnection.h"

DatabaseErrorManagementDialog::DatabaseErrorManagementDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DatabaseErrorManagementDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::SplashScreen);
    //...
    QMovie *movie = new QMovie(":/loading_restart.gif", QByteArray(), this);
    ui->lblLoading->setMovie(movie);
    movie->start();
    //...

}

DatabaseErrorManagementDialog::~DatabaseErrorManagementDialog()
{
    delete ui;
}

void DatabaseErrorManagementDialog::setLabelErrorDescriptionText(const QString &str)
{
    ui->lblErrDescription->setText(str);
}

void DatabaseErrorManagementDialog::on_btnCancel_clicked()
{
    reject();
}

void DatabaseErrorManagementDialog::on_btnOk_clicked()
{
    animatePageErrManagement();
}

void DatabaseErrorManagementDialog::on_btnClose_clicked()
{
    on_btnCancel_clicked();
}

void DatabaseErrorManagementDialog::on_btnCreateNewDb_clicked()
{
     if ( createNewDatabase() ) {

         // چون پایگاه داده تازه ساخته شده است حتما در اجرای بعدی برنامه صفحه
         // انتخاب رمز را باز کنم تا برای این پایگاه دادخ جدید رمز انتخاب شود
         Utility::setNumOfRunApp( /*1*/ );

         Utility::saveDatabasePath();
         //...
         animatePageLoading();
     }
}

void DatabaseErrorManagementDialog::on_btnOpenExistsDb_clicked()
{    
    QString backupPath = QFileDialog::getOpenFileName(
                this, "انتخاب فایل پایگاه داده" ,
                qApp->applicationDirPath()  + QString("/db/"),
                "فایل پایگاه داده (*.db)" );
    if( backupPath.isEmpty() )   // کاربر کنسل کرده است
        return ;

    //...
    appInfo.databasePath = backupPath;
    Utility::saveDatabasePath();
    //...
    animatePageLoading();
}

void DatabaseErrorManagementDialog::animatePageLoading()
{
    animateStackedWidgetPages(ui->stackedWidget , ui->pageLoading);
    QTimer::singleShot(3500, this, SLOT(accept()) );
}

void DatabaseErrorManagementDialog::animatePageErrManagement()
{
    animateStackedWidgetPages(ui->stackedWidget,ui->pageErrManagement);
}

void DatabaseErrorManagementDialog::animatePageErrDescription()
{
    animateStackedWidgetPages(ui->stackedWidget,ui->pageErrDescription);
}
void DatabaseErrorManagementDialog::animateStackedWidgetPages(QStackedWidget *stackedWidget,
                                                              QWidget *page)
{
    stackedWidget->setCurrentWidget(page);

    const QPoint posEnd = page->pos();
    QPoint posStart = posEnd;

    posStart.setX(-25);
    posStart.setY(0);

    QPropertyAnimation *animation = new QPropertyAnimation(page, "pos");

    animation->setDuration(1000);
    animation->setStartValue(posStart);
    animation->setEndValue(posEnd);

    animation->setEasingCurve(QEasingCurve::OutElastic);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}
