#ifndef DBCONNECTION_H
#define DBCONNECTION_H

#include <QtSql>

//=================================================================================

struct UserInfo {
    QString userName;
    QString password;   // TODO: ****  takmil gardad
    QString passHint;
    bool isAdmin;
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
passhint
is_admin
nickname
*/
enum DB_Users_Fields {
    UserName ,
    Password ,
    PassHint ,
    IsAdmin  ,  // 0=false   ,   otherwise=true
    Nickname
};

//=================================================================================

DatabaseErrors createConnection( QSqlDatabase &db,const QString &dbPath);

bool createNewDatabase();

int  getNumberOfRecord(const QString &tableName = QString("table_books"),
                       const QString &fieldName = QString("key_field")
                      );

bool isUserExist(const QString &userName);
bool isRegisterNumberExist(const QString &registerId);
bool setUsersPass(QString &pass, QString passHint = QString() );

QStringList allUserNames();

QString getUserPassword(const QString &userName);
QString getUserPassHint(const QString &userName);
QString getUserNickname(const QString &userName);

QStringList getTableUsersRecord(const QString &userName);

QStringList getTableBooksRecord(const QString &registerNumber);


#endif  //  #ifndef DBCONNECTION_H
