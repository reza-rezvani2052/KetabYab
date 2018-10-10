#include "advancedsearchselectedcolumnsdialog.h"
#include "ui_advancedsearchselectedcolumnsdialog.h"

#include "dbconnection.h"


AdvancedSearchSelectedColumnsDialog::AdvancedSearchSelectedColumnsDialog(QDialog *parent):
    QDialog(parent), ui(new Ui::AdvancedSearchSelectedColumnsDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::SplashScreen);

    //...

    //TODO: شاید نیاز نباشد
    //resize( sizeHint() );
}

AdvancedSearchSelectedColumnsDialog::~AdvancedSearchSelectedColumnsDialog()
{
    delete ui;
}

void AdvancedSearchSelectedColumnsDialog::on_btnCancel_clicked()
{
    reject();
}

void AdvancedSearchSelectedColumnsDialog::on_btnOk_clicked()
{
    accept();
}

QStringList AdvancedSearchSelectedColumnsDialog::getSelectedColumns()
{
    QStringList list;
    const int rowCount = ui->listWidget->count();

    for (int row=0; row<rowCount; row++)
    {
        if ( ui->listWidget->item(row)->checkState() == Qt::Checked )
        {
            switch (row+1) {
            case BookTitle:
                list.append("عنوان کتاب");
                break;
            case BookWriter:
                list.append("پدید آور");
                break;
            case BookTranslator:
                list.append("مترجم");
                break;
            case BookPub:
                list.append("انتشارات");
                break;
            case BookTopic:
                list.append("موضوع");
                break;
            case BookRegisterNumber:
                list.append("شماره ثبت");
                break;
            }
        }
    }

    return list;
}
