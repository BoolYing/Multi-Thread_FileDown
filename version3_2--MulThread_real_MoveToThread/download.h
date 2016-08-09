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
    explicit Download(QObject *parent = 0,int _ID = 0);
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
    //任务的开始与结束点
    qint64 startBytes,endBytes;
    //用来记录当前线程已经下载的字节、1s之前的字节、与剩余字节。
    qint64 oldSize,newSize,leftSize;
    //15s之前下载的字节，用来判断网络情况。
    qint64 last_15s_Size;
    //下载速度与剩余时间
    qint64 speed,time_left;
    //用来打包speed与未下载字节大小，发送给下载管理器每秒进行统计。
    pair_2int64 pair;
    //从缓冲区读取字节到buffer
    QByteArray buffer;
    QTimer  *timer;

    QMutex * mutex;
signals:
   //下载完成，会发送信号到线程中，终止线程里的等待循环，并结束线程。
    void Finished_Thread();


private slots:
    //下载完成被触发的槽
    void httpFinished();
    //有数据可读的时候，读取数据并更新已读取字节大小
    void httpReadyRead();

public slots:
    //和DownloadControl类的定时器绑定，每秒会计算一次当前线程的下载速度与剩余大小，
    //并将结果通过信号参数传递给DownloadControl类对象。
    void updateSpeed();
    void sendSpeed_leftSize(pair_2int64 &pair,int i);

};


















#endif // DOWNLOAD

