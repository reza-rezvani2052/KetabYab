#include "popupdialog.h"
#include "ui_popupdialog.h"

#include <QDebug>
#include <QTimer>
#include <QKeyEvent>
#include <QDesktopWidget>
#include <QPropertyAnimation>


PopupDialog::PopupDialog(QString title, QString body,
                         QPoint xy, int autoCloseDelay,
                         QWidget *parent):
    QWidget(parent),
    ui(new Ui::PopupDialog)
{
    ui->setupUi(this);
    m_duration = autoCloseDelay;

    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(Qt::Popup);
    //...
    if( title.isEmpty() )
        ui->lblTitle->setVisible(false);
    else
        ui->lblTitle->setText( title );

    if( body.isEmpty() )
        ui->lblBody->setVisible(false);
    else
        ui->lblBody->setText( body );

    //...

    this->show();

    if( parent )
        centerWidgets(this, parent);

    if( xy != QPoint() )
        this->move(xy);

    //...

    if( autoCloseDelay > 0 )
        QTimer::singleShot(autoCloseDelay, this, SLOT(animateWindowOnClose()) );

    //...
    installEventFilter(this);
    //...

}

PopupDialog::~PopupDialog()
{    
    delete ui;
}

void PopupDialog::setTitleText(const QString &text)
{   ui->lblTitle->setText(text);  }

void PopupDialog::setBodyText(const QString &text)
{   ui->lblBody->setText(text);   }

void PopupDialog::setTitleVisible(bool val)
{   ui->lblTitle->setVisible(val);  }

void PopupDialog::setBodyVisible(bool val)
{   ui->lblBody->setVisible(val);  }

void PopupDialog::setWindowStyleSheet(const QString &styleSheet)
{   setStyleSheet(styleSheet);     }

void PopupDialog::centerWidgets(QWidget *widget, QWidget *host)
{
    if (!host)
        host = widget->parentWidget();

    if (host) {
        auto hostRect = host->geometry();
        widget->move(hostRect.center() - widget->rect().center());
    } else {
        QRect screenGeometry = QApplication::desktop()->screenGeometry();
        int x = (screenGeometry.width() - widget->width()) / 2;
        int y = (screenGeometry.height() - widget->height()) / 2;
        widget->move(x, y);
    }
}

void PopupDialog::animateWindow(int duration, int startValue, int endValue)
{
    QPropertyAnimation *animation = new QPropertyAnimation(
                this,
                "windowOpacity");

    animation->setDuration(duration);
    animation->setStartValue(startValue);
    animation->setEndValue(endValue);

    //animation->setEasingCurve(QEasingCurve::InOutCubic);
    animation->start(QAbstractAnimation::DeleteWhenStopped);

    //...
    //Fadeout:
    if (startValue > endValue)
        QTimer::singleShot(duration + 200 , this, SLOT(close()));
}

void PopupDialog::animateWindowOnClose()
{   animateWindow(300, 1.0, 0); }

int PopupDialog::getDuration() const
{ return m_duration; }

void PopupDialog::setDuration(const int duration)
{ m_duration = duration; }

bool PopupDialog::eventFilter(QObject *obj, QEvent *event)
{
    switch ( event->type() ) {
    case QEvent::KeyPress: {
        // اگر پنجره پاپاپ نمایش داده شد کاربر بازدن دکمه اینتر و یا دکمه اسکیپ
        // بتواند پنجره را سریعا ببندد
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if ( keyEvent->key() == Qt::Key_Enter ||  keyEvent->key() == Qt::Key_Return) {
            animateWindowOnClose();
            return true;
        }
        break;
    }
    default:
        return QObject::eventFilter(obj, event);    // standard event processing
    }
}
