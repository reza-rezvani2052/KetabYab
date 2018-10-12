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
    QString _dateTime = enLocale.toString(QDateTime::currentDateTime(), "yyyy_MM_dd-hh_mm_ss");

    QString filePath =
            QFileDialog::getSaveFileName( 0,
                                          "تعیین محل تشکیل فایل پایگاه داده " ,
                                          qApp->applicationDirPath() +
                                          "/db/" + _dateTime ,
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
        QMessageBox::critical(0, "خطا",
                              "هنگام ایجاد پایگاه داده یک خطا به شرح زیر رخ داده است:" +
                              QString("\n") + db.lastError().text() ,
                              QMessageBox::Ok);
        return false;
    }

    //...

    QSqlQuery query("DROP TABLE IF EXISTS table_books;", db);
    query.exec("CREATE TABLE table_books (" + QString() +
               "key_field INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL," +
               "book_title TEXT NOT NULL," +
               "book_writer TEXT NOT NULL," +
               "book_translator TEXT," +
               "book_pub TEXT NOT NULL," +
               "book_topic TEXT," +
               "book_register_number INTEGER NOT NULL" +
               ");");

    //...
    query.exec("DROP TABLE IF EXISTS table_users;");
    query.exec("CREATE TABLE table_users (" + QString() +
               "username TEXT NOT NULL," +
               "password TEXT," +
               "is_admin INTEGER NOT NULL," +
               "nickname TEXT," +
               "PRIMARY KEY (username ASC)" +
               ");");
    //...
    query.exec("INSERT INTO table_users VALUES ('guest', null, 0, 'کاربر میهمان');");
    query.exec("INSERT INTO table_users VALUES ('admin', 123456, 1, 'مدیر سیستم');" );
    //...
    query.exec("INSERT INTO sqlite_sequence VALUES ('table_books', 1000);" );


    db.close();

    return true;
}

int getNumberOfRecord( const QSqlDatabase &db, const QString &tableName,
                       const QString &fieldName)
{
    Q_UNUSED(db);

    QSqlQuery qry(
                QString("SELECT count(%1) FROM %2 ").arg(fieldName).arg(tableName) ,
                db);
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
    QSqlQuery qry(QString("SELECT username FROM table_users WHERE username='%1' ;").arg(userName),
                  appInfo.db);
    return qry.next();
}

QStringList allUserNames()
{
    QStringList userNamesList = QStringList();

    QSqlQuery qry("SELECT username FROM table_users  ORDER BY username;", appInfo.db );

    while( qry.next() )
        userNamesList.append( qry.value(0).toString() );

    return userNamesList;
}

// این فعلن عملیاتی نیست-صرفا جهت آموزش هست
QStringList getBookRecord(const QString &registerNumber)
{
    QStringList allFields = QStringList();

    QSqlQuery qry(
                QString("SELECT * FROM table_books WHERE book_register_number='%1' ;").arg(registerNumber)
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

QString getUserNickname(const QString &userName)
{
    QString nickname = QString();
    QSqlQuery qry(QString("SELECT nickname FROM table_users WHERE username='%1' ").arg(userName),
                  appInfo.db);
    if (qry.next())
        nickname = qry.value(0).toString();

    return nickname;
}
