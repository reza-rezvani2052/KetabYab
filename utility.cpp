#include "utility.h"

#include <QScreen>
#include <QSettings>
#include <QApplication>
#include <QDesktopWidget>

#include "dbconnection.h"   // AppInfo Struct definition


extern AppInfo  appInfo;


Utility::Utility(QObject *parent) : QObject(parent)
{
    m_timer.setSingleShot(true);
    connect(&m_timer, &QTimer::timeout, &m_eventLoop, &QEventLoop::quit);
    //...
}

void Utility::delay(int msec)
{
    m_timer.start(msec);
    m_eventLoop.exec();
}

//---------------------------------------------------------------------------------------

QSize Utility::getScreenSize()
{
    //    QRect rec = QApplication::desktop()->screenGeometry();
    //    int height = rec.height();
    //    int width = rec.width();
    //...
    QSize size = qApp->screens()[0]->size();
    return size;
}

int Utility::getScreenWidth()
{
    QSize size = qApp->screens()[0]->size();
    return size.width();
}

int Utility::getScreenHeight()
{
    QSize size = qApp->screens()[0]->size();
    return size.height();
}

//---------------------------------------------------------------------------------------

void Utility::saveDatabasePath()
{
    QSettings settings;
    settings.beginGroup("Main");
    //...
    settings.setValue("DatabasePath", appInfo.databasePath);
    //...
    settings.endGroup();
}

//---------------------------------------------------------------------------------------

PopupDialog *Utility::createPopupDialog(QString title, QString body, QPoint xy,
                                        bool animate, int autoCloseDelay,
                                        bool playBeep, QWidget *parent)
{
    PopupDialog *popupDialog = new PopupDialog(
                title, body , xy , autoCloseDelay, parent );

    if (animate)
        popupDialog->animateWindow();

    if(playBeep)
        qApp->beep();

    //popupDialog->show();
    return popupDialog;
}
