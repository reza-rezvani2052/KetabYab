#ifndef UTILITY_H
#define UTILITY_H

#include <QTimer>
#include <QObject>
#include <QEventLoop>

class Utility : public QObject
{
    Q_OBJECT

public:
    explicit Utility(QObject *parent = nullptr);

    void delay(int msec);

    static QSize getScreenُSize();
    static int getScreenWidth();
    static int getScreenHeight();

private:
    QTimer m_timer;
    QEventLoop m_eventLoop;

};

#endif // UTILITY_H
