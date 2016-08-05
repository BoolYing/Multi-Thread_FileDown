#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QNetworkAccessManager>
#include<QNetworkRequest>
#include<QNetworkReply>
#include<QDebug>
#include<QFile>
#include<QUrl>
#include<QFileInfo>
#include<QMessageBox>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    Task_ID = 0;
    dow = NULL;
    timer = new QTimer;
    timer->start(1000);
    downloading_layout = new QVBoxLayout;
    Finished_layout = new QVBoxLayout;
    Trash_layout = new QVBoxLayout;

    //分别给正在下载、下载完成、回收站设置垂直布局
    ui->tab_1->setLayout(downloading_layout);
    ui->tab_2->setLayout(Finished_layout);
    ui->tab_3->setLayout(Trash_layout);

}

MainWindow::~MainWindow()
{
    delete ui;
    for(int i =0;i<task.length();i++){
        if(!task[i].first){
            delete task[i].first;
            task[i].first = NULL;
        }
        if(!task[i].second){
            delete task[i].second;
            task[i].second = NULL;

        }
    }
    delete downloading_layout;
    downloading_layout = NULL;
    delete Finished_layout;
    Finished_layout = NULL;
    delete Trash_layout;
    Trash_layout = NULL;
}
//每一个正在下载的任务都有一个对应的下载状态栏。
ProgressTools::ProgressTools(){
    filename = new QLabel;
    bar = new QProgressBar;
    speed = new QLabel;
    lefttime = new QLabel;
    pauseDownload = new QPushButton;
    pauseDownload->setText("暂停");
    stopDownload = new QPushButton;
    stopDownload->setText("停止");
    layout = new QHBoxLayout;
    layout->addWidget(filename);
    layout->addWidget(bar);
    layout->addWidget(speed);
    layout->addWidget(lefttime);
    layout->addWidget(pauseDownload);
    layout->addWidget(stopDownload);
}
//进度条的析构函数。
ProgressTools::~ProgressTools(){
    delete filename;
    filename = NULL;
    delete bar;
    bar = NULL;
    delete  speed;
    speed = NULL;
    delete  lefttime;
    lefttime = NULL;
    delete pauseDownload;
    pauseDownload = NULL;
    delete layout ;
    layout  = NULL;

}

//新建下载任务
void MainWindow::on_pushButton_clicked()
{

    QUrl url= ui->lineEdit->text();
    QFileInfo info(url.path());
    QString fileName(info.fileName());

    //任务编号。
    dow = new DownloadControl(Task_ID++);
    pair.first = dow;
    pair.second = new ProgressTools();
    task.append(pair);
    dow->DownloadFile(url,fileName,30);
    connect(dow,SIGNAL(FileDownloadFinished(QString)),this,SLOT(TaskFinished(QString)));
    //在tab_1中添加当前新建下载任务的状态栏。
    downloading_layout->addLayout(pair.second->layout);
    connect(dow,SIGNAL(send_Ui_Msg(int,QString,qint64,qint64,QString,QString)),
            this,SLOT(upDateUI(int,QString,qint64,qint64,QString,QString)));

}

void MainWindow::TaskFinished(QString filename){
    qDebug()<<filename<<"下载完成 !";
}

//暂停下载
void MainWindow::on_pushButton_2_clicked()
{


}



void MainWindow::upDateUI(int Task_ID,
               QString filename,
               qint64 readSize,
               qint64 totalSize,
               QString speed,
               QString lefttime)
 {
     int id = Task_ID;
     ProgressTools * tools;
     //指向Task_ID对应的那个状态栏
     tools = task[id].second;
     //更新进度条、文件名、速度与剩余时间
     tools->bar->setMaximum(totalSize);
     tools->bar->setValue(readSize);
     tools->filename->setText(filename);
     tools->speed->setText(speed);
     tools->lefttime->setText(lefttime);

 }




