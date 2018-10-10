#ifndef ADVANCEDSEARCHDIALOG_H
#define ADVANCEDSEARCHDIALOG_H

#include <QHash>
#include <QDialog>


class QComboBox;
class QLineEdit;
class PopupDialog;
class QListWidgetItem;
class AdvancedSearchSelectedColumnsDialog;

namespace Ui {
class AdvancedSearchDialog;
}

class AdvancedSearchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AdvancedSearchDialog(QWidget *parent = 0);
    ~AdvancedSearchDialog();

    QStringList getColumnNames();
    QString getAdvancedQueryString();

private slots:
    void on_btnOk_clicked();
    void on_btnCancel_clicked();
    void on_btnCreateQuery_clicked();

    void on_btnFieldChoise_clicked();

    void on_groupBoxQuery_clicked(bool checked);

    void on_btnAddCond_clicked();
    void on_tabWidgetConditions_tabCloseRequested(int index);  

private:
    Ui::AdvancedSearchDialog *ui;
    AdvancedSearchSelectedColumnsDialog *m_dialog;

    enum ComparativeOperators{
        EqualTo,
        NotEqualTo,
        LessThan,
        LessThanOrEqualTo,
        GreaterThan,
        GreaterThanOrEqualTo,
        Like,
        NotLike
    };

    enum LogicalOperators {
        END_OF_CONDITION,
        AND,
        OR,
        AND_NOT,
        OR_NOT
    };

    void initializeTabWidgetConditions();

    void updateCmbOrderBy();

    QString prepareWhereExpression();
    QString composeWhereExpression(QComboBox *cmbRightOperand, QComboBox *cmbOperator,
                                   QLineEdit *ledLeftOperand , QComboBox *cmbLogicCond);

    PopupDialog *createPopupDialog(QString title, QString body,
                                   QPoint xy, bool animate,
                                   int autoCloseDelay, QWidget *parent);
};

#endif // ADVANCEDSEARCHDIALOG_H







