#include "utility.h"

#include <QScreen>
#include <QApplication>
#include <QDesktopWidget>

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

QSize Utility::getScreenُSize()
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
