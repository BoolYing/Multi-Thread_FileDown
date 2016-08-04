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

}

MainWindow::~MainWindow()
{
    delete ui;
    if(dow){
    delete dow;
    }
}

void MainWindow::on_pushButton_clicked()
{
    QUrl url= ui->lineEdit->text();
    QFileInfo info(url.path());
    QString fileName(info.fileName());
     //qDebug()<<"MainWindow -->filename: "<<fileName;
    dow = new DownloadControl();
    dow->DownloadFile(url,fileName,6);
    connect(dow,SIGNAL(FileDownloadFinished()),this,SLOT(TaskFinished()));
}

void MainWindow::TaskFinished(){
    qDebug()<<"The Task Finished !!!!!";
}


void MainWindow::on_pushButton_2_clicked()
{

}
