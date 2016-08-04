#ifndef DOWNLOAD
#define DOWNLOAD

#include<QNetworkAccessManager>
#include<QNetworkReply>
#include<QNetworkRequest>
#include<QDebug>
#include<QFile>
#include<QUrl>
#include<QString>
#include<QPair>
#include<QTimer>

 typedef QPair<qint64,qint64> pair_2int64;

class Download:public QObject
{
    Q_OBJECT
public:
    Download(int _ID);
    void StartDownload(QUrl url,
                       QFile *_file,
                       qint64 _startBytes,
                       qint64 _endBytes,
                       QMutex *_mutex);
    ~Download();


private:
   QNetworkAccessManager *manager;
    QNetworkReply *reply;
    QFile *file;
    int Thread_ID;
    qint64 startBytes,endBytes;
    qint64 oldSize,newSize,leftSize;
    qint64 last_15s_Size;
    qint64 speed,time_left;
    pair_2int64 pair;
    QByteArray buffer;
    QTimer  *timer;


    QMutex * mutex;
signals:
    void DownloadFinished();
    void Finished_Thread();


private slots:
    void httpFinished();
    void httpReadyRead();

public slots:
    //和DownloadControl类的定时器绑定，每秒会计算一次当前线程的下载速度与剩余大小，
    //并将结果通过信号参数传递给DownloadControl类对象。
    void updateSpeed();
    void sendSpeed_leftSize(pair_2int64 &pair,int i);

};


















#endif // DOWNLOAD

