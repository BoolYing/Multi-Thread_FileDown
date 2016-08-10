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
    /*****************************************/
    ui->setupUi(this);
    Task_ID = 0;
    dow = NULL;
    timer = new QTimer(this);
    timer->start(1000);
    downloading_layout = new QVBoxLayout();
    Finished_layout = new QVBoxLayout();
    Trash_layout = new QVBoxLayout();
    //分别给正在下载、下载完成、回收站设置垂直布局
    ui->tab_1->setLayout(downloading_layout);
    ui->tab_2->setLayout(Finished_layout);
    ui->tab_3->setLayout(Trash_layout);
    /****************************************/
    //获取当前工程的目录，再此检测是否存在配置文件。
    QString dir = QCoreApplication::applicationDirPath();
    qDebug()<<"dir :"<<dir;
    QString fulpath = dir +"/"+ "Download.config";
    QFileInfo fi(fulpath);

    if(!fi.isFile())
    {  //如果配置文件不存在，则创建配置文件。
        qDebug()<<"Download.config not exist. ";
        configFile = new QFile(fulpath);
        if(!configFile->open(QFile::ReadWrite  |QFile::Text))
        {
            QString errorInfo = "can not open file : " + configFile->errorString();
            configFile->close();
            configFile = NULL;
            qDebug()<<"Open file error!"<<errorInfo;
            return;
        }
        configFile->close();
    }

    else
    {  //配置文件存在，则读取所有未下载完的任务状态信息。加载到界面上。

        qDebug()<<"Download.config exist! ";
        configFile=new QFile(fulpath);
        //调试：输出配置文件全部内容
        if(!configFile->open(QFile::ReadOnly |QFile::Text))
        {
            QString errorInfo = "can not open file : " + configFile->errorString();
            configFile->close();
            configFile = NULL;
            qDebug()<<"Open file error!"<<errorInfo;
            return ;
        }
        /*****************加载任务信息到界面，方便继续下载****************/

        QTextStream txtInput(configFile);
        QByteArray bytearr;

        qDebug()<<"-----All_Downloading_Message -----\n";
        while(!txtInput.atEnd())
        {
            bytearr = configFile->readLine();
            QString strread(bytearr);
            qDebug() << strread ;
        }
        configFile->close();

        qDebug()<<"-----All_Downloading_Message -----\n";
        //调试：配置文件内容输出完毕

        /********************加载配置文件内容到主界面****************************/
        QString strAll;
        QStringList strList;
        QString _url;
        QString _fileDir;
        qint64 startPoint,newSize,endPoint;
        qint64 totalSize = 0;
        qint64 leftSize = 0;
        int Thread_Num;
  /****************************************************************************************/
        if(!configFile->open(QFile::ReadOnly|QFile::Text))//打开配置文件,读取所有信息到 缓冲字符串strAll
        {  //打开文件失败
            QString errorInfo =configFile->errorString();
            configFile->close();
            configFile = NULL;
            qDebug()<<"Open file error in Change_ConfigFile() :"<<errorInfo;
            return ;
        }
        //打开文件成功
        QTextStream stream(configFile);
        strAll = stream.readAll();
        //读取完关闭文件，因为信息已经被保存到了变量中
        configFile->close();



        strList = strAll.split("\n");
        for(int i =0;i<strList.count();i++)
        {
            if(strList.at(i).contains("http://",Qt::CaseInsensitive))
            {
                _url = strList.at(i);//读取url
                i++;//跳转到下一行，下一行内容为线程数。
                _fileDir = strList.at(i);//读取文件路径串
                i++;
                QString tempStr = strList.at(i);
                Thread_Num = tempStr.toInt();//读取线程数

                for(int j = 0;j<Thread_Num;j++) //每一个线程都有三条数据（开始点，已下载量，结束点）
                {
                    //读取每一个线程的三个数据，再将它们写入文件
                    tempStr=strList.at(i+j*3+1);
                    startPoint = tempStr.toLongLong();
                    tempStr=strList.at(i+j*3+2);
                    newSize = tempStr.toLongLong();
                    tempStr=strList.at(i+j*3+3);
                    endPoint = tempStr.toLongLong();
                    leftSize += (endPoint - startPoint - newSize); //得到文件未下载部分的大小
                }
                i =i + Thread_Num*3;

                QUrl url(_url);
                QFileInfo info(url.path());
                QString fileName(info.fileName());

                //任务编号，Task_ID,从0开始编号，每增加一个任务，编号+1 .
                dow = new DownloadControl(this,Task_ID,configFile,url,fileName,_fileDir);
                bool to =dow->Read_From_ConfigFile();
                if(to == false){
                    qDebug()<<"pause error!!! some file open error~";
                    return;
                }

                //把新任务的下载管理器指针与状态栏指针都保存到一个pair对象中。
                pair.first = dow;

                pair.second = new ProgressTools(this,Task_ID,dow);
                pair.second->pauseDownload.setText("继续下载");
                pair.second->status = 1;

                Task_ID++;

                //将pair添加到任务列表里去。
                task.append(pair);
                //在tab_1中添加当前新建下载任务的状态栏。
                downloading_layout->addWidget(&(pair.second->widget));
                //由于是继续以前未完成的下载任务，所以调用无参数版的 DownloadFile()

                totalSize = GetFileSize(url,3); //得到文件全部文件大小

                //下载管理器完成任务，发送信号表示已完成文件的下载

                connect(dow,SIGNAL(FileDownloadFinished(QString,int,qint64,QString)),
                        this,SLOT(TaskFinished(QString,int,qint64,QString)));
                //下载管理器通过信号来更新它的状态栏。
                connect(dow,SIGNAL(send_Ui_Msg(int,QString,qint64,qint64,QString,QString)),
                        this,SLOT(upDateUI(int,QString,qint64,qint64,QString,QString)));

                upDateUI(Task_ID-1,fileName,totalSize-leftSize,totalSize,QString("0 K/s"),QString(""));

             }
        }





        /*********************************************************************/




    }

    /************************************************************************************/


}

MainWindow::~MainWindow()
{
    delete downloading_layout;
    delete Finished_layout;
    delete Trash_layout;

    delete ui;
}

//每一个正在下载的任务都有一个对应的进度状态栏。
/*
ProgressTools::ProgressTools(QObject *parent,int task_id):QObject(parent)
{
    task_ID = task_id;
    filename = new QLabel;
    bar = new QProgressBar(this);
    speed = new QLabel(this);
    lefttime = new QLabel(this);
    pauseDownload = new QPushButton(this);
    pauseDownload->setText("暂停下载");
    stopDownload = new QPushButton(this);
    stopDownload->setText("停止");

    d_layout = new QHBoxLayout(this);
    d_layout->addWidget(filename);
    d_layout->addWidget(bar);
    d_layout->addWidget(speed);
    d_layout->addWidget(lefttime);
    d_layout->addWidget(pauseDownload);
    d_layout->addWidget(stopDownload);

    f_layout = new QHBoxLayout(this);
    t_layout = new QHBoxLayout(this);

}
*/

ProgressTools::ProgressTools(QObject *parent,int task_id,DownloadControl *_dow):QObject(parent)
{
    task_ID = task_id;
    status = 0;
    dow = _dow;
    //_window = parent;
    pauseDownload.setText("暂停下载");
    stopDownload.setText("停止");

    d_layout.addWidget(&filename);
    //filename.setParent(&widget);

    d_layout.addWidget(&bar);
   // bar.setParent( &widget);

    d_layout.addWidget(&speed);
    //speed.setParent(&widget);

    d_layout.addWidget(&lefttime);
   // lefttime.setParent(&widget);

    d_layout.addWidget(&pauseDownload);
   //pauseDownload.setParent(&widget);

    d_layout.addWidget(&stopDownload);
    //stopDownload.setParent(&widget);

    //d_layout.setParent(&widget);
    widget.setLayout(&d_layout);
    connect(&(this->pauseDownload),SIGNAL(clicked(bool)),this,SLOT(changeStatus()));
    connect(this,SIGNAL(pause_signal()),this,SLOT(pause()));
    connect(this,SIGNAL(startAgain_signal()),this,SLOT(startAgain()));
}


//点击按钮，若status为0，则发射暂停下载信号；否则为1，发射继续下载信号。
void ProgressTools::changeStatus(){
    if(status == 0){
        emit pause_signal();
        status = 1;
        }
    else{
        emit startAgain_signal();
        status = 0;
    }
}
//暂停下载
void ProgressTools::pause(){
    this->pauseDownload.setText("继续下载");
    dow->pause();
}
//继续下载
void ProgressTools::startAgain(){
    this->pauseDownload.setText("暂停下载");
    dow->startAgain();

}


FinishedTools::FinishedTools(QObject *parent):QObject(parent)
{
    LookInDir.setText("在目录中查看");
    delFile.setText("删除任务文件");

    layout.addWidget(&filename);
    layout.addWidget(&totalSize);
    layout.addWidget(&LookInDir);
    layout.addWidget(&delFile);

    connect(&(this->LookInDir),SIGNAL(clicked(bool)),this,SLOT(LookFileInDir()));

    connect(&(this->delFile),SIGNAL(clicked(bool)),this,SLOT(RemoveFiles()));
}
//在目录中查看文件
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
//一键删除文件
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
    dow = new DownloadControl(this,Task_ID,configFile,url,fileName,dir);

    //把新任务的下载管理器指针与状态栏指针都保存到一个pair对象中。
    pair.first = dow;
    pair.second = new ProgressTools(this,Task_ID++,dow);


    //将pair添加到任务列表里去。
    task.append(pair);
    //在tab_1中添加当前新建下载任务的状态栏。

    downloading_layout->addWidget(&(pair.second->widget));

    //开启下载管理器，启用五个线程进行下载
    dow->DownloadFile(5);

    //下载管理器完成任务，发送信号表示已完成文件的下载
    connect(dow,SIGNAL(FileDownloadFinished(QString,int,qint64,QString)),
            this,SLOT(TaskFinished(QString,int,qint64,QString)));



    //下载管理器通过信号来更新它的状态栏。
    connect(dow,SIGNAL(send_Ui_Msg(int,QString,qint64,qint64,QString,QString)),
            this,SLOT(upDateUI(int,QString,qint64,qint64,QString,QString)));
}

void MainWindow::TaskFinished(QString _filename,int _task_id,qint64 _totalSize,QString _path){
    qDebug()<<_path<<_filename<<"下载完成 !";
    //qDebug()<<"file path  :"<<_path;
    int i;
    for(i =0;i<task.length();i++){
        if(task[i].first->TASK_ID == _task_id)
            break;
    }
    disconnect(task[i].first,SIGNAL(send_Ui_Msg(int,QString,qint64,qint64,QString,QString)),
            this,SLOT(upDateUI(int,QString,qint64,qint64,QString,QString)));

    //将已经下载完成的任务的状态栏，从正在下载界面隐藏。
    hideAll(i);


   // ui->tab_2->show();
    FinishedTools *tools = new FinishedTools(this);
    task_finish.append(tools);
    tools->filename.setText(_filename);
    tools->totalSize.setText(QString::number(_totalSize/1024)+ QString("Kb"));
    tools->path = _path;    


    //在下载完成界面中，添加任务状态。
    Finished_layout->addLayout(&(tools->layout));
    qDebug()<<"Finished_layout->addLayout(&(tools->layout)) ok !";

}
//某一个任务下载完后，隐藏与这个任务有关的进度栏等条目。
/*
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

}

*/
void MainWindow::hideAll(int i){
    ProgressTools * tools=task[i].second;
/*
    tools->bar.hide();

    tools->filename.hide();

    tools->lefttime.hide();

    tools->speed.hide();

    tools->pauseDownload.hide();

    tools->stopDownload.hide();

    tools->widget.deleteLater();
    */


    tools->widget.hide();
     // tools->deleteLater();

}

void MainWindow::upDateUI(int Task_ID,
               QString filename,
               qint64 readSize,
               qint64 totalSize,
               QString speed,
               QString lefttime)
 {

    int i ;
    for(i =0;i<task.length();i++){
        if(task[i].second->task_ID == Task_ID)
            break;
    }
    ProgressTools * tools;
     //指向Task_ID对应的那个状态栏
     tools = task[i].second;
     //更新进度条、文件名、速度与剩余时间
     tools->bar.setMaximum(totalSize);
     tools->bar.setValue(readSize);
     tools->filename.setText(filename);
     tools->speed.setText(speed);
     tools->lefttime.setText(lefttime);

 }

qint64 MainWindow::GetFileSize(QUrl url, int tryTimes){
    qint64 size = -1;

    //尝试tryTimes次
    while(tryTimes --)
    {
        QNetworkAccessManager manager;
        QEventLoop loop;
        //发出请求，获取目标地址的头部信息
        QNetworkReply *reply = manager.head(QNetworkRequest(url));
        if(!reply)continue;

        connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
        loop.exec();

        if(reply->error() != QNetworkReply::NoError)
        {
            QString errorInfo = reply->errorString();
            qDebug()<<errorInfo;
            continue;
        }
        QVariant var = reply->header(QNetworkRequest::ContentLengthHeader);
        reply->deleteLater();
        size = var.toLongLong();
        break;
    }
    return size;
}
bool MainWindow::Space_enough()
{
 /*   std::string path=_fileName.toStdString();
    std::string rootName(path.begin(),path.begin()+3);
    QString name(rootName.c_str());

    LPCWSTR lpcwstrDriver=(LPCWSTR)name.utf16();
    ULARGE_INTEGER liFreeBytesAvailable, liTotalBytes, liTotalFreeBytes;
    if( !GetDiskFreeSpaceEx( lpcwstrDriver, &liFreeBytesAvailable, &liTotalBytes, &liTotalFreeBytes) )
    {
           qDebug() << "ERROR: Call to GetDiskFreeSpaceEx() failed.";
           return 0;
    }
    qDebug()<<"freespace:"<<(quint64)liTotalFreeBytes.QuadPart<<endl;
    return (quint64)liTotalFreeBytes.QuadPart / (quint64)_fileSize;   //返回0无空间  返回1有空间
    */

    return true;
}

