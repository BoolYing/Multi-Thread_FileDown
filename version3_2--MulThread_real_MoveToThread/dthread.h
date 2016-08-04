#ifndef DTHREAD
#define DTHREAD
#include<download.h>
#include<QThread>
#include<QFile>
#include<QNetworkAccessManager>
#include<QEventLoop>
#include<QTimer>
#include<QMutex>
#include<QCoreApplication>


class DThread:public QThread
{
    Q_OBJECT
public:
    DThread(int );
    Download * download;
    void SetInitValue(QUrl _url,QFile *_file,qint64 startPoint,qint64 endPoint,QMutex *);
    ~DThread();

private:
    int ID;
    QUrl url;
    QFile * file;
    qint64 startPoint,endPoint;
    QTimer * timer;
    QMutex * mutex;
     bool tag;
    //QNetworkAccessManager * manager;

private slots:
     void changeTag();


protected:
    void run();
};

#endif // DTHREAD

