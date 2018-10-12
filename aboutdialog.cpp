#include "aboutdialog.h"
#include "ui_aboutdialog.h"

#include <QTimer>

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::SplashScreen);

    //...

    rectInWindow = ui->lblVersionInfo->geometry();

    QRect rect = ui->lblVersionInfo->geometry();
    rect.setX(rect.x() + rect.width() + 100);
    rectOutOfWindow = rect;
    ui->lblVersionInfo->setGeometry(rect);
    QTimer::singleShot(750, this, SLOT(animateLabelVersionInfo()) );

    QTimer::singleShot(10000, this, SLOT(animateLabelVersionInfoReverse()));
    //...

    QString strVersion = ui->lblVersionInfo->text().replace(
                "VERSION", qApp->applicationVersion() );
    ui->lblVersionInfo->setText(strVersion);
    //...

}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::on_btnOk_clicked()
{
    this->close();
    //accept();
}

void AboutDialog::animateLabelVersionInfo()
{
    QPropertyAnimation *anim = new QPropertyAnimation(ui->lblVersionInfo,
                                                      "geometry", this);
    anim->setDuration(800);

    anim->setStartValue(rectOutOfWindow);
    anim->setEndValue(rectInWindow);

    anim->setEasingCurve(QEasingCurve::InBounce);
    anim->start(QPropertyAnimation::DeleteWhenStopped);
}

void AboutDialog::animateLabelVersionInfoReverse()
{
    QPropertyAnimation *anim = new QPropertyAnimation(ui->lblVersionInfo,
                                                      "geometry", this);
    anim->setDuration(800);

    anim->setStartValue(rectInWindow);
    anim->setEndValue(rectOutOfWindow);

    anim->setEasingCurve(QEasingCurve::OutBounce);
    anim->start(QPropertyAnimation::DeleteWhenStopped);
}
