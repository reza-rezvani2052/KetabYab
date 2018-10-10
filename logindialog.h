#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

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
    void keyPressEvent(QKeyEvent *e);

    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

private slots:
    void on_btnLogIn_clicked();
    void on_btnCancel_clicked();

    void animateLoginLogo();

    void on_cmbUsername_currentIndexChanged(const QString &userName);

private:
    Ui::LoginDialog *ui;
    QCompleter *compUserName;

    bool m_startDraging;
    QPoint m_dragPosition;

    bool isValidUser();

    PopupDialog *createPopupDialog(QString title = QString(),
                                   QString body = QString(),
                                   QPoint xy = QPoint(),
                                   bool animate = true,
                                   int autoCloseDelay = 0,
                                   QWidget *parent = 0);
};

#endif // LOGINDIALOG_H
