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
#include<QSpacerItem>
#include<QBoxLayout>


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

   // download_space = new QSpacerItem(20,20);
   // downloading_layout->addSpacerItem(download_space);

}

MainWindow::~MainWindow()
{
    delete ui;
    for(int i =0;i<task.length();i++){
        if(task[i].first){
            delete task[i].first;
            task[i].first = NULL;
        }
        if(task[i].second){
            delete task[i].second;
            task[i].second = NULL;
        }
    }
    for(int i =0;i<task_finish.length();i++)
    {
        if(task_finish[i]){
            delete task_finish[i];
            task_finish[i] = NULL;
        }
    }
    delete downloading_layout;
    downloading_layout = NULL;
    delete Finished_layout;
    Finished_layout = NULL;
    delete Trash_layout;
    Trash_layout = NULL;
}
//每一个正在下载的任务都有一个对应的进度状态栏。
ProgressTools::ProgressTools(){
    filename = new QLabel;
    bar = new QProgressBar;
    speed = new QLabel;
    lefttime = new QLabel;
    pauseDownload = new QPushButton;
    pauseDownload->setText("暂停下载");
    stopDownload = new QPushButton;
    stopDownload->setText("停止");

    d_layout = new QHBoxLayout;
    d_layout->addWidget(filename);
    d_layout->addWidget(bar);
    d_layout->addWidget(speed);
    d_layout->addWidget(lefttime);
    d_layout->addWidget(pauseDownload);
    d_layout->addWidget(stopDownload);

    f_layout = new QHBoxLayout;
    t_layout = new QHBoxLayout;

}
//进度状态栏的析构函数。
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
    delete d_layout ;
    d_layout  = NULL;
    delete t_layout;
    t_layout = NULL;
    delete f_layout;
    f_layout = NULL;

}
FinishedTools::FinishedTools(){
    layout.addWidget(&filename);
    layout.addWidget(&totalSize);
    layout.addWidget(&LookInDir);
    layout.addWidget(&delFile);
    connect(&(this->LookInDir),SIGNAL(clicked(bool)),this,SLOT(LookFileInDir()));

    connect(&(this->delFile),SIGNAL(clicked(bool)),this,SLOT(RemoveFiles()));

}
void FinishedTools::LookFileInDir(){
    QString fulpath;
    fulpath = this->path +"/"+ this->filename.text();
    QFileInfo fi(fulpath);
    if(!fi.isFile()){
        QMessageBox msgBox;
        msgBox.setText("The file has been deleted.");
        msgBox.exec();
    }

    else{
        QDesktopServices::openUrl(QUrl(this->path, QUrl::TolerantMode));
    }
}
void FinishedTools::RemoveFiles(){
    QString fulpath;
    fulpath = this->path +"/"+ this->filename.text();
     QFile::remove(fulpath);//刪除文件
}

//新建下载任务
void MainWindow::on_pushButton_clicked()
{


    QUrl url= ui->lineEdit->text();
    QFileInfo info(url.path());

   QString dir = QFileDialog::getExistingDirectory(this);

    qDebug()<< "path :"<<dir;
    QString fileName(info.fileName());



    //任务编号，Task_ID,从0开始编号，每增加一个任务，编号+1 .
    dow = new DownloadControl(Task_ID++);

    //把新任务的下载管理器指针与状态栏指针都保存到一个pair对象中。
    pair.first = dow;
    pair.second = new ProgressTools();

    //将pair添加到任务列表里去。
    task.append(pair);
    //在tab_1中添加当前新建下载任务的状态栏。
    downloading_layout->addLayout(pair.second->d_layout);

    //开启下载管理器
    dow->DownloadFile(url,fileName,10,dir);

    //下载管理器完成任务，发送信号表示已完成文件的下载
    connect(dow,SIGNAL(FileDownloadFinished(QString,int,qint64,QString)),
            this,SLOT(TaskFinished(QString,int,qint64,QString)));



    //下载管理器通过信号来更新它的状态栏。
    connect(dow,SIGNAL(send_Ui_Msg(int,QString,qint64,qint64,QString,QString)),
            this,SLOT(upDateUI(int,QString,qint64,qint64,QString,QString)));

}

void MainWindow::TaskFinished(QString _filename,int _task_id,qint64 _totalSize,QString _path){
    qDebug()<<_filename<<"下载完成 !";

    hideAll(_task_id);

   // ui->tab_2->show();
    FinishedTools *tools = new FinishedTools;
    task_finish.append(tools);
    tools->filename.setText( _filename  );
    tools->totalSize.setText(QString::number(_totalSize/1024)+ QString("Kb"));
    tools->path = _path;
    tools->LookInDir.setText("在目录中查看");
    tools->delFile.setText("删除任务文件");
    //connect(&(tools->LookInDir),SIGNAL(clicked(bool)),this,SLOT(LookFileInDir()));

    qDebug()<<"file path  :"<<_path;
    Finished_layout->addLayout(&(tools->layout));
    qDebug()<<"Finished_layout->addLayout(&(tools->layout)) ok !";


   // QLayoutItem *item = task[_task_id].second->layout;
   // downloading_layout->removeItem(item);
}
void MainWindow::hideAll(int id){
    ProgressTools * tools=task[id].second;
    tools->bar->hide();
    tools->d_layout->removeWidget(tools->bar);
    tools->filename->hide();
    tools->d_layout->removeWidget(tools->filename);
    tools->lefttime->hide();
    tools->d_layout->removeWidget(tools->lefttime);
    tools->speed->hide();
    tools->d_layout->removeWidget(tools->speed);
    tools->pauseDownload->hide();
    tools->d_layout->removeWidget(tools->pauseDownload);
    tools->stopDownload->hide();
    tools->d_layout->removeWidget(tools->stopDownload);


     //delete task[id].second;
    //tools->d_layout->removeItem();

    //tools->filename->

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




