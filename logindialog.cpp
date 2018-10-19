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
    //...
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
    //...
    settings.setValue("NumOfRunApp", numOfRunApp);
    //...
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
    if( isValidUser() ) {
        numOfRunApp++;
        writeSettings(); // TODO: ****** shahaydniaz nabashad
        accept();
    } else {
        QString str = QString("نام کاربری یا کلمه عبور شما نامعتبر است");

        // تعداد تلاش های ناموفقی که کاربر برای ورود به برنامه انجام میدهد را برای نمایش
        // راهنمای پسورد استفاده میکنیم
        static int numRetry = 0;

        QString passHint = getUserPassHint(ui->cmbUsername->currentText());
        bool canShowPassHint = (++numRetry > 3) && (!passHint.isEmpty());
        if ( canShowPassHint ) {
            str += "\n";
            str += "می‌توانید از یادآور زیر جهت بازیابی کلمه عبور خود استفاده کنید:";
            str += "\n";
            str += passHint ;
        }

        //...

        qApp->beep();
        createPopupDialog(QString("کاربر نامعتبر"), str, QPoint(), true,
                          canShowPassHint ? 5000 : 2500, this)->show();
        ui->ledPassword->clear();
        ui->ledPassword->setFocus();
    }
}

PopupDialog *LoginDialog::createPopupDialog(QString title, QString body,
                                            QPoint xy, bool animate,
                                            int autoCloseDelay, QWidget *parent)
{
    PopupDialog *popupDialog = new PopupDialog( title, body , xy ,
                                                autoCloseDelay, parent);

    if (animate)
        popupDialog->animateWindow();

    return popupDialog;
}

bool LoginDialog::isValidUser()
{
    QString userName = ui->cmbUsername->currentText();

    QString qryString =
            QString("SELECT * FROM table_users WHERE username='%1' AND password='%2' ").arg( userName, ui->ledPassword->text() );
    
    QSqlQuery qry(";", appInfo.db); // Oooops!
    if( qry.exec(qryString) )
    {
        if (qry.next()) {
            userInfo.userName = qry.value(UserName).toString();

            userInfo.password  = qry.value(Password).toString();

            userInfo.passHint = qry.value(PassHint).toString();

            userInfo.isAdmin = qry.value(IsAdmin).toBool();

            userInfo.nickname = qry.value(Nickname).toString();
            //if (userInfo.nickname.trimmed().isEmpty())
            //    userInfo.nickname = userInfo.userName;

            return true;
        } else if(userName == "guest") {
            userInfo.userName = "guest";
            userInfo.isAdmin  = false;
            userInfo.passHint = "فاقد کلمه عبور";
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

    //...
    // در اولین اجرای برنامه مقدار
    // userInfo.userName
    // خالی میباشد. بنابراین با مقدار زیر آنرا مقدار دهی میکنیم
    userInfo.userName = "admin";

    QString passHint  = ui->ledPassHint->text().trimmed();

    if (setUsersPass(newPassword, passHint)) {

        // چون کاربر پیش فرض میهمان است و بعد از ست کردن کلمه عبور حتما کاربر مدیر قصد ورود
        // به سامانه را دارد، در اجرای اول، کاربر مدیر را پیش فرض قرار میدهیم
        ui->cmbUsername->setCurrentIndex( ui->cmbUsername->findText(userInfo.userName) );

        animatePageLogin();
        //...
        numOfRunApp++;
        writeSettings();
    } else {
        qApp->beep();
        createPopupDialog( QString(" خطایی رخ داده است! ") ,
                           QString() ,QPoint() , true, 2000, this )->show();
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

