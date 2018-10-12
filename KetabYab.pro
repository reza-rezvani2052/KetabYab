#-------------------------------------------------
#
# Project created by QtCreator 2017-07-15T11:14:29
#
#-------------------------------------------------

QT       += core gui widgets xml sql network

TARGET = KetabYab
TEMPLATE = app

#مسیر فایل اجرایی
DESTDIR = $$PWD/bin

#MOC_DIR = $$PWD/bin   یا   $$DESTDIR
#OBJECTS_DIR = $$PWD/bin
#RCC_DIR = $$PWD/bin
#UI_DIR = $$PWD/bin


DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    aboutdialog.cpp \
    dbconnection.cpp \
    logindialog.cpp \
    popupdialog.cpp \
    advancedsearchdialog.cpp \
    utility.cpp \
    databaseerrormanagementdialog.cpp \
    advancedsearchselectedcolumnsdialog.cpp \
    checkversiondialog.cpp

HEADERS += \
        mainwindow.h \
    aboutdialog.h \
    dbconnection.h \
    logindialog.h \
    popupdialog.h \
    advancedsearchdialog.h \
    utility.h \
    databaseerrormanagementdialog.h \
    advancedsearchselectedcolumnsdialog.h \
    checkversiondialog.h

FORMS += \
        mainwindow.ui \
    aboutdialog.ui \
    logindialog.ui \
    popupdialog.ui \
    advancedsearchdialog.ui \
    databaseerrormanagementdialog.ui \
    advancedsearchselectedcolumnsdialog.ui \
    checkversiondialog.ui

RESOURCES += \
    rc/rc.qrc


win32{
    RC_FILE = rc/rc.rc
#   message( "hmmmmmm" )
#   message( PWD = $$PWD )
}unix{
    error("RC FILE is not set")
}macx{
    error("RC FILE is not set")
}

!exists( $$DESTDIR/db/books.db ) {
     message( "Database file not found! ($$DESTDIR/db/books.db )" )
}

!packagesExist(sqlite3) {
   warning("Need more Package!")
#    error("sqlite3 NOT exist!")
}


#Test only:
#system("ls /bin"): HAS_BIN = TRUE


DISTFILES += \
    TODO.txt
