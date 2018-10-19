#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtSql>
#include <QEvent>
#include <QMainWindow>
#include <QCloseEvent>
#include <QStackedWidget>

#include "popupdialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *);
    bool eventFilter(QObject *obj, QEvent *event);

private slots:
    void on_btnSearch_clicked();
    void on_actSearch_triggered();
    void on_actAdvancedSearch_triggered();

    void on_btnNext_clicked();
    void on_btnPrev_clicked();
    void on_btnLast_clicked();
    void on_btnFirst_clicked();

    void on_actGoToRecordN_triggered();
    void on_ledGoToRecordN_returnPressed();
    void on_ledGoToRecordN_editingFinished();

    void on_tabWidgetBooks_currentChanged(int index);

    void on_actAbout_triggered();
    void on_actCheckVersion_triggered();
    void on_actHelp_triggered();

    void on_actShowAllBooks_triggered();

    void on_actInsert_triggered();
    void on_btnInsertAndOk_clicked();

    void on_actUpdate_triggered();
    void on_btnUpdateAndOk_clicked();    

    void on_btnRemove_clicked();
    void on_actRemove_triggered();

    void on_btnCancelInsertOrUpdate_clicked();

    void on_actNavigationMode_triggered();
    void on_actBooksManagement_triggered();

    void on_actBackup_triggered();
    void on_actRestoreBackup_triggered();

    void on_actReenter_triggered();
    void on_actChangeLoginPass_triggered();
    void on_actQuit_triggered();

private:

    enum TabViews { TableView, FormView };

    enum { NormalMode, InsertionMode, UpdateMode } DataBaseMode;

    Ui::MainWindow *ui;

    QSqlQuery *qryTableBooks;
    QSqlQueryModel *qrySearchResult;

    // فیلد کلید رکورد انتخاب شده در نمای جدول را نگهداری میکند
    // چون ممکن است حین عملیات ویرایش کاربر در بین رکوردها حرکت کند و موقعیت رکورد
    // جاری از دست برود
    QString m_currKeyField;

    void setAdminWidgetsEnable(bool val=true);
    void setReadonlyFormViewsLineEdits(bool val=true);

    void readSettings();
    void writeSettings();

    PopupDialog *createPopupDialog(QString title = QString(),
                                   QString body = QString(),
                                   QPoint xy = QPoint(),
                                   bool animate = true,
                                   int autoCloseDelay = 0,
                                   QWidget *parent = 0);

    void resetFormView();
    void clearFormViewsLineEdits();
    bool isFormViewsLineEditsEmpty(bool markLineEditIfIsEmpty=true);
    void setFormViewsLineEditsStylesheet(const QString &s);
    void fillFormFromTable();

    void switchBetweenStackedWidgets(QWidget *targetPage,
                                     QStackedWidget *stackedWidget);

    void setupTableBooks();

    void updateLblRecordStatistics();

};

#endif // MAINWINDOW_H
