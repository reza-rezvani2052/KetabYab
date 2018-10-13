#include "logindialog.h"
#include "ui_logindialog.h"

#include <QDebug>
#include <QSysInfo>
#include <QKeyEvent>
#include <QMessageBox>
#include <QPropertyAnimation>

#include "popupdialog.h"
#include "dbconnection.h"


extern AppInfo appInfo;
extern UserInfo userInfo;


LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    //...
    m_startDraging = false;
    setWindowFlags(Qt::FramelessWindowHint);
    //...
    ui->cmbUsername->addItems( allUserNames() );
    on_cmbUsername_currentIndexChanged( ui->cmbUsername->currentText() );

    QLineEdit *led = new QLineEdit(ui->cmbUsername) ;
    led->setReadOnly(true);
    ui->cmbUsername->setLineEdit(led);
    led->setAlignment(Qt::AlignLeft);
    //...
    readSettings();

    if (numOfRunApp > 1)
        ui->stackedWidgetMain->setCurrentWidget(ui->pageLogin);
    //...
}

LoginDialog::~LoginDialog()
{    
    delete ui;
}

void LoginDialog::readSettings()
{
    QSettings settings;
    settings.beginGroup("LoginDialog");
    //...
    numOfRunApp = settings.value("NumOfRunApp", 1).toInt();
    //...
    settings.endGroup();
}

void LoginDialog::writeSettings()
{
    QSettings settings;
    settings.beginGroup("LoginDialog");

    // تعداد اجرای برنامه
    settings.setValue("NumOfRunApp", numOfRunApp); //  numOfRunApp++

    settings.endGroup();
}

void LoginDialog::closeEvent(QCloseEvent *)
{
    writeSettings();
}

void LoginDialog::animateLoginLogo()
{
    animateStackedWidgetPages(ui->stackedWidgetLogo , ui->pageUserLogo);
}

void LoginDialog::animatePageLogin()
{
    animateStackedWidgetPages(ui->stackedWidgetMain ,ui->pageLogin);
}

void LoginDialog::animateStackedWidgetPages(QStackedWidget *stackedWidget, QWidget *page)
{
    stackedWidget->setCurrentWidget(page);

    const QPoint posEnd = page->pos();
    QPoint posStart = posEnd;

    posStart.setX(-100);
    posStart.setY(0);

    QPropertyAnimation *animation = new QPropertyAnimation(page, "pos");

    animation->setDuration(1000);
    animation->setStartValue(posStart);
    animation->setEndValue(posEnd);

    animation->setEasingCurve(QEasingCurve::OutElastic);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void LoginDialog::on_cmbUsername_currentIndexChanged(const QString &userName)
{
    static QString defaultStylesheet = ui->ledPassword->styleSheet();

    if (userName == "guest")
    {
        ui->ledPassword->clear();
        ui->ledPassword->setReadOnly(true);
        ui->ledPassword->setStyleSheet("background-color: #CFC9CA;");
    } else {
        ui->ledPassword->setReadOnly(false);
        ui->ledPassword->setStyleSheet(defaultStylesheet);
    }
}

void LoginDialog::on_btnClose_clicked()
{
    reject();
}

void LoginDialog::on_btnLogIn_clicked()
{
    if( isValidUser() ){
        numOfRunApp++;
        writeSettings(); // TODO: ****** shahaydniaz nabashad
        accept();
    }else {
        qApp->beep();
        createPopupDialog(QString("کاربر نامعتبر"),
                          QString("نام کاربری یا کلمه عبور شما نامعتبر است"),
                          QPoint(), true, 2500, this)->show();
        ui->ledPassword->clear();
        ui->ledPassword->setFocus();
    }
}

PopupDialog *LoginDialog::createPopupDialog(QString title, QString body,
                                            QPoint xy, bool animate,
                                            int autoCloseDelay, QWidget *parent)
{
    PopupDialog *popupDialog = new PopupDialog(
                title, body , xy ,
                autoCloseDelay, parent
                );

    if (animate)
        popupDialog->animateWindow();

    //popupDialog->show();
    return popupDialog;
}

bool LoginDialog::isValidUser()
{
    QString userName = ui->cmbUsername->currentText();

    QString qryString =
            QString("SELECT username, is_admin, nickname FROM table_users WHERE username='%1' "
                    "AND password='%2' ").arg(
                userName, ui->ledPassword->text() );
    
    QSqlQuery qry(";", appInfo.db); // Oooops!
    if( qry.exec(qryString) )
    {
        if (qry.next()) {
            userInfo.userName = qry.value(0).toString();
            userInfo.isAdmin  = qry.value(1).toBool();
            userInfo.nickname = qry.value(2).toString();

            //if (userInfo.nickname.trimmed().isEmpty())
            //    userInfo.nickname = userInfo.userName;

            return true;
        } else if(userName == "guest") {
            userInfo.userName = "guest";
            userInfo.isAdmin  = false;
            userInfo.nickname = "کاربر میهمان";
            return true;
        } else {
            return false;
        }
    } else {
        qApp->beep();
        createPopupDialog(QString("اخطار"),
                          trUtf8("شرح خطا ") + ":"
                          + "\n" + qry.lastError().text(),
                          QPoint(), true, 5500, this)->show();
        return false;
    }

    return false;
}

void LoginDialog::on_btnCancelChangePassAndClose_clicked()
{
    on_btnClose_clicked();
}

void LoginDialog::on_btnOkSetPassAndLogIn_clicked()
{
    QString newPassword = ui->ledNewPassword->text();
    QString newPasswordAgain = ui->ledNewPasswordAgain->text();

    if (newPassword.isEmpty() || newPasswordAgain.isEmpty()) {
        qApp->beep();
        createPopupDialog( QString(" کلمه عبور نمی‌تواند خالی باشد. ") ,
                           QString() ,QPoint(), true, 2000, this )->show();
        return;
    }

    if ( newPassword.compare(newPasswordAgain) != 0 ) {
        qApp->beep();
        createPopupDialog( QString(" کلمه عبور و تکرا آن متفاوت است. ") ,
                           QString() ,QPoint(), true, 2000, this )->show();
        return;
    }

    //ایمیل را اختیاری کنم
    QString email = ui->ledEmailAddress->text().trimmed();

    //...

    if ( setUsersPass(newPassword) ) {
        animatePageLogin();
        //...
        numOfRunApp++;
        writeSettings();
    } else {
        qApp->beep();
        createPopupDialog( QString(" خطایی رخ داده است! ") ,
                           QString() ,QPoint(), true, 2000, this )->show();
    }

}

void LoginDialog::on_stackedWidgetMain_currentChanged(int arg1)
{
    switch (arg1) {
    case 0:  // pageLogin
        QTimer::singleShot(2000, this, SLOT(animateLoginLogo()) );
        break;
    case 1:  // pageChangePass
        ;
    default:
        break;
    }
}

//------------------------------------------------------------------------------

void LoginDialog::keyPressEvent(QKeyEvent *e)
{
    //agar karbar dokmeye Escape ra mizad moshkel ijad mishod, in ra neveshtam
    if( e->key() == Qt::Key_Escape )
        reject(); //close();
    //...
    QDialog::keyPressEvent(e);
}

void LoginDialog::mouseMoveEvent(QMouseEvent *e)
{
    if ((e->buttons() & Qt::LeftButton) && m_startDraging)
    {
        move(e->globalPos() - m_dragPosition);
        e->accept();
    }
    //...
    QDialog::mouseMoveEvent(e);
}

void LoginDialog::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        m_startDraging = true;
        m_dragPosition = e->globalPos() - frameGeometry().topLeft();
        e->accept();
    }
    //...
    QDialog::mousePressEvent(e);
}

void LoginDialog::mouseReleaseEvent(QMouseEvent *e)
{
    m_startDraging = false;
    //...
    QDialog::mouseReleaseEvent(e);
}

