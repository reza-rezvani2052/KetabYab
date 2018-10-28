#ifndef DBCONNECTION_H
#define DBCONNECTION_H

#include <QtSql>

//=================================================================================

struct UserInfo {
    QString userName;  //Readonly
    QString password;
    QString passHint;
    bool isAdmin;      //Readonly
    QString nickname;  //Readonly
};

struct AppInfo {
    QSqlDatabase db;
    QString databasePath;
    bool isBackdoorLogin;
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
book_register_number            //key field
book_title
book_writer
book_translator
book_pub
book_topic
*/
enum DB_Books_Fields{
    BookRegisterNumber,     // شماره ثبت  // Key Field
    BookTitle,              // عنوان کتاب
    BookWriter,             // پدید آور
    BookTranslator,         // مترجم
    BookPub,                // انتشارات
    BookTopic,              // موضوع    
    DB_Books_Fields_Count   //  6
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
                       const QString &fieldName = QString("key_field") );

bool isUserExist(const QString &userName);
bool isRegisterNumberExist(const QString &registerId);

QStringList allUserNames();

bool    isPasswordSet(const QString &userName);
QString getUserPassword(const QString &userName);
bool    setUsersPass(QString &pass, QString passHint = QString() );

QString getUserPassHint(const QString &userName);
QString getUserNickname(const QString &userName);

QStringList getTableUsersRecord(const QString &userName);
QStringList getTableBooksRecord(const QString &registerNumber);

bool isSearchPhraseExist(const QString &phrase);
QStringList getMostSearchedPhrases();
bool setMostSearchedPhrase(const QString &phrase);
void clearSearchHistory();

#endif  //  #ifndef DBCONNECTION_H
