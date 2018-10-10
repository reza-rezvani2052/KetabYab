#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::SplashScreen);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::on_btnOk_clicked()
{
    this->close();
}
