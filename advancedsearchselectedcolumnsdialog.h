#ifndef ADVANCEDSEARCHSELECTEDCOLUMNSDIALOG_H
#define ADVANCEDSEARCHSELECTEDCOLUMNSDIALOG_H

//#include <QMap>
#include <QDialog>


namespace Ui {
class AdvancedSearchSelectedColumnsDialog;
}

class AdvancedSearchSelectedColumnsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AdvancedSearchSelectedColumnsDialog( QDialog *parent = 0);
    ~AdvancedSearchSelectedColumnsDialog();

    QStringList getSelectedColumns();


private slots:
    void on_btnOk_clicked();
    void on_btnCancel_clicked();

private:
    Ui::AdvancedSearchSelectedColumnsDialog *ui;
    //QMap m_selectedColumns<int, int>;
};

#endif // ADVANCEDSEARCHSELECTEDCOLUMNSDIALOG_H
