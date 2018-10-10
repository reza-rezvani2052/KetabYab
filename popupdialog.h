#ifndef POPUPDIALOG_H
#define POPUPDIALOG_H

#include <QWidget>

namespace Ui {
class PopupDialog;
}

class PopupDialog : public QWidget
{
    Q_OBJECT

public:
    //explicit PopupDialog(QWidget *parent = 0);
    PopupDialog( QString title = QString(),
                 QString body = QString(),
                 QPoint xy = QPoint(),
                 int autoCloseDelay = 0,
                 QWidget *parent = 0 );

    ~PopupDialog();

    void setBodyText(const QString &text);
    void setTitleText(const QString &text);

    void setBodyVisible(bool val);
    void setTitleVisible(bool val);

    void setWindowStyleSheet(const QString &styleSheet);

    void centerWidgets(QWidget *widget,
                       QWidget *host = 0);

    void animateWindow(int duration = 300,
                               int startValue = 0,
                               int endValue = 1.0);

    int getDuration() const;
    void setDuration(const int duration);

public slots:
    void animateWindowOnClose();

private:
    Ui::PopupDialog *ui;
    int m_duration;

};

#endif // POPUPDIALOG_H
