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
    // البته در اینجا فقط یک جدول را تست کردم
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

    //TODO: بعدا این را تست کنم
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

    appInfo.databasePath = filePath;

    //...

    QSqlQuery query("DROP TABLE IF EXISTS \"sqlite_sequence\";", db);
    query.exec("CREATE TABLE \"sqlite_sequence\" (\"name\",\"seq\");");

    query.exec("DROP TABLE IF EXISTS table_books;");
    query.exec("CREATE TABLE \"table_books\" (\n" + QString() +
               "  \"book_register_number\" integer NOT NULL,\n" +
               "  \"book_title\" text NOT NULL,\n" +
               "  \"book_writer\" text NOT NULL,\n" +
               "  \"book_translator\" text,\n" +
               "  \"book_pub\" text NOT NULL,\n" +
               "  \"book_topic\" text,\n" +
               "  PRIMARY KEY (\"book_register_number\")\n" +
               ");");
    //...

    query.exec("DROP TABLE IF EXISTS table_users;");
    query.exec( "CREATE TABLE \"table_users\" (\n"  + QString() +
                "  \"username\" TEXT NOT NULL,\n" +
                "  \"password\" TEXT,\n" +
                "  \"passhint\" TEXT,\n" +
                "  \"is_admin\" TEXT,\n" +
                "  \"nickname\" TEXT,\n" +
                "  PRIMARY KEY (\"username\")\n" +
                ");"
                );

    //...

    // پسورد را خالی بگذارم تا در اجرای بعدی گزینه انتخاب پسورد ظاهر شود
    query.exec("INSERT INTO \"table_users\" VALUES ('guest', '', 'فاقد کلمه عبور', 0, 'میهمان');" );
    query.exec("INSERT INTO \"table_users\" VALUES ('admin', '', '', 1, 'مدیر سیستم');");

    //...

    //----------------------------------------------------------------------------------------------------------

    // این بخش صرفا جهت تست میباشد. در انتشار نهایی حذف گردد
    // خط زیر چون انگلیسی نوشته شده مشکل ندارد
    ///query.exec("INSERT INTO \"table_books\" VALUES (101, 'title', 'writer', 'translater', 'pub', 'topic');" );

//    با توجه به فارسی نویسی، در محیط کیوت کریتور کمی جابجا نشان داده میشود و در اجرا مشکل  پیش می‌آید
//    query.exec("INSERT INTO \"table_books\" VALUES('عنوان کتاب اول', 'نویسنده کتاب اول', 'مترجم کتاب اول', 'ناشر کتاب اول', 'موضوع کتاب اول', 101)");
//    query.exec("INSERT INTO table_books VALUES('عنوان کتاب دوم', 'نویسنده کتاب دوم', 'مترجم کتاب دوم', 'ناشر کتاب دوم', 'موضوع کتاب دوم', 102)");
//    query.exec("INSERT INTO table_books VALUES('عنوان کتاب سوم', 'نویسنده کتاب سوم', 'مترجم کتاب سوم', 'ناشر کتاب سوم', 'موضوع کتاب سوم', 103)");
//    query.exec("INSERT INTO table_books VALUES('عنوان کتاب چهارم', 'نویسنده کتاب چهارم', 'مترجم کتاب چهارم', 'ناشر کتاب چهارم', 'موضوع کتاب چهارم', 104)");
//    query.exec("INSERT INTO table_books VALUES('عنوان کتاب پنجم', 'نویسنده کتاب پنجم', 'مترجم کتاب پنجم', 'ناشر کتاب پنجم', 'موضوع کتاب پنجم', 105)");

    //----------------------------------------------------------------------------------------------------------

    db.close();

    return true;
}

int getNumberOfRecord( const QString &tableName, const QString &fieldName)
{
    QSqlQuery qry(QString("SELECT count(%1) FROM %2 ").arg(fieldName).arg(tableName) ,
                  appInfo.db);
    qry.next();

    return qry.value(0).toInt();
}

bool isRegisterNumberExist(const QString &registerId)
{
    QSqlQuery qry(QString("SELECT book_register_number FROM table_books WHERE book_register_number='%1' ").
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

    //برای اینکه اول کاربر میهمان در لیست قرار گیرد از اوردر بای استفاده کردم
    QSqlQuery qry("SELECT username FROM table_users ORDER BY username DESC ;", appInfo.db );

    while( qry.next() )
        userNamesList.append( qry.value(0).toString() );

    return userNamesList;
}

QStringList getTableUsersRecord(const QString &userName)
{    
    QStringList allFields = QStringList();

    QSqlQuery qry( QString("SELECT * FROM table_users WHERE username='%1';").arg(userName)
                   , appInfo.db );
    if (qry.next())
    {
        allFields << qry.value(UserName).toString();
        allFields << qry.value(Password).toString();
        allFields << qry.value(PassHint).toString();
        allFields << qry.value(IsAdmin ).toString();
        allFields << qry.value(Nickname).toString();
    }

    return allFields;
}

// این فعلن عملیاتی نیست
QStringList getTableBooksRecord(const QString &registerNumber)
{
    QStringList allFields = QStringList();

    QSqlQuery qry( QString("SELECT * FROM table_books WHERE book_register_number='%1' ;").arg(registerNumber)
                   , appInfo.db );
    if (qry.next())
    {
        allFields << qry.value(BookRegisterNumber).toString();
        allFields << qry.value(BookTitle).toString();
        allFields << qry.value(BookWriter).toString();
        allFields << qry.value(BookTranslator).toString();
        allFields << qry.value(BookPub).toString();
        allFields << qry.value(BookTopic).toString();

    }

    return allFields;
}

bool isPasswordSet(const QString &userName)
{
    bool hasPassword = false;

    QSqlQuery qry(QString("SELECT password FROM table_users WHERE username='%1' ").arg(userName),
                  appInfo.db);
    if (qry.next())
        if (!qry.value(0).toString().trimmed().isEmpty())
            hasPassword = true;

    return hasPassword;
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
    QStringList allFields = getTableUsersRecord(
                // چون فقط پسورد یوزر ادمین قابل تغییر است میتوان در اینجا مستقیم ادمین را نوشت
                // "admin"
                userInfo.userName
                );

    if (allFields.isEmpty())
        return false;

    QString userName = allFields.at(UserName);
    QString password = pass;
    //passHint = OOOOO;
    QString isAdmin  = allFields.at(IsAdmin);
    QString nickName = allFields.at(Nickname);

    QString qryString;
    QSqlQuery qry(";", appInfo.db);

    qryString = "UPDATE table_users SET "
                "username = '%1' ,password = '%2' , passhint = '%3' "
                ",is_admin = '%4' ,nickname = '%5' WHERE username='%6' ;";
    qryString = qryString.arg( userName, password, passHint,isAdmin,
                               nickName, userInfo.userName);
    //...
    if (qry.exec(qryString)) {
        // تغییرات در برنامه بصورت آنی لحاظ شود
        userInfo.password = pass;
        userInfo.passHint = passHint;

        return true;
    } else {
        return false;
    }
}
