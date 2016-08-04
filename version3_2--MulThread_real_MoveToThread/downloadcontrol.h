#ifndef DOWNLOADCONTROL
#define DOWNLOADCONTROL
//#include<mainwindow.h>
#include<QNetworkAccessManager>
#include<QNetworkReply>
#include<QNetworkRequest>
#include<QDebug>
#include<QTimer>
#include<QFile>
#include<QUrl>
#include<QString>
#include<QPair>
#include<QThread>
#include<QMutex>
#include<QReadWriteLock>
#include<dthread.h>
#include<QVector>
#include<QEventLoop>
class MainWindow;
typedef QPair<qint64,qint64> pair_2int64;

class DownloadControl:public QObject
{
    Q_OBJECT
public:
    enum State{Waiting,Downloading,Pause,Stop,Finished};

    //DownloadControl(MainWindow * _window,QObject * parent);
     DownloadControl();
     ~DownloadControl();

     //开始下载
    void DownloadFile(QUrl url,QString saveFile,int _ThreadNum);

    //通过配置文件继续下载
    //void DownloadFile(QString configFile);

    //获取下载链接大小
    qint64 GetFileSize(QUrl url,int tryTimes);

    //用来获取错误信息。
    QString errorString();

    //每一个任务都有一个相对应的进度栏，包括进度条，文件名，下载速度，剩余时间等。
    //这个函数用来更新任务对应的进度栏。
    void updateUI(QString,QString);



    pair_2int64 pair;



private:
    //QNetworkAccessManager *manager;
    qint64 readySize,totalSize,leftSize;
    qint64 speed,time_left;
    QUrl url;
    QString saveFile;
    int ThreadNum,RunningThread; //线程数量
    QTimer *timer;          //1s的定时测速器
    QTimer *timer_15s;      //超时15s则出现网络异常，则暂停下载。
    MainWindow * window;
    QFile *file;
    State state;
    int Thread_Finished_Num;
    QVector<DThread*> threads;
    QString errorInfo;
    QMutex *mutex;


signals:
    void FileDownloadFinished();
    void  getPair(pair_2int64 & pair,int i);


private slots:
    //有线程完成下载的时候，这个槽函数被调用，当所有线程都完成下载任务，则发送下载完成信号。
     void SubPartFinished();
     void NetSpeed();

     //一个线程下载任务完结的时候，会发送这个信号到 下载控制器，通知自己即将结束。。
     void PrintThreadEnd();



};




















#endif // DOWNLOADCONTROL

