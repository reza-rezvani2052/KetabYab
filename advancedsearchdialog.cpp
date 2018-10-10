#include "advancedsearchdialog.h"
#include "ui_advancedsearchdialog.h"

#include <QMessageBox>
#include <QListWidgetItem>

#include "utility.h"
#include "popupdialog.h"
#include "dbconnection.h"
#include "advancedsearchselectedcolumnsdialog.h"

//---------------------------------------------------------------------------------

extern UserInfo userInfo;

//تعداد تب های شرط
#define NumberOfTabs 6

//---------------------------------------------------------------------------------

AdvancedSearchDialog::AdvancedSearchDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdvancedSearchDialog)
{
    ui->setupUi(this);
    //...
    ui->lblWarning->setVisible(false);
    //...
    if ( !userInfo.isAdmin )
    {
        ui->groupBoxQuery->setVisible(false);

        QSize size = this->size();
        adjustSize();
        resize(size.width(), this->size().height() );
    }
    //...
    initializeTabWidgetConditions();
    //...

/*
    QListWidgetItem *item = 0;

    item = new QListWidgetItem("عنوان کتاب");
    item->setCheckState(Qt::Unchecked);
    m_hashItems.insert(item, "book_title");

    item = new QListWidgetItem("پدید آور");
    item->setCheckState(Qt::Checked);
    m_hashItems.insert(item, "book_writer");

    item = new QListWidgetItem("مترجم");
    item->setCheckState(Qt::Checked);
    m_hashItems.insert(item, "book_translator");

    item = new QListWidgetItem("انتشارات");
    item->setCheckState(Qt::Unchecked);
    m_hashItems.insert(item, "book_pub");

    item = new QListWidgetItem("موضوع");
    item->setCheckState(Qt::Unchecked);
    m_hashItems.insert(item, "book_topic");

    item = new QListWidgetItem("شماره ثبت");
    item->setCheckState(Qt::Unchecked);
    m_hashItems.insert(item, "book_register_number");
*/

    //...

    m_dialog = new AdvancedSearchSelectedColumnsDialog(this);

    updateCmbOrderBy();

    //...
    const int tabCount = ui->tabWidgetConditions->count();
    for(int i=1; i< tabCount; i++)
        ui->tabWidgetConditions->removeTab(1);
    //...
}

AdvancedSearchDialog::~AdvancedSearchDialog()
{
    delete ui;
}

void AdvancedSearchDialog::initializeTabWidgetConditions()
{
    QStringList listRightOperand;
    listRightOperand.append( "عنوان کتاب" );
    listRightOperand.append( "پدید آور" );
    listRightOperand.append( "مترجم" );
    listRightOperand.append( "انتشارات" );
    listRightOperand.append( "موضوع" );
    listRightOperand.append( "شماره ثبت" );

    ui->cmbRightOperand1->addItems( listRightOperand );
    ui->cmbRightOperand2->addItems( listRightOperand );
    ui->cmbRightOperand3->addItems( listRightOperand );
    ui->cmbRightOperand4->addItems( listRightOperand );
    ui->cmbRightOperand5->addItems( listRightOperand );
    ui->cmbRightOperand6->addItems( listRightOperand );
    //...

    QStringList listOperator;
    listOperator << "=" << "<>" << "<" << "<=" << ">" << ">=" <<
                    "LIKE" << "NOT LIKE" ;
    ui->cmbOperator1->addItems( listOperator );
    ui->cmbOperator2->addItems( listOperator );
    ui->cmbOperator3->addItems( listOperator );
    ui->cmbOperator4->addItems( listOperator );
    ui->cmbOperator5->addItems( listOperator );
    ui->cmbOperator6->addItems( listOperator );
    //...
    QStringList listLogicOperator;
    listLogicOperator << trUtf8("پایان شرط") << "AND" << "OR" <<
                         "AND NOT" << "OR NOT" ;
    ui->cmbLogicCond1->addItems( listLogicOperator );
    ui->cmbLogicCond2->addItems( listLogicOperator );
    ui->cmbLogicCond3->addItems( listLogicOperator );
    ui->cmbLogicCond4->addItems( listLogicOperator );
    ui->cmbLogicCond5->addItems( listLogicOperator );
    ui->cmbLogicCond6->addItems( listLogicOperator );
    //...
    //QStringList listOrderBy( listRightOperand );
    //listOrderBy.append( "عدم مرتب سازی" );
    //ui->cmbOrderBy->addItems( listOrderBy );
    ui->cmbOrderBy->addItem("عدم مرتب سازی");
}

void AdvancedSearchDialog::on_btnFieldChoise_clicked()
{    
    QPoint xy = ui->btnFieldChoise->mapToGlobal(QPoint(0,0));

    m_dialog->show();
    m_dialog->move( m_dialog->x(), xy.y() + ui->btnFieldChoise->height() + 1 );


    //TODO: *************************
    if ( m_dialog->exec() == QDialog::Accepted ) {
        updateCmbOrderBy();

        //qDebug() << "Accepted";
    } else {
        //qDebug() << "--------";
    }
}

//FIXME: *****
void AdvancedSearchDialog::updateCmbOrderBy()
{
    ui->cmbOrderBy->clear();
    ui->cmbOrderBy->addItem("عدم مرتب سازی");
    //...
    ui->cmbOrderBy->addItems( m_dialog->getSelectedColumns() );
}

QString AdvancedSearchDialog::getAdvancedQueryString()
{
    return ui->plainTextEdit->toPlainText();
}

//FIXME: *****
QStringList AdvancedSearchDialog::getColumnNames()
{
    // عناوین ستونهایی که در نتیجه جستجو پیشرفته باید نمایش داده شود

    QStringList list;


    return list;

}

void AdvancedSearchDialog::on_btnCancel_clicked()
{
    reject();
}

void AdvancedSearchDialog::on_btnOk_clicked()
{
    accept();
}

void AdvancedSearchDialog::on_groupBoxQuery_clicked(bool checked)
{
    static int n = -2;
    if( checked ) {
        if( n ) {
            n++;
            QMessageBox::StandardButton ret =
                    QMessageBox::warning(this, " ",
                                         trUtf8("آیا شما قصد نوشتن کوئری خود به صورت دستی را دارید؟"),
                                         QMessageBox::Yes | QMessageBox::No);
            if( ret == QMessageBox::No ) {
                ui->groupBoxQuery->setChecked(false);
                return;
            }
        }
        ui->btnOk->setEnabled(true);
    } else {
        ui->btnOk->setEnabled(false);
    }
}

void AdvancedSearchDialog::on_btnAddCond_clicked()
{
    if( !ui->tabWidgetConditions->isVisible() )
        ui->tabWidgetConditions->setVisible(true);

    int tabCount = ui->tabWidgetConditions->count();
    if( ui->tabWidgetConditions->indexOf(ui->tab1)== -1 ) {
        ui->tabWidgetConditions->insertTab(tabCount, ui->tab1, "" );
        ui->tabWidgetConditions->setCurrentWidget(ui->tab1);
    }else if( ui->tabWidgetConditions->indexOf(ui->tab2)== -1 ){
        ui->tabWidgetConditions->insertTab(tabCount, ui->tab2, "" );
        ui->tabWidgetConditions->setCurrentWidget(ui->tab2);
    }else if( ui->tabWidgetConditions->indexOf(ui->tab3)== -1 ){
        ui->tabWidgetConditions->insertTab(tabCount, ui->tab3, "" );
        ui->tabWidgetConditions->setCurrentWidget(ui->tab3);
    }else if( ui->tabWidgetConditions->indexOf(ui->tab4)== -1 ){
        ui->tabWidgetConditions->insertTab(tabCount, ui->tab4, "" );
        ui->tabWidgetConditions->setCurrentWidget(ui->tab4);
    }else if( ui->tabWidgetConditions->indexOf(ui->tab5)== -1 ){
        ui->tabWidgetConditions->insertTab(tabCount, ui->tab5, "" );
        ui->tabWidgetConditions->setCurrentWidget(ui->tab5);
    }else if( ui->tabWidgetConditions->indexOf(ui->tab6)== -1 ){
        ui->tabWidgetConditions->insertTab(tabCount, ui->tab6, "" );
        ui->tabWidgetConditions->setCurrentWidget(ui->tab6);
    }else{
        qApp->beep();
        createPopupDialog(QString(),
                          trUtf8("در حال حاضر فقط امکان ایجاد %1 شرط وجود دارد").arg(NumberOfTabs),
                          QPoint(), true, 2500, this)->show();
    }
}

void AdvancedSearchDialog::on_tabWidgetConditions_tabCloseRequested(int index)
{
    ui->tabWidgetConditions->removeTab( index );

    if( ui->tabWidgetConditions->count() == 0 ) {
        ui->groupBoxCond->setChecked(false);
        ui->tabWidgetConditions->setVisible(false);
        //...
        ui->lblWarning->setVisible(false);
    }
}

PopupDialog *AdvancedSearchDialog::createPopupDialog(QString title, QString body,
                                                     QPoint xy, bool animate, int autoCloseDelay,
                                                     QWidget *parent)
{
    PopupDialog *popupDialog = new PopupDialog(
                title, body , xy , autoCloseDelay, parent
                );

    if (animate)
        popupDialog->animateWindow();

    //popupDialog->show();
    return popupDialog;
}

void AdvancedSearchDialog::on_btnCreateQuery_clicked()
{
    ui->lblWarning->setVisible(false);
    //...
    QString qryString1 = QString("SELECT ");
    //...
    QString qryString2 = QString("%1 %2 %3 %4 %5 %6");



    //FIXME: **************************************************************************

    qryString2 = true /*ui->actRegisterNumber->isChecked()*/ ?
                qryString2.arg("book_title") :
                qryString2.arg("");
    qryString2 = true ? qryString2.arg(",book_writer") :
                        qryString2.arg("");
    qryString2 = true ? qryString2.arg(",book_translator") :
                        qryString2.arg("");
    qryString2 = true? qryString2.arg(",book_pub") :
                       qryString2.arg("");
    qryString2 = true ? qryString2.arg(",book_topic") :
                        qryString2.arg("");
    qryString2 = true ? qryString2.arg(",book_register_number") :
                        qryString2.arg("");


    if (getColumnNames().isEmpty())  // بررسی میکنیم که آیا آیتمی انتخاب شده است؟
    {
        qApp->beep();
        PopupDialog *popupDialog = createPopupDialog(
                    QString("هیچ گزینه ای برای جستجو انتخاب نشده است.") ,
                    QString() , QPoint(),true , 2000, this );
        popupDialog->show();
        //...
        Utility util;
        util.delay(popupDialog->getDuration() + 75 );
        //...
        ui->btnFieldChoise->showMenu();
        return ;
    }

    qryString2 = qryString2.simplified();
    if( qryString2.startsWith(",", Qt::CaseInsensitive) ) //agar key_field(KeyField) entekhab nashodeh bashad , avalin character ',' mishavad
        qryString2 = qryString2.right( qryString2.length()-1 );
    qryString2.append(" \n");
    //...
    QString qryString3 = "FROM table_thesis \n";
    //...
    QString qryString4 = QString();
    if( ui->groupBoxCond->isChecked() && ui->tabWidgetConditions->isVisible() ) {
        QString strRet = prepareWhereExpression();
        if( strRet == QString() )  //payam dar method prepareWhereExpression() dadeh mishavad
            return ;
        qryString4 =  "WHERE " + strRet + "\n" ;
    }
    //...

    QString qryString5 = QString();

    //FIXME: *****
    /*QHashIterator<QListWidgetItem *, QString> itr(m_hashItems);
    while (itr.hasNext())
    {
        itr.next();
        if ( itr.key()->text() == ui->cmbOrderBy->currentText() )
            qryString5 = itr.value();
    }*/

    /*
    //TODO: نام فیلدها باید عوض شود
    switch ( ui->cmbOrderBy->currentIndex()+1 ) {
    case RegisterNumber: qryString5 = "ORDER BY register_number ASC";  break;
    case ThesisTitle   : qryString5 = "ORDER BY thesis_title ASC";     break;
    case FieldOfStudy  : qryString5 = "ORDER BY field_of_study ASC";   break;
    case MaestroName   : qryString5 = "ORDER BY maestro_name ASC";     break;
    case StudentName   : qryString5 = "ORDER BY student_name ASC";     break;
    case YearOfWriting : qryString5 = "ORDER BY year_of_writing ASC";  break;
    }
    */

    //...

    ui->plainTextEdit->setPlainText( qryString1 + qryString2 +
                                     qryString3 + qryString4 + qryString5);
    ui->btnOk->setEnabled(true);
}

QString AdvancedSearchDialog::prepareWhereExpression()
{
    QHash<int, QWidget *> hash;
    int index;
    if( (index = ui->tabWidgetConditions->indexOf(ui->tab1))!= -1)
        hash.insert(index, ui->tab1);
    if( (index = ui->tabWidgetConditions->indexOf(ui->tab2))!= -1)
        hash.insert(index, ui->tab2);
    if( (index = ui->tabWidgetConditions->indexOf(ui->tab3))!= -1)
        hash.insert(index, ui->tab3);
    if( (index = ui->tabWidgetConditions->indexOf(ui->tab4))!= -1)
        hash.insert(index, ui->tab4);
    if( (index = ui->tabWidgetConditions->indexOf(ui->tab5))!= -1)
        hash.insert(index, ui->tab5);
    if( (index = ui->tabWidgetConditions->indexOf(ui->tab6))!= -1)
        hash.insert(index, ui->tab6);


    QString strMessage = trUtf8("جعبه متن خالی می باشد. لطفا آن را تکمیل نمایید");
    QStringList listCond = QStringList();
    int lastCondIndex = hash.size()-1 ;

    for( int i=0; i<hash.size(); i++ ) {
        if( hash.value(i)->objectName()== "tab1" ) {
            if( ui->ledLeftOperand1->text().isEmpty() ) {
                qApp->beep();
                createPopupDialog(QString(strMessage), QString(),
                                  QPoint(), true, 2000, this)->show();
                ui->tabWidgetConditions->setCurrentIndex(i);
                ui->ledLeftOperand1->setFocus();
                return QString();
            }
            if( i==lastCondIndex  ) {
                if( (ui->cmbLogicCond1->currentIndex() != END_OF_CONDITION) ) {
                    ui->lblWarning->setVisible(true);
                    ui->tabWidgetConditions->setCurrentIndex(i);
                    ui->cmbLogicCond1->showPopup();
                    return QString();
                }else{
                    listCond.append( composeWhereExpression( ui->cmbRightOperand1, ui->cmbOperator1, ui->ledLeftOperand1, 0 ) );
                }
            }else{
                if( ui->cmbLogicCond1->currentIndex() == END_OF_CONDITION ) {
                    ui->lblWarning->setVisible(true);
                    ui->tabWidgetConditions->setCurrentIndex(i);
                    ui->cmbLogicCond1->showPopup();
                    return QString();
                }else{
                    listCond.append( composeWhereExpression( ui->cmbRightOperand1, ui->cmbOperator1, ui->ledLeftOperand1, ui->cmbLogicCond1 ) );
                }
            }
        }else if( hash.value(i)->objectName() == "tab2" ) {
            if( ui->ledLeftOperand2->text().isEmpty() ) {
                qApp->beep();
                createPopupDialog(QString(strMessage), QString(),
                                  QPoint(), true, 2000, this)->show();
                ui->tabWidgetConditions->setCurrentIndex(i);
                ui->ledLeftOperand2->setFocus();
                return QString();
            }
            if( i==lastCondIndex  ) {
                if( (ui->cmbLogicCond2->currentIndex() != END_OF_CONDITION) ) {
                    ui->lblWarning->setVisible(true);
                    ui->tabWidgetConditions->setCurrentIndex(i);
                    ui->cmbLogicCond2->showPopup();
                    return QString();
                }else{
                    listCond.append( composeWhereExpression( ui->cmbRightOperand2, ui->cmbOperator2, ui->ledLeftOperand2, 0 ) );
                }
            }else{
                if( ui->cmbLogicCond2->currentIndex() == END_OF_CONDITION ) {
                    ui->lblWarning->setVisible(true);
                    ui->tabWidgetConditions->setCurrentIndex(i);
                    ui->cmbLogicCond2->showPopup();
                    return QString();
                }else{
                    listCond.append( composeWhereExpression( ui->cmbRightOperand2, ui->cmbOperator2, ui->ledLeftOperand2, ui->cmbLogicCond2) );
                }
            }
        }else if( hash.value(i)->objectName() == "tab3" ) {
            if( ui->ledLeftOperand3->text().isEmpty() ) {
                qApp->beep();
                createPopupDialog(QString(strMessage), QString(),
                                  QPoint(), true, 2000, this)->show();
                ui->tabWidgetConditions->setCurrentIndex(i);
                ui->ledLeftOperand3->setFocus();
                return QString();
            }
            if( i==lastCondIndex  ) {
                if( (ui->cmbLogicCond3->currentIndex() != END_OF_CONDITION) ) {
                    ui->lblWarning->setVisible(true);
                    ui->tabWidgetConditions->setCurrentIndex(i);
                    ui->cmbLogicCond3->showPopup();
                    return QString();
                }else{
                    listCond.append( composeWhereExpression( ui->cmbRightOperand3, ui->cmbOperator3, ui->ledLeftOperand3, 0 ) );
                }
            }else{
                if( ui->cmbLogicCond3->currentIndex() == END_OF_CONDITION ) {
                    ui->lblWarning->setVisible(true);
                    ui->tabWidgetConditions->setCurrentIndex(i);
                    ui->cmbLogicCond3->showPopup();
                    return QString();
                }else{
                    listCond.append( composeWhereExpression( ui->cmbRightOperand3, ui->cmbOperator3, ui->ledLeftOperand3, ui->cmbLogicCond3 ) );
                }
            }
        }else if( hash.value(i)->objectName() == "tab4" ) {
            if( ui->ledLeftOperand4->text().isEmpty() ) {
                qApp->beep();
                createPopupDialog(QString(strMessage), QString(),
                                  QPoint(), true, 2000, this)->show();
                ui->tabWidgetConditions->setCurrentIndex(i);
                ui->ledLeftOperand4->setFocus();
                return QString();
            }
            if( i==lastCondIndex  ) {
                if( (ui->cmbLogicCond4->currentIndex() != END_OF_CONDITION) ) {
                    ui->lblWarning->setVisible(true);
                    ui->tabWidgetConditions->setCurrentIndex(i);
                    ui->cmbLogicCond4->showPopup();
                    return QString();
                }else{
                    listCond.append( composeWhereExpression( ui->cmbRightOperand4, ui->cmbOperator4, ui->ledLeftOperand4, 0 ) );
                }
            }else{
                if( ui->cmbLogicCond4->currentIndex() == END_OF_CONDITION ) {
                    ui->lblWarning->setVisible(true);
                    ui->tabWidgetConditions->setCurrentIndex(i);
                    ui->cmbLogicCond4->showPopup();
                    return QString();
                }else{
                    listCond.append( composeWhereExpression( ui->cmbRightOperand4, ui->cmbOperator4, ui->ledLeftOperand4, ui->cmbLogicCond4 ) );
                }
            }
        }else if( hash.value(i)->objectName() == "tab5" ) {
            if( ui->ledLeftOperand5->text().isEmpty() ) {
                qApp->beep();
                createPopupDialog(QString(strMessage), QString(),
                                  QPoint(), true, 2000, this)->show();
                ui->tabWidgetConditions->setCurrentIndex(i);
                ui->ledLeftOperand5->setFocus();
                return QString();
            }
            if( i==lastCondIndex  ) {
                if( (ui->cmbLogicCond5->currentIndex() != END_OF_CONDITION) ) {
                    ui->lblWarning->setVisible(true);
                    ui->tabWidgetConditions->setCurrentIndex(i);
                    ui->cmbLogicCond5->showPopup();
                    return QString();
                }else{
                    listCond.append( composeWhereExpression( ui->cmbRightOperand5, ui->cmbOperator5, ui->ledLeftOperand5, 0 ) );
                }
            }else{
                if( ui->cmbLogicCond5->currentIndex() == END_OF_CONDITION ) {
                    ui->lblWarning->setVisible(true);
                    ui->tabWidgetConditions->setCurrentIndex(i);
                    ui->cmbLogicCond5->showPopup();
                    return QString();
                }else{
                    listCond.append( composeWhereExpression( ui->cmbRightOperand5, ui->cmbOperator5, ui->ledLeftOperand5, ui->cmbLogicCond5 ) );
                }
            }
        }else if( hash.value(i)->objectName() == "tab6" ) {
            if( ui->ledLeftOperand6->text().isEmpty() ) {
                qApp->beep();
                createPopupDialog(QString(strMessage), QString(),
                                  QPoint(), true, 2000, this)->show();
                ui->tabWidgetConditions->setCurrentIndex(i);
                ui->ledLeftOperand6->setFocus();
                return QString();
            }
            if( i==lastCondIndex  ) {
                if( (ui->cmbLogicCond6->currentIndex() != END_OF_CONDITION) ) {
                    ui->lblWarning->setVisible(true);
                    ui->tabWidgetConditions->setCurrentIndex(i);
                    ui->cmbLogicCond6->showPopup();
                    return QString();
                }else{
                    listCond.append( composeWhereExpression( ui->cmbRightOperand6, ui->cmbOperator6, ui->ledLeftOperand6, 0 ) );
                }
            }else{
                if( ui->cmbLogicCond6->currentIndex() == END_OF_CONDITION ) {
                    ui->lblWarning->setVisible(true);
                    ui->tabWidgetConditions->setCurrentIndex(i);
                    ui->cmbLogicCond6->showPopup();
                    return QString();
                }else{
                    listCond.append( composeWhereExpression(
                                         ui->cmbRightOperand6,
                                         ui->cmbOperator6,
                                         ui->ledLeftOperand6,
                                         ui->cmbLogicCond6
                                         ));
                }
            }
        }
    } // end of for( int i=0; i<hash.size(); i++ ) ...


    QString qryString = QString() ;  //agar null bargasht yani user kamel por nakardeh
    for(int i=0; i< listCond.count(); i++)
        qryString += listCond[i];
    return qryString;
}

QString AdvancedSearchDialog::composeWhereExpression(QComboBox *cmbRightOperand,
                                                     QComboBox *cmbOperator,
                                                     QLineEdit *ledLeftOperand,
                                                     QComboBox *cmbLogicCond)
{
    QString strRightOperand;
    //
    switch ( cmbRightOperand->currentIndex()+1 ) {
    case BookTitle          : strRightOperand="book_title"; break;
    case BookWriter         : strRightOperand="book_writer";    break;
    case BookTranslator     : strRightOperand="book_translator";  break;
    case BookPub            : strRightOperand="book_pub";    break;
    case BookTopic          : strRightOperand="book_topic";    break;
    case BookRegisterNumber : strRightOperand="book_register_number"; break;
    }

    QString strLogicCond = QString();
    if( cmbLogicCond != 0 )
        if( cmbLogicCond->currentIndex() != END_OF_CONDITION )
            strLogicCond = cmbLogicCond->currentText();


    QString strQuery = QString() ;

    if( cmbOperator->currentIndex()== Like ||
            cmbOperator->currentIndex()== NotLike ) {
        if( cmbLogicCond == 0 ) {  //akharin shart
            strQuery = QString("%1 %2 '\%%3\%' ").arg(strRightOperand, cmbOperator->currentText(), ledLeftOperand->text());
        }else{
            strQuery = QString("%1 %2 '\%%3\%' \n%4 \n").arg(strRightOperand, cmbOperator->currentText(), ledLeftOperand->text(), strLogicCond);
        }

    } else {
        if( cmbLogicCond == 0 ) {
            strQuery = QString("%1 %2 '%3' ").arg(strRightOperand, cmbOperator->currentText(), ledLeftOperand->text());
        }else{
            strQuery = QString("%1 %2 '%3' \n%4 \n").arg(strRightOperand, cmbOperator->currentText(), ledLeftOperand->text(), strLogicCond);
        }
    }

    return strQuery;
}
