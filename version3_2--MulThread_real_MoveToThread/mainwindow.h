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


namespace Ui {
class MainWindow;
}

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
typedef  QPair<DownloadControl*,ProgressTools *> taskPair;




class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
     void TaskFinished(QString);

     void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
    DownloadControl *dow;
    taskPair  pair;
    //用来保存下载
    QVector<taskPair> task;
    QTimer * timer;

};

#endif // MAINWINDOW_H
