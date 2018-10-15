#include <QMessageBox>
#include <QFileDialog>

#include "dbconnection.h"
#include "popupdialog.h"


DatabaseErrors createConnection(QSqlDatabase &db, const QString &dbPath)
{
    if( !QFile::exists(dbPath) )
        return DatabaseErrors::FileNotFound;

    //...

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName( dbPath );

    if( !db.open() )
        return DatabaseErrors::ConnectionError;

    //...
    // جهت تست کردن سالم بودن فایل پایگاه داده
    QSqlQuery qryTest("SELECT username FROM table_users;" , appInfo.db);
    if( !qryTest.next() )
        return DatabaseErrors::FileIsCorrupted;

    //...
    return DatabaseErrors::NoError;
}

bool createNewDatabase()
{
    QLocale enLocale(QLocale::English, QLocale::UnitedStates);
    QString dateTime = enLocale.toString(QDateTime::currentDateTime(), "yyyy_MM_dd-hh_mm_ss");

    QString filePath =
            QFileDialog::getSaveFileName( 0, "تعیین محل تشکیل فایل پایگاه داده " ,
                                          qApp->applicationDirPath() + "/db/" + dateTime ,
                                          "فایل پایگاه داده (*.db);;هر فایلی (*.*)");
    // کاربر کنسل کرده است
    if( filePath.isEmpty() )
        return false;
    //...

    if( QFile::exists(filePath) )
    {
        QFile file(filePath);
        if (!file.remove())
        {
            //TODO: badan ba PopUpDialog benevisam
            QMessageBox::warning(0 , "خطا",
                                 "برنامه موفق به حذف فایل نشد" + QString("\n") +
                                 file.errorString()  );
            return false;
        }
    }

    appInfo.databasePath = filePath;
    //...

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(filePath);
    if (!db.open())
    {
        //TODO: badan ba PopUpDialog benevisam
        QMessageBox::critical(0, "خطا",
                              "هنگام ایجاد پایگاه داده یک خطا به شرح زیر رخ داده است:" +
                              QString("\n") + db.lastError().text(), QMessageBox::Ok);
        return false;
    }

    //...

    //FIXME: ***** تست نکردم هنوز  *******

    QSqlQuery query("DROP TABLE IF EXISTS \"sqlite_sequence\";", db);
    query.exec("CREATE TABLE \"sqlite_sequence\" (\"name\",\"seq\");");


    query.exec("DROP TABLE IF EXISTS table_books;");
    query.exec("CREATE TABLE table_books (" + QString() +
               "key_field INTEGER NOT NULL," +
               "book_title TEXT NOT NULL," +
               "book_writer TEXT NOT NULL," +
               "book_translator TEXT," +
               "book_pub TEXT NOT NULL," +
               "book_topic TEXT," +
               "book_register_number INTEGER NOT NULL" +
               "PRIMARY KEY (\"key_field\", \"book_register_number\")" +
               ");");

    //...

    query.exec("DROP TABLE IF EXISTS table_users;");
    query.exec("CREATE TABLE table_users (" + QString() +
               "username TEXT NOT NULL," +
               "password TEXT," +
               "passhint TEXT," +
               "is_admin INTEGER NOT NULL," +
               "nickname TEXT," +
               "PRIMARY KEY (username)" +
               ");");
    //...

    //TODO: agar lazem shod bejaye PassHint ebarate monaseb benevisam
    query.exec("INSERT INTO table_users VALUES ('guest', null, null, 0, 'کاربر میهمان');");
    query.exec("INSERT INTO table_users VALUES ('admin', 123456, null ,1, 'مدیر سیستم');" );

    //INSERT INTO "table_users" VALUES ('guest', NULL, 'فاقد کلمه عبور', 0, 'میهمان');
    //INSERT INTO "table_users" VALUES ('admin', 1, NULL, 1, 'مدیر سیستم');

    //...

    query.exec("INSERT INTO sqlite_sequence VALUES ('table_books', 1000);" );

    //----------------------------------------------------------------------------------------------------------
    //NOTE: این بخش صرفا جهت تست میباشد. در انتشار نهایی حذف گردد
    // با توجه به فارسی نویسی، در محیط کیوت کریتور کمی جابجا نشان داده میشود، در اجرا مشکلی پیش نمی‌آید
//    query.exec("INSERT INTO table_books VALUES(1001, 'عنوان کتاب اول', 'نویسنده کتاب اول', 'مترجم کتاب اول', 'ناشر کتاب اول', 'موضوع کتاب اول', 101)");
//    query.exec("INSERT INTO table_books VALUES(1002, 'عنوان کتاب دوم', 'نویسنده کتاب دوم', 'مترجم کتاب دوم', 'ناشر کتاب دوم', 'موضوع کتاب دوم', 102)");
//    query.exec("INSERT INTO table_books VALUES(1003, 'عنوان کتاب سوم', 'نویسنده کتاب سوم', 'مترجم کتاب سوم', 'ناشر کتاب سوم', 'موضوع کتاب سوم', 103)");
//    query.exec("INSERT INTO table_books VALUES(1004, 'عنوان کتاب چهارم', 'نویسنده کتاب چهارم', 'مترجم کتاب چهارم', 'ناشر کتاب چهارم', 'موضوع کتاب چهارم', 104)");
//    query.exec("INSERT INTO table_books VALUES(1005, 'عنوان کتاب پنجم', 'نویسنده کتاب پنجم', 'مترجم کتاب پنجم', 'ناشر کتاب پنجم', 'موضوع کتاب پنجم', 105)");

    //----------------------------------------------------------------------------------------------------------

    db.close();

    return true;
}

int getNumberOfRecord( const QString &tableName, const QString &fieldName)
{
    QSqlQuery qry(
                QString("SELECT count(%1) FROM %2 ").arg(fieldName).arg(tableName) ,
                appInfo.db);
    qry.next();

    return qry.value(0).toInt();
}

bool isRegisterNumberExist(const QString &registerId)
{
    QSqlQuery qry(
                QString("SELECT book_register_number FROM table_books WHERE book_register_number='%1' ").
                arg(registerId), appInfo.db );

    return qry.next();
}

bool isUserExist(const QString &userName)
{
    QSqlQuery qry(QString("SELECT username FROM table_users WHERE username='%1';").arg(userName),
                  appInfo.db);
    return qry.next();
}

QStringList allUserNames()
{
    QStringList userNamesList = QStringList();

    //TODO: ???? QSqlQuery qry("SELECT username FROM table_users  ORDER BY username;", appInfo.db );
    QSqlQuery qry("SELECT username FROM table_users;", appInfo.db );

    while( qry.next() )
        userNamesList.append( qry.value(0).toString() );

    return userNamesList;
}

QStringList getTableUsersRecord(const QString &userName)
{
    QStringList allFields = QStringList();

    QSqlQuery qry(
                QString("SELECT * FROM table_users WHERE username='%1' ;").arg(userName)
                , appInfo.db );
    if (qry.next())
    {
        allFields << qry.value(UserName).toString();
        allFields << qry.value(Password).toString();
        allFields << qry.value(PassHint).toString();
        allFields << qry.value(IsAdmin).toString();
        allFields << qry.value(Nickname).toString();
    }

    return allFields;
}

// این فعلن عملیاتی نیست-صرفا جهت آموزش هست
QStringList getTableBooksRecord(const QString &registerNumber)
{
    QStringList allFields = QStringList();

    QSqlQuery qry( QString("SELECT * FROM table_books WHERE book_register_number='%1' ;").arg(registerNumber)
                   , appInfo.db );
    if (qry.next())
    {
        allFields << qry.value(KeyField).toString();
        allFields << qry.value(BookTitle).toString();
        allFields << qry.value(BookWriter).toString();
        allFields << qry.value(BookTranslator).toString();
        allFields << qry.value(BookPub).toString();
        allFields << qry.value(BookTopic).toString();
        allFields << qry.value(BookRegisterNumber).toString();
    }

    return allFields;
}

QString getUserPassword(const QString &userName)
{
    QString userPassword = QString();
    QSqlQuery qry(QString("SELECT password FROM table_users WHERE username='%1' ").arg(userName),
                  appInfo.db);
    if (qry.next())
        userPassword = qry.value(0).toString();

    return userPassword;
}

QString getUserPassHint(const QString &userName)
{
    QString passHint = QString();
    QSqlQuery qry(QString("SELECT passhint FROM table_users WHERE username='%1' ").arg(userName),
                  appInfo.db);
    if (qry.next())
        passHint = qry.value(0).toString();

    return passHint;
}

QString getUserNickname(const QString &userName)
{
    QString nickname = QString();
    QSqlQuery qry(QString("SELECT nickname FROM table_users WHERE username='%1' ").arg(userName),
                  appInfo.db);
    if (qry.next())
        nickname = qry.value(0).toString();

    return nickname;
}

bool setUsersPass(QString &pass, QString passHint)
{
    QStringList allFields = getTableUsersRecord(userInfo.userName);
    if (allFields.isEmpty())
        return false;


    QString userName = allFields.at(UserName);
    QString password = pass; // allFields.at(Password);
    //QString _passHint = passHint;
    QString isAdmin  = allFields.at(IsAdmin);
    QString nickName = allFields.at(Nickname);
    //qDebug() << userName << password << passHint << isAdmin << nickName;

    QString qryString;
    QSqlQuery qry(";", appInfo.db);

    qryString = "UPDATE table_users SET "
                "username = '%1' ,password = '%2' , passhint = '%3' "
                ",is_admin = '%4' ,nickname = '%5' WHERE username='%6' ;";
    qryString = qryString.arg( userName, password, passHint,isAdmin,
                               nickName, userInfo.userName);

    //...
    userInfo.passHint = passHint;
    //...

    return qry.exec(qryString) ;
}
