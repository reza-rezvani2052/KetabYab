#include "utility.h"

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
