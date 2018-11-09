#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QRegExp>
#include <QProcess>
#include <QSettings>
#include <QValidator>
#include <QFileDialog>
#include <QMessageBox>
#include <QActionGroup>
#include <QStandardPaths>
#include <QRegExpValidator>
#include <QDesktopServices>
#include <QPropertyAnimation>

#include "Utility.h"
#include "popupdialog.h"
#include "aboutdialog.h"
#include "dbconnection.h"
#include "checkversiondialog.h"
#include "advancedsearchdialog.h"
#include "changepassworddialog.h"

//---------------------------------------------------------------------

extern AppInfo appInfo;
extern UserInfo userInfo;

//---------------------------------------------------------------------

#define RETURN_IF_TABLE_BOOKS_IS_EMPTY int _rowCount = ui->tableBooks->rowCount(); \
    if (!_rowCount) \
    return;

#define DB_BOOKS_COLUMN_COUNT DB_Books_Fields::DB_Books_Fields_Count

//---------------------------------------------------------------------


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    completerSearchHistory = 0;
    //...
    QRegExp rx("[1-9]\\d{0,6}");
    QValidator *validator = new QRegExpValidator(rx, this);
    ui->ledGoToRecordN->setValidator(validator);

    ui->ledBookRegisterNumber->setValidator(validator);

    ui->ledGoToRecordN->setVisible(false);
    ui->ledGoToRecordN->installEventFilter(this);
    //...
    connect(ui->ledSearchTopic, &QLineEdit::returnPressed,
            ui->btnSearch, &QPushButton::click );
    //...
    readSettings();
    //...
    QFont menuBarFont = ui->menuBar->font();
    menuBarFont.setPointSize(menuBarFont.pointSize() + 1);
    ui->menuBar->setFont(menuBarFont);
    //...

    // Setup Table Search Result
    qrySearchResult = new QSqlQueryModel(this);
    //qrySearchResult->setQuery ("SELECT * FROM table_books", appInfo.db);
    ui->tableSearchResult->setModel(qrySearchResult);

    //...

    QActionGroup *actionGroup = new QActionGroup(this);
    actionGroup->addAction(ui->actNavigationMode);
    actionGroup->addAction(ui->actBooksManagement);
    actionGroup->setExclusive(true);
    ui->actNavigationMode->setCheckable(true);
    ui->actBooksManagement->setCheckable(true);
    ui->actNavigationMode->setChecked(true);
    //ui->actBooksManagement->setChecked(false);

    //...

    setAdminWidgetsEnable(userInfo.isAdmin);

    if (userInfo.isAdmin) {
        ui->tableBooks->setContextMenuPolicy(Qt::ActionsContextMenu);
        ui->tableBooks->addActions( ui->mnuTableBooksContextMenu->actions() );
    }

    //...

    QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );
    this->show();

    //TODO: در زمان لود با داده زیاد کمی زمانبر است
    qryTableBooks = new QSqlQuery(QString(), appInfo.db);  //SELECT * FROM table_books;
    setupTableBooks();
    on_btnFirst_clicked();

    // برای غیر فعال کردن دکمه های ناوبری به این نیاز است
    fillFormFromTable();

    QApplication::restoreOverrideCursor();

    //..
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnSearch_clicked()
{
    if (ui->tableBooks->rowCount() == 0) {
        qApp->beep();
        Utility::createPopupDialog( QString("لیسست کتاب خالی می‌باشد") ,
                                    QString() ,QPoint(), true, 2000, this )->show();
        return;
    }
    //...
    QString searchString = ui->ledSearchTopic->text().trimmed();
    if (searchString.isEmpty())
    {
        qApp->beep();
        QPoint xy = ui->ledSearchTopic->mapToGlobal(QPoint(0,0));

        //...
        //اگر کاربر پنجره اصلی برنام را به گوشه برده باشد و این پیام نمایش داده شود
        //این پنجره از کادر خارج میشد؛ با کد زیر این مشکل را اصلاح کردم
        int screenWidth = Utility::getScreenWidth();
        //...

        PopupDialog *popupDialog = createPopupDialog(
                    QString("خطای جستجو!") , QString("کادر جستجو خالی می‌باشد!") + QString("\n") +
                    QString("ابتدا عبارت مورد نظر را در کادر جستجو نوشته و سپس بر روی دکمه جستجو کلیک نمایید."),
                    xy, true, 5500, this );
        popupDialog->show();

        int availableWidth = screenWidth - ( xy.x() + popupDialog->width() );
        if ( availableWidth <= 0 )
            xy.setX( xy.x() - qAbs(availableWidth) - 3 );

        popupDialog->move( xy.x() , xy.y() - popupDialog->height() );

        ui->ledSearchTopic->setFocus();
        return;
    }

    //...

    QString strQry = "SELECT * FROM table_books WHERE";

    switch ( ui->cmbSearchTopic->currentIndex())
    {
    case BookRegisterNumber:strQry += " book_register_number LIKE ";   break;
    case BookTitle:         strQry += " book_title LIKE " ;            break;
    case BookWriter:        strQry += " book_writer LIKE " ;           break;
    case BookTranslator:    strQry += " book_translator LIKE " ;       break;
    case BookPub:           strQry += " book_pub LIKE " ;              break;
    case BookTopic:         strQry += " book_topic LIKE " ;            break;
    }

    strQry += QString("'\%%1\%'").arg( ui->ledSearchTopic->text().trimmed() );

    //NOTE: اگر در هنگام کار با داده‌های بزرگ رابط کاربری فریز شد، در یک ترد بنویسم
    qrySearchResult->setQuery (strQry, appInfo.db);

    //...
    qrySearchResult->setHeaderData (0, Qt::Horizontal, "شماره ثبت");
    qrySearchResult->setHeaderData (1, Qt::Horizontal, "عنوان کتاب");
    qrySearchResult->setHeaderData (2, Qt::Horizontal, "پدید آور");
    qrySearchResult->setHeaderData (3, Qt::Horizontal, "مترجم");
    qrySearchResult->setHeaderData (4, Qt::Horizontal, "انتشارات");
    qrySearchResult->setHeaderData (5, Qt::Horizontal, "موضوع");
    //...

    if (qrySearchResult->rowCount() == 0) {
        qApp->beep();
        Utility::createPopupDialog(QString("موردی یافت نشد") , QString(),
                                   QPoint(), true, 1400, this)->show();
    } else {
        setMostSearchedPhrase(searchString);
        ui->tableSearchResult->resizeColumnsToContents();
    }
}

void MainWindow::on_actSearch_triggered()
{
    if (ui->stackedWidget->currentWidget() != ui->pageSearch )
        switchBetweenStackedWidgets(ui->pageSearch, ui->stackedWidget);

    //...

    //TODO: با روش زیر مطالب جستجو شده اخیر در برنامه در اجرای بعدی برنامه
    // نمایش داده میشود. به نظر این روش بد نیست
    if (completerSearchHistory == 0) {

        QStringList mostSearchedPhrases =  getMostSearchedPhrases();

        if (mostSearchedPhrases.size() > 1000 && userInfo.isAdmin ) {
            QMessageBox msgBox(this);
            msgBox.addButton("بله", QMessageBox::YesRole);
            msgBox.addButton("خیر", QMessageBox::NoRole );
            msgBox.setWindowTitle("حذف تاریخچه جستجو");
            msgBox.setText(
                        trUtf8("تاریخچه جستجو کمی حجیم شده است!") +
                        QString("\n") +
                        trUtf8("وجود تاریخچه جستجو حجیم ممکن است سبب اندکی کاهش سرعت در نمایش تاریخچه شود.")
                        + QString("\n") + QString("\n") +
                        trUtf8("آیا مایل به حذف تاریخچه جستجو هستید؟")
                        );
            msgBox.setIcon(QMessageBox::Information);

            if (msgBox.exec() == QMessageBox::AcceptRole) {
                clearSearchHistory(false);

                mostSearchedPhrases.clear();
            }
        }

        completerSearchHistory = new QCompleter(mostSearchedPhrases, this);
        completerSearchHistory->setCaseSensitivity(Qt::CaseInsensitive);
        completerSearchHistory->setMaxVisibleItems(5);

        ui->ledSearchTopic->setCompleter(completerSearchHistory);
    }
}

void MainWindow::clearSearchHistory(bool askBeforeClear)
{
    if (askBeforeClear)
    {
        QMessageBox msgBox(this);
        msgBox.addButton("بله", QMessageBox::YesRole);
        msgBox.addButton("خیر", QMessageBox::NoRole );
        msgBox.setWindowTitle("تایید حذف تاریخچه جستجو");
        msgBox.setText( trUtf8("آیا مایل به حذف تاریخچه جستجو هستید؟") );
        msgBox.setIcon(QMessageBox::Warning);

        if (msgBox.exec() == QMessageBox::AcceptRole) {
            clearAllSearchHistory();

            //FIXME: *****  بصورت آنی تاریخچه اعمال شود؛ نه در اجرای بعدی برنامه
            if (completerSearchHistory != 0) {
                delete completerSearchHistory;
                completerSearchHistory = new QCompleter( QStringList(), this);
                completerSearchHistory->setCaseSensitivity(Qt::CaseInsensitive);
                completerSearchHistory->setMaxVisibleItems(5);

                ui->ledSearchTopic->setCompleter(completerSearchHistory);
            }
        }

    } else {
        clearAllSearchHistory();

        //FIXME: *****  بصورت آنی تاریخچه اعمال شود؛ نه در اجرای بعدی برنامه
        if (completerSearchHistory != 0) {
            delete completerSearchHistory;
            completerSearchHistory = new QCompleter( QStringList(), this);
            completerSearchHistory->setCaseSensitivity(Qt::CaseInsensitive);
            completerSearchHistory->setMaxVisibleItems(5);

            ui->ledSearchTopic->setCompleter(completerSearchHistory);
        }
    }
}

void MainWindow::on_btnClearSearchHistory_clicked()
{
    clearSearchHistory();
}

void MainWindow::on_actAdvancedSearch_triggered()
{
    qApp->beep();
    Utility::createPopupDialog( QString("آزمایشی!") ,
                                QString(" در نسخه‌های آتی این بخش پیاده‌سازی خواهد شد. ") ,
                                QPoint(), true, 3000, this )->show();
    return;

    //----------------------------------------------------------------

    AdvancedSearchDialog dialog(this);
    if ( dialog.exec()== QDialog::Accepted )
    {
        if (ui->tableBooks->rowCount() == 0)
        {
            qApp->beep();
            Utility::createPopupDialog( QString("لیسست کتاب‌ها خالی می‌باشد") ,
                                        QString() ,QPoint(), true, 2000, this )->show();
            return;
        }
        //...
        on_actSearch_triggered();
        //...

        //TODO:
        /*
        اگر کوئری ساخته شده در جستجوی پیشرفته کلماتی مانند
        delete یا update
        داشت به کاربر هشدار دهم و یا اصلا اجرا نکنم
        */
        QString strQuery = dialog.getAdvancedQueryString();
        qrySearchResult->setQuery (strQuery, appInfo.db);

        //qDebug() << strQuery;

        //...

        QStringList list = dialog.getColumnNames();

        for (int i=0; i<list.length(); i++)
            qrySearchResult->setHeaderData (i, Qt::Horizontal, list.at(i) );
        //...

        if (qrySearchResult->rowCount() == 0) {
            qApp->beep();
            Utility::createPopupDialog(QString("موردی یافت نشد") ,
                                       QString(), QPoint(), true, 1400, this)->show();
        }

        ui->tableSearchResult->resizeColumnsToContents();
    }
}

void MainWindow::on_actShowAllBooks_triggered()
{
    if (ui->stackedWidget->currentWidget() != ui->pageBooks )
        switchBetweenStackedWidgets( ui->pageBooks, ui->stackedWidget );
    //...
    ui->tabWidgetBooks->setCurrentWidget( ui->tabTableView );
}

void MainWindow::setAdminWidgetsEnable(bool val)
{
    ui->actBackup->setEnabled(val);
    ui->actRestoreBackup->setEnabled(val);
    ui->actChangeLoginPass->setEnabled(val);

    //Hide mnuTableBooksContextMenu from menu bar
    ui->mnuTableBooksContextMenu->menuAction()->setVisible(false);
    ui->actRemove->setEnabled(val);
    ui->actInsert->setEnabled(val);
    ui->actUpdate->setEnabled(val);

    ui->actBooksManagement->setEnabled(val);

    ui->actCheckVersion->setEnabled(val);

    ui->pageEditMode->setEnabled(val);

    //...

    //NOTE: در صورت ورود با کاربر "میهمان" ؛ آیتم ها اینجا هندل شوند
    if (!val) {
        QString str = "در حالت کاربر میهمان این آیتم در دسترس نمی‌باشد.";

        ui->actBackup->setStatusTip(str);
        ui->actRestoreBackup->setStatusTip(str);
        ui->actChangeLoginPass->setStatusTip(str);

        ui->actInsert->setStatusTip(str);
        ui->actRemove->setStatusTip(str);
        ui->actUpdate->setStatusTip(str);

        ui->actBooksManagement->setStatusTip(str);

        ui->actCheckVersion->setStatusTip(str);

        ui->btnClearSearchHistory->setEnabled(false);
        ui->btnClearSearchHistory->setVisible(false);

        //ui->pageEditMode->setStatusTip(str);
    }
}

void MainWindow::on_actAbout_triggered()
{
    AboutDialog aboutDialog(this);
    aboutDialog.exec();
}

void MainWindow::closeEvent(QCloseEvent *)
{
    writeSettings();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == ui->ledGoToRecordN)
    {
        QString txt = ui->ledGoToRecordN->text();
        //...
        switch ( event->type() ) {
        case QEvent::KeyPress: {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

            if ( keyEvent->key() == Qt::Key_Escape )
            {
                ui->ledGoToRecordN->setVisible(false);
                return true;
            }
            break;
        }
        case QEvent::FocusOut:
            if ( txt.isEmpty() )
            {
                ui->ledGoToRecordN->setVisible(false);
                return true;
            } else {
                return true;
            }
            break;
        }
    }

    return QObject::eventFilter(obj, event);    // standard event processing
}

void MainWindow::readSettings()
{
    QSettings settings;
    settings.beginGroup("MainWindow");
    //...
    restoreGeometry(settings.value("Geometry").toByteArray());
    //...
    settings.endGroup();
}

void MainWindow::writeSettings()
{
    if (userInfo.isAdmin)
    {
        QSettings settings;
        settings.beginGroup("MainWindow");
        //...
        settings.setValue("Geometry", saveGeometry());
        //...
        settings.endGroup();
        //...
        // Utility::saveDatabasePath();
    }
}

PopupDialog *MainWindow::createPopupDialog(QString title, QString body,
                                           QPoint xy, bool animate,
                                           int autoCloseDelay, QWidget *parent)
{
    PopupDialog *popupDialog = new PopupDialog(
                title, body , xy ,
                autoCloseDelay, parent
                );

    if (animate)
        popupDialog->animateWindow();

    return popupDialog;
}

void MainWindow::on_actBackup_triggered()
{
    QLocale engLocale(QLocale::English, QLocale::UnitedStates);
    QString dateTime = engLocale.toString(QDateTime::currentDateTime(), "yyyy_MM_dd-hh_mm_ss");
    //...

    QString toSavePath = qApp->applicationDirPath() + "/db/";
    QString backupPath = QFileDialog::getSaveFileName(
                this, trUtf8("تعیین محل تشکیل فایل پشتیبان ") ,
                toSavePath + "bk-" + userInfo.userName + "-" + dateTime ,
                trUtf8("فایل پایگاه داده (*.db);;هر فایلی (*.*)")
                );
    if( backupPath.isEmpty() ) //user cancel kardeh ast
        return ;
    //...

    QFile file(appInfo.databasePath);

    if( !appInfo.databasePath.isEmpty() )
    {
        if( file.copy(backupPath) ) {
            Utility::createPopupDialog(QString(),
                                       QString("فایل پشتیبان با موفقیت ایجاد شد.") ,
                                       QPoint(), true, 1500, this)->show();
        } else {
            qApp->beep();
            Utility::createPopupDialog("خطا",
                                       QString("هنگام ایجاد فایل پشتیبان خطایی به شرح زیر رخ داده است:") +
                                       QString("\n") + file.errorString() ,
                                       QPoint(), true, 4000, this)->show();
        }
    } else {
        qApp->beep();
        Utility::createPopupDialog(QString("خطا"),
                                   "مسیر پایگاه داده اشتباه است",
                                   QPoint(), true, 2000, this)->show();
    }
}

void MainWindow::on_actRestoreBackup_triggered()
{
    QMessageBox msgBox(this);
    msgBox.addButton("بله", QMessageBox::YesRole);
    msgBox.addButton("خیر", QMessageBox::NoRole);
    msgBox.setWindowTitle("تایید اجرای مجدد");
    msgBox.setText( trUtf8("برای بازیابی فایل پشتیبان، نرم افزار نیاز به راه اندازی مجدد دارد") + "\n" +
                    trUtf8("آیا مایل به انجام این کار هستید؟"));
    msgBox.setIcon(QMessageBox::Information);

    if (msgBox.exec() == QMessageBox::RejectRole)
        return ;

    //...
    QString backupPath = QFileDialog::getOpenFileName(
                this, trUtf8("انتخاب فایل پشتیبان") ,
                qApp->applicationDirPath() ,
                trUtf8("فایل پایگاه داده (*.db);;هر فایلی (*.*)")
                );
    if( backupPath.isEmpty() ) // user cancel kardeh ast
        return ;
    //...

    if ( appInfo.databasePath.trimmed() == backupPath.trimmed())
    {
        qApp->beep();
        Utility::createPopupDialog(
                    QString("نیازی به بازیابی نسخه پشتیبان نیست!"),
                    QString("پایگاه داده‌ای که در برنامه اجرا شده است با نسخه بازیابی شما یکسان می‌باشد.") ,
                    QPoint(), true, 6000, this)->show();
        return;
    }

    //...

    appInfo.databasePath = backupPath;

    Utility::saveDatabasePath();

    on_actReenter_triggered();
}

void MainWindow::on_actReenter_triggered()
{
    this->close();   //   writeSettings();
    qApp->quit();
    QProcess::startDetached( qApp->arguments()[0], qApp->arguments() );
}

void MainWindow::on_actQuit_triggered()
{
    qApp->quit();
}

void MainWindow::on_actHelp_triggered()
{
    QString helpPath = qApp->applicationDirPath() + "/help/index.html" ;
    QDesktopServices::openUrl(QUrl("file:///" + helpPath ,QUrl::TolerantMode));
}

void MainWindow::on_ledGoToRecordN_editingFinished()
{
    ui->ledGoToRecordN->setVisible(false);
}

void MainWindow::on_actGoToRecordN_triggered()
{
    int rowCount = ui->tableBooks->rowCount();
    if (rowCount > 0)
    {
        if (ui->stackedWidget->currentWidget() != ui->pageBooks )
            switchBetweenStackedWidgets(ui->pageBooks,
                                        ui->stackedWidget );
        //...
        ui->tabWidgetBooks->setCurrentWidget(ui->tabFormView);

        ui->ledGoToRecordN->clear();
        ui->ledGoToRecordN->setVisible(true);
        ui->ledGoToRecordN->setFocus();
    } else {
        qApp->beep();
        Utility::createPopupDialog(QString("  لیست کتاب خالیست!  ") ,
                                   QString(), QPoint(), true, 2000, this)->show();
    }
}

void MainWindow::on_ledGoToRecordN_returnPressed()
{
    bool isNum = false;
    int recordToGo = ui->ledGoToRecordN->text().toInt(&isNum);

    if (!isNum)
        return;
    //...

    int rowCount = ui->tableBooks->rowCount();

    if( !rowCount )
    {
        qApp->beep();
        Utility::createPopupDialog(QString("  لیست کتاب‌ها خالی می‌باشد!  ") ,
                                   QString(), QPoint(), true, 2000, this)->show();
        ui->ledGoToRecordN->setVisible(false);
        return;
    }

    if ( rowCount >= recordToGo )
    {
        ui->tableBooks->setCurrentCell(recordToGo-1 , 0);
        fillFormFromTable();
        ui->ledGoToRecordN->setVisible(false);
    } else {
        qApp->beep();
        Utility::createPopupDialog(QString("  عدد وارد شده خارج از محدوده است.  ") ,
                                   QString(), QPoint(), true, 2500, this)->show();
        //...
        ui->ledGoToRecordN->setVisible(true);
        ui->ledGoToRecordN->setFocus();
    }
}

void MainWindow::on_tabWidgetBooks_currentChanged(int index)
{
    switch (index) {
    case TableView:
        if (ui->btnCancelInsertOrUpdate->isEnabled())
            on_btnCancelInsertOrUpdate_clicked();
        break;
    case FormView:
        if ( !ui->btnCancelInsertOrUpdate->isEnabled() ) {
            fillFormFromTable();
        } else {
            //ui->pageNavigationMode->setEnabled(false);
            //ui->actNavigationMode->setEnabled(false);
        }
        break;
    }
}

void MainWindow::updateLblRecordStatistics()
{
    int currentRecord = ui->tableBooks->currentRow();
    QString str1 = QString::number( currentRecord < 0 ? 0 : currentRecord+1 ) ;
    QString str2 = QString::number( ui->tableBooks->rowCount() );

    ui->lblRecordStatistics->setText( str1 + trUtf8(" از ") + str2 );
}

void MainWindow::resetFormView()
{
    clearFormViewsLineEdits();
}

void MainWindow::clearFormViewsLineEdits()
{
    ui->ledBookRegisterNumber->clear();
    ui->ledBookTitle->clear();
    ui->ledBookWriter->clear();
    ui->ledBookTranslator->clear();
    ui->ledBookPub->clear();
    ui->ledBookTopic->clear();
}

bool MainWindow::isFormViewsLineEditsEmpty(bool markLineEditIfIsEmpty)
{
    bool isLineEditsEmpty = false;

    static QString defaultStyleSheet = ui->ledBookRegisterNumber->styleSheet();
    QString styleSheetNotOk = "border: 2px solid #D47D7E";    // Red

    //...
    setFormViewsLineEditsStylesheet(defaultStyleSheet);
    //...

    QList<QLineEdit *> lineEdits ;
    lineEdits << ui->ledBookRegisterNumber << ui->ledBookTitle <<
                 ui->ledBookWriter << ui->ledBookTranslator <<
                 ui->ledBookPub << ui->ledBookTopic ;

    foreach (QLineEdit *lineEdit, lineEdits)
        if (lineEdit->text().trimmed().isEmpty()) {
            if( markLineEditIfIsEmpty )
                lineEdit->setStyleSheet(styleSheetNotOk);
            isLineEditsEmpty = true;
        }

    return isLineEditsEmpty;
}

void MainWindow::setFormViewsLineEditsStylesheet(const QString &s)
{  
    ui->ledBookTitle->setStyleSheet(s);
    ui->ledBookWriter->setStyleSheet(s);
    ui->ledBookTranslator->setStyleSheet(s);
    ui->ledBookPub->setStyleSheet(s);
    ui->ledBookTopic->setStyleSheet(s);
    ui->ledBookRegisterNumber->setStyleSheet(s);
}

void MainWindow::on_btnNext_clicked()
{
    RETURN_IF_TABLE_BOOKS_IS_EMPTY;

    const int currRow = ui->tableBooks->currentRow();
    if (currRow < _rowCount-1 )
    {
        ui->tableBooks->setCurrentCell( currRow + 1, 0 );
        fillFormFromTable();
    }
}

void MainWindow::on_btnPrev_clicked()
{
    RETURN_IF_TABLE_BOOKS_IS_EMPTY;

    const int currRow = ui->tableBooks->currentRow();
    if (currRow > 0)
    {
        ui->tableBooks->setCurrentCell( currRow - 1, 0 );
        fillFormFromTable();
    }
}

void MainWindow::on_btnLast_clicked()
{
    RETURN_IF_TABLE_BOOKS_IS_EMPTY;

    ui->tableBooks->setCurrentCell( _rowCount-1, 0 );
    fillFormFromTable();
}

void MainWindow::on_btnFirst_clicked()
{
    RETURN_IF_TABLE_BOOKS_IS_EMPTY;

    ui->tableBooks->setCurrentCell(0, 0);
    fillFormFromTable();
}

void MainWindow::fillFormFromTable()
{
    resetFormView();

    const int currRow = ui->tableBooks->currentRow();
    if ( currRow == -1 )   // اگر جدول خالی باشد و یا انتخاب نشده باشد
    {      
        ui->lblRecordStatistics->setText(" ");

        ui->pageNavigationMode->setEnabled(false);
        return;
    }

    ui->pageNavigationMode->setEnabled(true);

    QTableWidgetItem *itm;
    for( int col=0; col < DB_BOOKS_COLUMN_COUNT; col++ )
    {
        itm = ui->tableBooks->item(currRow, col);
        if ( itm )
        {
            switch (col) {
            case BookRegisterNumber:
                ui->ledBookRegisterNumber->setText(itm->text());
                break;
            case BookTitle:
                ui->ledBookTitle->setText(itm->text());
                break;
            case BookWriter:
                ui->ledBookWriter->setText(itm->text());
                break;
            case BookTranslator:
                ui->ledBookTranslator->setText(itm->text());
                break;
            case BookPub:
                ui->ledBookPub->setText(itm->text());
                break;
            case BookTopic:
                ui->ledBookTopic->setText(itm->text());
                break;
            }
        }
    }

    //...
    updateLblRecordStatistics();
}

void MainWindow::switchBetweenStackedWidgets(QWidget *targetPage,
                                             QStackedWidget *stackedWidget)
{
    stackedWidget->setCurrentWidget(targetPage);

    const QPoint posEnd = targetPage->pos();
    QPoint posStart = posEnd;

    posStart.setX(-50);
    //posStart.setY(0);

    QPropertyAnimation *animation = new QPropertyAnimation(targetPage, "pos");

    animation->setDuration(200);
    animation->setStartValue(posStart);
    animation->setEndValue(posEnd);
    animation->setEasingCurve(QEasingCurve::OutElastic);

    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void MainWindow::setupTableBooks()
{
    ui->tableBooks->clearContents();
    ui->tableBooks->setRowCount(0) ;
    //...
    qryTableBooks->exec("SELECT * FROM table_books;");

    int row = 0;

    int numOfRecord = getNumberOfRecord("table_books", "book_register_number");
    ui->tableBooks->setRowCount( numOfRecord );

    while( qryTableBooks->next() )
    {
        ui->tableBooks->setItem(row, BookRegisterNumber,
                                new QTableWidgetItem(qryTableBooks->value(BookRegisterNumber).toString()));
        ui->tableBooks->setItem(row, BookTitle,
                                new QTableWidgetItem(qryTableBooks->value(BookTitle).toString()));
        ui->tableBooks->setItem(row, BookWriter,
                                new QTableWidgetItem(qryTableBooks->value(BookWriter).toString()));
        ui->tableBooks->setItem(row, BookTranslator,
                                new QTableWidgetItem(qryTableBooks->value(BookTranslator).toString()));
        ui->tableBooks->setItem(row, BookPub,
                                new QTableWidgetItem(qryTableBooks->value(BookPub).toString()));
        ui->tableBooks->setItem(row, BookTopic,
                                new QTableWidgetItem(qryTableBooks->value(BookTopic).toString()));
        row++;
    }
    //...
    //ui->tableBooks->setCurrentCell(0,0);
    ui->tableBooks->resizeColumnsToContents();
}

void MainWindow::on_actBooksManagement_triggered()
{
    ui->actBooksManagement->setChecked(true);
    //...
    if( ui->stackedWidget->currentWidget() != ui->pageBooks )
        switchBetweenStackedWidgets( ui->pageBooks, ui->stackedWidget );
    //...
    ui->tabWidgetBooks->setCurrentWidget(ui->tabFormView);
    //...
    if( ui->stackedWidgetDbMode->currentWidget() != ui->pageEditMode )
        switchBetweenStackedWidgets(ui->pageEditMode, ui->stackedWidgetDbMode);
}

void MainWindow::on_actNavigationMode_triggered()
{
    ui->actNavigationMode->setChecked(true);
    //...
    if (ui->stackedWidget->currentWidget() != ui->pageBooks )
        switchBetweenStackedWidgets( ui->pageBooks, ui->stackedWidget );
    //...
    ui->tabWidgetBooks->setCurrentWidget(ui->tabFormView);
    //...
    if (ui->stackedWidgetDbMode->currentWidget() != ui->pageNavigationMode )
        switchBetweenStackedWidgets(ui->pageNavigationMode, ui->stackedWidgetDbMode);
}

void MainWindow::setReadonlyFormViewsLineEdits(bool val)
{
    ui->ledBookTitle->setReadOnly(val);
    ui->ledBookWriter->setReadOnly(val);
    ui->ledBookTranslator->setReadOnly(val);
    ui->ledBookPub->setReadOnly(val);
    ui->ledBookTopic->setReadOnly(val);
    ui->ledBookRegisterNumber->setReadOnly(val);

    //...

    //TODO: برای تمیز شدن کدها بهتر است بخش زیر در توابع دیگری نوشته شوند
    ui->actNavigationMode->setEnabled(val);

    ui->actGoToRecordN->setEnabled(val);
    ui->ledGoToRecordN->setEnabled(val);
    ui->ledGoToRecordN->clear();
    ui->ledGoToRecordN->setVisible(false);


    ui->actInsert->setEnabled(val);
    ui->actRemove->setEnabled(val);
    ui->actUpdate->setEnabled(val);

    //...
}

void MainWindow::on_actInsert_triggered()
{
    ui->tabWidgetBooks->setCurrentWidget(ui->tabFormView);
    on_actBooksManagement_triggered();
    on_btnInsertAndOk_clicked();
}

void MainWindow::on_btnInsertAndOk_clicked()
{
    if( ui->btnRemove->isEnabled() )  //amadeh shodan baraye gereftane vorudi az user
    {
        resetFormView();

        setReadonlyFormViewsLineEdits(false);
        setFormViewsLineEditsStylesheet("border: 2px solid lightblue");

        ui->ledBookRegisterNumber->setFocus();

        ui->btnInsertAndOk->setIcon(QIcon(":/ok.png"));
        ui->btnCancelInsertOrUpdate->setEnabled( true );
        ui->btnRemove->setEnabled( false );
        ui->btnUpdateAndOk->setEnabled( false );
    }
    else    //    کاربر ورودی را تایید کرد
    {
        if( isFormViewsLineEditsEmpty() )
        {
            qApp->beep();
            Utility::createPopupDialog(QString("مشخصات کتاب به درستی وارد نشده است.") +
                                       QString("\n") +
                                       QString("لطفا موارد مشخص شده را تکمیل نمایید."),
                                       QString(), QPoint(), true, 3000, this)->show();
            return;
        }

        if( isRegisterNumberExist(ui->ledBookRegisterNumber->text().trimmed()) ) {
            qApp->beep();
            Utility::createPopupDialog(QString("شماره ثبت تکراری می‌باشد.") +
                                       QString("\n") +
                                       QString("در صورت نیاز می‌توانید از گزینه های ویرایش/حذف استفاده نمایید."),
                                       QString(), QPoint(), true, 4000, this)->show();
            ui->ledBookRegisterNumber->selectAll();
            ui->ledBookRegisterNumber->setFocus();
            return;
        }

        QSqlQuery qryInsert(QString(), appInfo.db);
        QString   strQueryInsert = "INSERT INTO table_books "
                                   "VALUES('%1', '%2', '%3', '%4', '%5', '%6');";
        strQueryInsert = strQueryInsert.arg(
                    ui->ledBookRegisterNumber->text(),
                    ui->ledBookTitle->text(),
                    ui->ledBookWriter->text(),
                    ui->ledBookTranslator->text(),
                    ui->ledBookPub->text(),
                    ui->ledBookTopic->text()
                    );
        if( !qryInsert.exec(strQueryInsert) )
        {
            qApp->beep();
            Utility::createPopupDialog(QString("خطا"),
                                       QString("در هنگام افزودن کتاب خطایی به شرح زیر رخ داده است:") +
                                       QString("\n") +
                                       qryInsert.lastError().text(),
                                       QPoint(), true, 5000, this)->show();
            //...
            ui->ledBookRegisterNumber->setFocus();
            return;
        }
        //...
        setFormViewsLineEditsStylesheet( QString() );
        //...
        setupTableBooks();
        on_btnLast_clicked();
        //...
        ui->btnInsertAndOk->setIcon(QIcon(":/book-add.png"));
        ui->btnCancelInsertOrUpdate->setEnabled( false );
        ui->btnRemove->setEnabled( true );
        ui->btnUpdateAndOk->setEnabled( true );

        setReadonlyFormViewsLineEdits();
        //...
        Utility::createPopupDialog(QString(),
                                   QString(" کتاب با موفقیت به لیست اضافه گردید. "),
                                   QPoint(), true, 1500, this)->show();
    }
}

void MainWindow::on_btnCancelInsertOrUpdate_clicked()
{
    if (ui->btnInsertAndOk->isEnabled())
    {
        ui->btnInsertAndOk->setIcon(QIcon(":/book-add.png"));
        ui->btnCancelInsertOrUpdate->setEnabled( false );
        ui->btnRemove->setEnabled( true );
        ui->btnUpdateAndOk->setEnabled( true );
    }
    else if (ui->btnUpdateAndOk->isEnabled())
    {
        m_currKeyField.clear();

        ui->btnInsertAndOk->setEnabled(true);
        ui->btnCancelInsertOrUpdate->setEnabled( false );
        ui->btnRemove->setEnabled( true );
        ui->btnUpdateAndOk->setIcon(QIcon(":/book-edit.png"));
    }
    //...

    setReadonlyFormViewsLineEdits();

    fillFormFromTable();

    setFormViewsLineEditsStylesheet( QString() );
}

void MainWindow::on_actRemove_triggered()
{
    on_btnRemove_clicked();
}

void MainWindow::on_btnRemove_clicked()
{
    int rowCount = ui->tableBooks->rowCount();
    //...

    //int currentRow = qryTableBooks->at();
    int currentRow = ui->tableBooks->currentRow();
    if( currentRow < 0 || rowCount == 0)
    {
        qApp->beep();
        Utility::createPopupDialog(QString("کتابی ای برای حذف کردن وجود ندارد!"),
                                   QString(), QPoint(), true, 2000, this)->show();
        return;
    }

    //...

    QString strMessage = "آیا مایل به حذف کتاب با مشخصات زیر هستید؟";
    strMessage += "\n\n";

    QString bookTitle= ui->tableBooks->item(currentRow, BookTitle)->text();
    strMessage += "نام کتاب: ";
    strMessage += bookTitle;
    strMessage += "\n";

    QString bookRegisterNumber= ui->tableBooks->item(currentRow, BookRegisterNumber)->text();
    strMessage += "شماره ثبت کتاب: ";
    strMessage += bookRegisterNumber;
    strMessage += "\n";

    QMessageBox msgBox(this);
    msgBox.addButton("بله", QMessageBox::YesRole);
    msgBox.addButton("خیر", QMessageBox::NoRole);
    msgBox.setWindowTitle("تایید حذف");
    msgBox.setText(strMessage);
    msgBox.setIcon(QMessageBox::Warning);

    if (msgBox.exec() == QMessageBox::RejectRole)
        return ;

    //...

    QString qryString =
            QString("DELETE FROM table_books WHERE book_register_number=%1 ;").arg(bookRegisterNumber);

    QSqlQuery qryDelete(QString() , appInfo.db);
    if( !qryDelete.exec(qryString) )
    {
        qApp->beep();
        Utility::createPopupDialog(QString("خطا") ,
                                   QString("در هنگام حذف کتاب خطایی به شرح زیر رخ داده است:") +
                                   QString("\n") +
                                   qryDelete.lastError().text() ,
                                   QPoint(), true , 2500, this)->show();
    } else {
        rowCount--;
        setupTableBooks();

        //...
        int rowToSelect = 0;
        //...

        if (rowCount == 0) {  // لیست خالی شده است
            rowToSelect = 0;
            ui->tableBooks->setCurrentCell(rowToSelect, 0);
            fillFormFromTable();

            qApp->beep();
            Utility::createPopupDialog( QString() , QString("لیست خالی شد!") ,
                                        QPoint(),true , 1500, this )->show();
            return;
        }

        if( rowCount == 1 || currentRow == 0)
            rowToSelect = 0;
        else
            rowToSelect = currentRow == rowCount ?  currentRow - 1 : currentRow ;
        //...

        ui->tableBooks->setCurrentCell(rowToSelect, 0);
        fillFormFromTable();
    }
}

void MainWindow::on_actUpdate_triggered()
{
    ui->tabWidgetBooks->setCurrentWidget(ui->tabFormView);
    on_actBooksManagement_triggered();
    on_btnUpdateAndOk_clicked();
}

void MainWindow::on_btnUpdateAndOk_clicked()
{    
    int currRow = ui->tableBooks->currentRow();
    if( currRow < 0 )
    {
        qApp->beep();
        Utility::createPopupDialog(QString("کتابی برای ویرایش وجود ندارد"),
                                   QString(), QPoint(), true, 2500, this)->show();
        return;
    }
    //...

    if( ui->btnRemove->isEnabled() )  //    آماده شدن برای آپدیت
    {
        setReadonlyFormViewsLineEdits(false);
        ui->ledBookRegisterNumber->setReadOnly(true);

        setFormViewsLineEditsStylesheet("border: 2px solid lightblue");
        ui->ledBookRegisterNumber->setStyleSheet(QString());

        ui->ledBookTitle->setFocus();

        ui->btnInsertAndOk->setEnabled( false );
        ui->btnCancelInsertOrUpdate->setEnabled( true );
        ui->btnRemove->setEnabled( false );
        ui->btnUpdateAndOk->setIcon(QIcon(":/ok.png"));

        m_currKeyField = ui->tableBooks->item(currRow,  BookRegisterNumber)->text().trimmed();
    }
    else   // کاربر آپدیت را تایید کرد
    {
        if( isFormViewsLineEditsEmpty() )
        {
            qApp->beep();
            Utility::createPopupDialog(QString("مشخصات کتاب به درستی وارد نشده است.") +
                                       QString("\n") +
                                       QString("لطفا موارد مشخص شده را تکمیل نمایید."),
                                       QString(), QPoint(), true, 3000, this)->show();
            return;
        }

        //...

        QString qryString;
        QSqlQuery qry(QString(), appInfo.db);

        qryString = "UPDATE table_books SET "
                    "book_title = '%1' , book_writer = '%2' , book_translator = '%3' ,"
                    "book_pub = '%4' , book_topic = '%5' WHERE book_register_number=%6 ;";
        qryString = qryString.arg( ui->ledBookTitle->text(),
                                   ui->ledBookWriter->text(),
                                   ui->ledBookTranslator->text(),
                                   ui->ledBookPub->text(),
                                   ui->ledBookTopic->text(),
                                   m_currKeyField
                                   );

        if( qry.exec(qryString) )
        {
            setupTableBooks();
            ui->tableBooks->setCurrentCell(currRow, 0);
            fillFormFromTable();
            //...
            Utility::createPopupDialog(QString(), QString(" ویرایش انجام شد "),
                                       QPoint(), true, 1500, this)->show();
        } else {
            qApp->beep();
            Utility::createPopupDialog(QString("خطا!"),
                                       QString("در هنگام به روز رسانی خطایی به شرح زیر رخ داده است:") +
                                       QString("\n") +
                                       qry.lastError().text(),
                                       QPoint(), true, 3500, this)->show();
            return ;
        }

        //...
        setFormViewsLineEditsStylesheet( QString() );

        setReadonlyFormViewsLineEdits();
        //...
        ui->btnInsertAndOk->setEnabled(true);
        ui->btnCancelInsertOrUpdate->setEnabled( false );
        ui->btnRemove->setEnabled( true );
        ui->btnUpdateAndOk->setIcon(QIcon(":/book-edit.png"));
    }
}

void MainWindow::on_actCheckVersion_triggered()
{
    CheckVersionDialog dialog(this);
    dialog.exec();
}

void MainWindow::on_actChangeLoginPass_triggered()
{
    ChangePasswordDialog dialog(this);
    dialog.exec();
}
