#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include"downloadcontrol.h"
#include"download.h"
#include"dthread.h"

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
    ProgressTools(QObject *parent = 0,int task_id = 0);
    QWidget widget;
    int task_ID;
    QLabel filename ;
    QProgressBar bar ;
    QLabel  speed;
    QLabel lefttime;
    QPushButton pauseDownload;
    QPushButton stopDownload;
    QHBoxLayout  d_layout;
    QHBoxLayout  f_layout;
    QHBoxLayout  t_layout;
private:
    int status; //按钮状态
signals:
    void pause_signal();//暂停下载信号
    void startAgain_signal();//继续下载信号

public slots:
    void pause();//暂停函数
    void startAgain();//继续下载函数
    void changeStatus();//改变按钮状态功能
 };

//使用pair将一个下载任务的 下载控制器 与 下载状态栏 的对象的指针包装，并存到任务列表中。
typedef  QPair<DownloadControl*,ProgressTools *> taskPair;

class FinishedTools:public QObject{

     Q_OBJECT
public:
   FinishedTools(QObject * parent = 0);
   QHBoxLayout layout;
   QLabel filename;
   QLabel totalSize;
   QString path;
   QPushButton LookInDir;
   QPushButton delFile; 
public slots:
   void LookFileInDir();
   void RemoveFiles();
};




class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void hideAll(int);

    //任务列表，保存下载任务的状态。
    QVector<taskPair> task;

    //保存完成任务的列表
    QVector<FinishedTools*> task_finish;

    //垃圾箱列表

private slots:
    void on_pushButton_clicked();
    void TaskFinished(QString,int,qint64,QString);

    void upDateUI(int,QString,qint64,qint64,QString,QString);


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
    QVBoxLayout *Trash_layout;
    int Task_ID;
    QSpacerItem *download_space;


};

#endif // MAINWINDOW_H
