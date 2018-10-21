#include "checkversiondialog.h"
#include "ui_checkversiondialog.h"

#include <QMovie>

CheckVersionDialog::CheckVersionDialog(QWidget *parent) :
    DraggableDialog(parent),
    ui(new Ui::CheckVersionDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::SplashScreen);
    //...
    reply = NULL;
    //...
    m_netManager = new QNetworkAccessManager(this);
    connect(m_netManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));
    //...
    timerTimeout.setSingleShot(true);
    connect(&timerTimeout, &QTimer::timeout, this, &CheckVersionDialog::timeOuted);

    QTimer::singleShot(1000, this, SLOT(checkNewVersionAvilable()));
    //...    
    QMovie *movie = new QMovie(":/loading.gif", QByteArray(), this);
    ui->lblRetVal->setMovie(movie);
    movie->start();
    //...
}

CheckVersionDialog::~CheckVersionDialog()
{
    delete ui;
}

void CheckVersionDialog::checkNewVersionAvilable()
{
    // چون اجرای دستورات زیر در سازنده کلاس باعث فریز شدن فرم و انیمیشن آن برای مدت کمی میشد
    // مجبور شدم این دستورات را در این قسمت بنویسم

    //...

    timerTimeout.start(7000);   // 7 secs. timeout

    QString url = "http://www.chaluspl.ir/lib-apps/versions/index.php?appname=ketabyab";
    //QString url = "http://localhost/lib-apps/versions/index.php?appname=ketabyab";

    reply = m_netManager->get( QNetworkRequest(QUrl(url)) );

}

void CheckVersionDialog::replyFinished(QNetworkReply *reply)
{    
    if (timerTimeout.isActive()) {
        timerTimeout.stop();
    }

    //...

    /*
    int v = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (v >= 200 && v < 300) {  // Success
        // .....
    }
    */

    //...

    if(reply->error()) {
        /*
         qDebug() << reply->errorString();

         QString errMessage =reply->attribute(
                    QNetworkRequest::HttpReasonPhraseAttribute).toString();
         qDebug() << errMessage ;
        */

        QString str = QString("خطا در دریافت اطلاعات") + QString("\n") +
                QString("آیا به اینترنت متصل هستید؟");

        ui->lblRetVal->setText(str);

    } else {
        QString currVer = qApp->applicationVersion();
        QString latestVer  = reply->readAll();

        QString strResult;

        int compareResult = QString::compare(currVer, latestVer, Qt::CaseInsensitive);
        if (compareResult == 0) {
            strResult = " برنامه بروز است! ";
        } else if(compareResult < 0) {
            QString strOnlineVer = " نسخه جدیدتر برنامه موجود است " + QString("<br/>") +
                    latestVer + QString("<br/>");

            strResult += "<html><head/><body>";
            strResult += "<div>" + strOnlineVer + "</div> <br/>";
            strResult += "<a href=\"";
            strResult += "http://www.chaluspl.ir/lib-apps/index.html";
            strResult += "\">";
            strResult += "<span style=\" text-decoration: none; color:#0000ff;\">";
            strResult += "دریافت نسخه جدید";
            strResult += "</span>";
            strResult += "</a></body></html>";
        } else { /*if(compareResult > 0 )*/
            //  !یعنی برنامه نصب شده در سیستم کاربر از نسخه آنلاین بروزتر است
            strResult = "عدم تطبیق نسخ!";
        }

        ui->lblRetVal->setText(strResult);
    }

    //...

    reply->deleteLater();
}

void CheckVersionDialog::timeOuted()
{
    reply->abort();
}

void CheckVersionDialog::on_btnClose_clicked()
{
    close();
}
