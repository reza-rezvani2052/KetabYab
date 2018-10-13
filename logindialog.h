#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

class QStackedWidget;


#include <QtSql>
#include <QDialog>
#include <QCompleter>
#include <QMouseEvent>

#include "popupdialog.h"

class QKeyEvent ;    // baraye modiriate dokmeye 'Escape'

namespace Ui {
    class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = 0);
    ~LoginDialog();

protected:
    void closeEvent(QCloseEvent *);

    void keyPressEvent(QKeyEvent *e);

    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

private slots:
    void on_btnLogIn_clicked();
    void on_btnClose_clicked();

    void animateLoginLogo();
    void animatePageLogin();
    void animateStackedWidgetPages(QStackedWidget *stackedWidget, QWidget *page);

    void on_cmbUsername_currentIndexChanged(const QString &userName);

    void on_btnOkSetPassAndLogIn_clicked();
    void on_btnCancelChangePassAndClose_clicked();

    void on_stackedWidgetMain_currentChanged(int arg1);

private:
    Ui::LoginDialog *ui;
    QCompleter *compUserName;

    // دفعات اجرای برنامه
    int numOfRunApp;

    bool m_startDraging;
    QPoint m_dragPosition;

    bool isValidUser();

    PopupDialog *createPopupDialog(QString title = QString(),
                                   QString body = QString(),
                                   QPoint xy = QPoint(),
                                   bool animate = true,
                                   int autoCloseDelay = 0,
                                   QWidget *parent = 0);   

    void readSettings();
    void writeSettings();

};

#endif // LOGINDIALOG_H
