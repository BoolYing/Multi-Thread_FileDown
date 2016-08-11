#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include"downloadcontrol.h"

#include <QMainWindow>
#include<QNetworkAccessManager>
#include<QNetworkReply>
#include<QNetworkRequest>
#include<QDebug>
#include<QFile>
#include<QUrl>
#include<QString>
#include<QPair>
#include<QProgressBar>
#include<QTabWidget>
#include<QLabel>
#include<QPushButton>
#include<QPair>
#include<QHBoxLayout>
#include<QVBoxLayout>
#include<QSpacerItem>
#include<QFileDialog>
#include<QDesktopServices>
#include<string>
#include<windows.h>
#include<QMessageBox>


namespace Ui {
class MainWindow;
}

//每一个正在下载的任务都有一个对应的下载状态栏，包含ProgressBar、lable、button.
/*
class ProgressTools:public QObject {
    Q_OBJECT
public:
    ProgressTools(QObject *parent = 0,int task_id = 0);
    int task_ID;
    QLabel *filename ;
    QProgressBar *bar ;
    QLabel * speed;
    QLabel *lefttime;
    QPushButton *pauseDownload;
    QPushButton *stopDownload;
    QHBoxLayout * d_layout;
    QHBoxLayout * f_layout;
    QHBoxLayout * t_layout;
public slots:
    //void pause();
    //void startAgain();
 };
*/
class ProgressTools:public QObject {
    Q_OBJECT
public:
    ProgressTools(QObject *parent = 0,int task_id = 0,DownloadControl * _dow = 0);
    QWidget widget;
    int task_ID;   
    DownloadControl * dow;
    QLabel filename ;
    QProgressBar bar ;
    QLabel  speed;
    QLabel lefttime;
    QPushButton pauseDownload;
    QPushButton stopDownloadButton;
    QHBoxLayout  d_layout;
    QHBoxLayout  f_layout;
    QHBoxLayout  t_layout;
    int status; //按钮状态
signals:
    void pause_signal();//暂停下载信号
    void startAgain_signal();//继续下载信号
    void moveToRecycle(QUrl _url,QString _dir,QString _filename,qint64 _totalsize);

public slots:
    void pause();//暂停函数
    void startAgain();//继续下载函数
    void changeStatus();//改变按钮状态功能
    void stopDownload(); //停止正在下载的任务
 };

//使用pair将一个下载任务的 下载控制器 与 下载状态栏 的对象的指针包装，并存到任务列表中。
typedef  QPair<DownloadControl*,ProgressTools *> taskPair;

class FinishedTools:public QObject{

     Q_OBJECT
public:
   FinishedTools(QObject * parent = 0);
   QUrl url;
   QString file_Name;
   qint64 total_size;
   QString path;

   QWidget widget;
   QHBoxLayout layout;
   QLabel filename;
   QLabel totalSize; 
   QPushButton LookInDir;
   QPushButton delFile; 
signals:
   //发送到MainWindow类的信号，用来传递参数
   void moveToRecycle(QUrl _url,QString _dir,QString _filename,qint64 _totalsize);
public slots:
   void LookFileInDir();
   //点击删除按钮，会触发自己的槽函数，删除文件的同时，发送上边的信号传递参数，
   void Remove_to_Recycle();
};

class RecycleTools:public QObject{
    Q_OBJECT
public:
    RecycleTools(QObject *parent = 0);
    QUrl url;
    QString file_Name;
    qint64 total_size;
    QString path;

    QWidget widget;
    QHBoxLayout layout;
    QLabel filename;
    QLabel totalSize;
    QPushButton reloadButton;
    QPushButton destoryButton;
signals:
    //这个信号的作用，是发送再次开始下载所需要的参数（url，路径）
    void reload_Dow(QUrl url,QString);

public slots:
    //当reloadButton被点击时，触发这个槽函数，用来向MainWindow类发送重新下载的信号
    void reloadDownloading();

   // void thoroughDestory(QFile *Recycle_configFile);
    void thoroughDestory();

};



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void hideAll(int); //隐藏了所有部件
    qint64 GetFileSize(QUrl url,int tryTimes);

    //任务列表，保存下载任务的状态。
    QVector<taskPair> task;

    //保存完成任务的列表
    QVector<FinishedTools*> task_finish;

    //垃圾箱列表
    QVector<RecycleTools *> task_recycle;

    //判断剩余空间是否足够。
    bool Space_enough(qint64,QString);
    //判断任务是否重复。
    bool findExistTask(QUrl,QString);



private slots:
    void on_pushButton_clicked();
     void TaskFinished(QString,int,qint64,QString,QUrl);


    void upDateUI(int,QString,qint64,qint64,QString,QString);

    //接收来自下载完成界面的信息（url，文件路径，文件名，文件大小）
    void Move_To_Recycle(QUrl,QString,QString,qint64);
    //真正实现重新下载的地方
    void Reload_Downloading(QUrl _url,QString _path);
    //停止下载
    void Downloading_To_Recycle(QUrl,QString,QString,qint64);


private:
    Ui::MainWindow *ui;  
    QFile * file;
    QFile * configFile;

    DownloadControl *dow;

    //临时保存一个任务信息的pair。
    taskPair  pair;

    QTimer * timer;
    QVBoxLayout *downloading_layout;
    QVBoxLayout *Finished_layout;
    QVBoxLayout *Recycle_layout;
    int Task_ID;


};

#endif // MAINWINDOW_H
