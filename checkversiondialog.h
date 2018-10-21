#ifndef CHECKVERSIONDIALOG_H
#define CHECKVERSIONDIALOG_H

#include <QDialog>

#include <QTimer>
#include <QNetworkReply>
#include <QPropertyAnimation>
#include <QNetworkAccessManager>

#include "draggabledialog.h"

namespace Ui {
class CheckVersionDialog;
}

class CheckVersionDialog : public DraggableDialog
{
    Q_OBJECT

public:
    explicit CheckVersionDialog(QWidget *parent = 0);
    ~CheckVersionDialog();

private slots:
    void on_btnClose_clicked();

    void checkNewVersionAvilable();
    void replyFinished(QNetworkReply *reply);

    void timeOuted();

private:
    Ui::CheckVersionDialog *ui;

    QNetworkReply *reply;
    QNetworkAccessManager *m_netManager;

    QTimer timerTimeout;

};

#endif // CHECKVERSIONDIALOG_H
