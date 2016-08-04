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
    dow = NULL;
    timer = new QTimer;
    timer->start(1000);

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
}
ProgressTools::ProgressTools(){
    filename = new QLabel;
    bar = new QProgressBar;
    speed = new QLabel;
    lefttime = new QLabel;
    pauseDownload = new QPushButton;
    pauseDownload->setText("Pause");
    stopDownload = new QPushButton;
    stopDownload->setText("delete");
    layout = new QHBoxLayout;
    layout->addWidget(filename);
    layout->addWidget(bar);
    layout->addWidget(speed);
    layout->addWidget(lefttime);
    layout->addWidget(pauseDownload);
    layout->addWidget(stopDownload);
}
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
     //qDebug()<<"MainWindow -->filename: "<<fileName;
    dow = new DownloadControl();
    pair.first = dow;
    pair.second = new ProgressTools();
    task.append(pair);
    dow->DownloadFile(url,fileName,10);
    connect(dow,SIGNAL(FileDownloadFinished(QString)),this,SLOT(TaskFinished(QString)));
    ui->tab_1->setLayout(pair.second->layout);


}

void MainWindow::TaskFinished(QString filename){
    qDebug()<<filename<<"下载完成 !";
}

//暂停下载
void MainWindow::on_pushButton_2_clicked()
{


}
