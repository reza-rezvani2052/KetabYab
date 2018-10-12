#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include <QPropertyAnimation>

namespace Ui {
class AboutDialog;
}

class AboutDialog : public QDialog
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
