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
     DownloadControl(QObject *parent = 0, int _TASK_ID = 0);
     ~DownloadControl();

     //开始下载
    void DownloadFile(QUrl url, QString saveFile, int _ThreadNum, QString dir, QFile *_configFile);

    //通过配置文件继续下载
    //void DownloadFile(QString *configFile);

    //获取下载链接大小
    qint64 GetFileSize(QUrl url,int tryTimes);

    //用来获取错误信息。
    QString errorString();

    void pause();

    void startAgain();

    pair_2int64 pair;
    int TASK_ID;



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
    QFile * configFile; //配置文件的文件指针。
    State state;
    int Thread_Finished_Num;
    QVector<DThread*> threads;
    QString errorInfo;
    QMutex *mutex;

    QString FileDir;

    qint64 startArray[50];
    qint64 newArray[50];
    qint64 endArray[50];


signals:
    //下载完成会发送这个信号，将任务从正在下载界面，挪到下载完成界面。
    void FileDownloadFinished(QString saveFile,int TASK_ID,qint64 totalSize,QString path);

    //这个信号每秒发送，去请求获得每一个线程的数据。
    void  getPair(pair_2int64 & pair,int i);

    //每一个任务都有一个相对应的进度栏，包括进度条，文件名，下载速度，剩余时间等。
    //这个信号用来传递参数去更新状态栏。
    void send_Ui_Msg(int,QString,qint64,qint64,QString,QString);


private slots:
    //有线程完成下载的时候，这个槽函数被调用，当所有线程都完成下载任务，则发送下载完成信号。
     void SubPartFinished();
     //每秒统计当前下载速度，与剩余时间的槽函数。
     void NetSpeed();
     //一个线程下载任务完结的时候，会发送这个信号到 下载控制器，通知自己即将结束。。
     void PrintThreadEnd();

};


#endif // DOWNLOADCONTROL

