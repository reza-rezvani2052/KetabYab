#ifndef UTILITY_H
#define UTILITY_H

#include <QTimer>
#include <QObject>
#include <QEventLoop>

#include "popupdialog.h"

class Utility : public QObject
{
    Q_OBJECT

public:
    explicit Utility(QObject *parent = nullptr);

    void delay(int msec);

    static QSize getScreenُSize();
    static int getScreenWidth();
    static int getScreenHeight();

    // تعداد دفعاتی که برنامه اجرا شده است
    static int  getNumOfRunApp();
    static void setNumOfRunApp(int times = 1);

    static PopupDialog *createPopupDialog(QString title = QString(),
                                          QString body = QString(),
                                          QPoint xy = QPoint(),
                                          bool animate = true,
                                          int autoCloseDelay = 0,
                                          QWidget *parent = 0);

    static void saveDatabasePath();

private:
    QTimer m_timer;
    QEventLoop m_eventLoop;

};

#endif // UTILITY_H
