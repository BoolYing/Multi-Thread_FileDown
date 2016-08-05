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


namespace Ui {
class MainWindow;
}

//每一个正在下载的任务都有一个对应的下载状态栏，包含ProgressBar、lable、button.
struct ProgressTools{
public:
    ProgressTools();
    ~ProgressTools();
    QLabel * filename ;
    QProgressBar *bar ;
    QLabel * speed;
    QLabel *lefttime;
    QPushButton *pauseDownload;
    QPushButton *stopDownload;
    QHBoxLayout * layout;
 };
//使用pair将一个下载任务的 下载控制器 与 下载状态栏 的对象的指针包装，并存到任务列表中。
typedef  QPair<DownloadControl*,ProgressTools *> taskPair;




class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    //任务列表，保存下载任务的状态。
    QVector<taskPair> task;

private slots:
    void on_pushButton_clicked();
     void TaskFinished(QString);

     void on_pushButton_2_clicked();
     void upDateUI(int,QString,qint64,qint64,QString,QString);

private:
    Ui::MainWindow *ui;
    DownloadControl *dow;

    //临时保存一个任务信息的pair。
    taskPair  pair;

    QTimer * timer;
    QVBoxLayout *downloading_layout;
    QVBoxLayout *Finished_layout;
    QVBoxLayout *Trash_layout;
    int Task_ID;

};

#endif // MAINWINDOW_H
