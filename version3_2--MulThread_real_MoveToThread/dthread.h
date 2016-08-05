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
    //设置下载链接与范围
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
     //改变标记值tag为true，结束死循环。
     void changeTag();


protected:
    void run();
};

#endif // DTHREAD

