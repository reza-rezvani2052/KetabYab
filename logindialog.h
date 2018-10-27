#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

class QStackedWidget;


#include <QtSql>
#include <QDialog>
#include <QCompleter>
#include <QMouseEvent>

#include "popupdialog.h"
#include "draggabledialog.h"

class QKeyEvent ;    // baraye modiriate dokmeye 'Escape'

namespace Ui {
    class LoginDialog;
}

class LoginDialog : public DraggableDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = 0);
    ~LoginDialog();

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

    bool isValidUser();

    PopupDialog *createPopupDialog(QString title = QString(),
                                   QString body = QString(),
                                   QPoint xy = QPoint(),
                                   bool animate = true,
                                   int autoCloseDelay = 0,
                                   QWidget *parent = 0);   

};

#endif // LOGINDIALOG_H
