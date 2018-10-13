#ifndef DBCONNECTION_H
#define DBCONNECTION_H

#include <QtSql>

//=================================================================================

struct UserInfo{
    bool isAdmin;
    QString userName;
    QString nickname;
};

struct AppInfo {
    QSqlDatabase db;
    QString databasePath;
};

extern AppInfo appInfo;
extern UserInfo userInfo;

//=================================================================================

enum DatabaseErrors
{
    NoError,
    FileNotFound,
    FileIsCorrupted,
    ConnectionError
};

//=================================================================================

/*
table_books :
key_field              // auto increment number
book_title
book_writer
book_translator
book_pub
book_topic
book_register_number
*/
enum DB_Books_Fields{
    KeyField,               // auto increment
    BookTitle,              // عنوان کتاب
    BookWriter,             // پدید آور
    BookTranslator,         // مترجم
    BookPub,                // انتشارات
    BookTopic,              // موضوع
    BookRegisterNumber,     // شماره ثبت
    DB_Books_Fields_Count   //  7
};

//=================================================================================

/*
table_users:
username
password
is_admin
nickname
*/
enum DB_Users_Fields {
    UserName ,
    Password ,
    IsAdmin  ,  // 0=false   ,   otherwise=true
    Nickname
};

//=================================================================================

DatabaseErrors createConnection( QSqlDatabase &db,const QString &dbPath);

bool createNewDatabase();

int  getNumberOfRecord(const QSqlDatabase &db = appInfo.db ,
                       const QString &tableName = QString("table_books"),
                       const QString &fieldName = QString("key_field")
                      );

bool isUserExist(const QString &userName);
bool isRegisterNumberExist(const QString &registerId);
bool setUsersPass(QString &pass);

QStringList allUserNames();

QString getUserPassword(const QString &userName);
QString getUserNickname(const QString &userName);

QStringList getTableUsersRecord(const QString &userName);

//NOTE:   این فعلن عملیاتی نیست-صرفا جهت آموزش هست
QStringList getTableBooksRecord(const QString &registerNumber);


#endif  //  #ifndef DBCONNECTION_H
