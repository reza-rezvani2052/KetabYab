#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include <QPropertyAnimation>

#include "draggabledialog.h"

namespace Ui {
class AboutDialog;
}

class AboutDialog : public DraggableDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = 0);
    ~AboutDialog();

private slots:
    void on_btnOk_clicked();

    void animateLabelVersionInfo();
    void animateLabelVersionInfoReverse();

private:
    Ui::AboutDialog *ui;

    //baraye animation
    QRect rectInWindow;
    QRect rectOutOfWindow;

};

#endif // ABOUTDIALOG_H
