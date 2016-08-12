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
    Recycle_layout = new QVBoxLayout();
    //分别给正在下载、下载完成、回收站设置垂直布局
    ui->tab_1->setLayout(downloading_layout);
    ui->tab_2->setLayout(Finished_layout);
    ui->tab_3->setLayout(Recycle_layout);
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

                connect(dow,SIGNAL(FileDownloadFinished(QString,int,qint64,QString,QUrl)),
                        this,SLOT(TaskFinished(QString,int,qint64,QString,QUrl)));
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
    delete Recycle_layout;

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
    stopDownloadButton.setText("停止");

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

    d_layout.addWidget(&stopDownloadButton);
    //stopDownload.setParent(&widget);

    //d_layout.setParent(&widget);
    widget.setLayout(&d_layout);
    connect(&(this->pauseDownload),SIGNAL(clicked(bool)),this,SLOT(changeStatus()));
    connect(&(this->stopDownloadButton),SIGNAL(clicked(bool)),this,SLOT(stopDownload()));
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
void ProgressTools::stopDownload(){
    qDebug()<<"pause begin!!!!!!!!!!!!!!!!!!!!!!";
    QUrl url = dow->url;
    QString path = dow->FileDir;
    QString filename = dow->saveFile;
    qint64 totalSize = dow->totalSize;

    qDebug()<< url;
     qDebug()<<path;
      qDebug()<<filename;
       qDebug()<<totalSize;
    dow->pause();
    dow->DelFrom_ConfigFile();
    qDebug()<<"pause ok!!!!!!!!!!!!!!!!!!!!!!";


    QString fulpath;
    fulpath = path +"/"+ filename;
    QFile::remove(fulpath);//刪除文件

    emit moveToRecycle(url,path,filename,totalSize);
    widget.hide();
}



FinishedTools::FinishedTools(QObject *parent):QObject(parent)
{
    LookInDir.setText("在目录中查看");
    delFile.setText("删除任务文件");

    layout.addWidget(&filename);
    layout.addWidget(&totalSize);
    layout.addWidget(&LookInDir);
    layout.addWidget(&delFile);
    widget.setLayout(&layout);

    connect(&(this->LookInDir),SIGNAL(clicked(bool)),this,SLOT(LookFileInDir()));

    //点击删除，删除文件的同时将任务信息从下载完成界面挪到回收站界面，
    connect(&(this->delFile),SIGNAL(clicked(bool)),this,SLOT(Remove_to_Recycle()));
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
        //打开路径指向的目录
        QDesktopServices::openUrl(QUrl(this->path, QUrl::TolerantMode));
    }
}
//一键删除文件
void FinishedTools::Remove_to_Recycle(){
     QString fulpath;
     fulpath = this->path +"/"+ this->filename.text();
     QFile::remove(fulpath);//刪除文件
     widget.hide();
     //向MainWindow类发送一个传参数的信号。
     emit moveToRecycle(url,path,file_Name,total_size);

}

RecycleTools::RecycleTools(QObject *parent):QObject(parent){
    reloadButton.setText("重新下载");
    destoryButton.setText("彻底删除");
    layout.addWidget(&filename);
    layout.addWidget(&totalSize);
    layout.addWidget(&reloadButton);
    layout.addWidget(&destoryButton);

    widget.setLayout(&layout);
    connect(&reloadButton,SIGNAL(clicked(bool)),this,SLOT(reloadDownloading()));
    connect(&destoryButton,SIGNAL(clicked(bool)),this,SLOT(thoroughDestory()));
}
void RecycleTools::reloadDownloading(){

    //widget.hide();

    emit reload_Dow(url,path);
}
void RecycleTools::thoroughDestory(){
    widget.hide();
    //QFile *configFile = Recycle_configFile;
    //从回收站配置文件删除这个任务的信息。
}

void MainWindow::Reload_Downloading(QUrl _url, QString _path){
    int threadnum;
    QFileInfo info(_url.path());
    QString fileName(info.fileName());
    if(ui->lineEdit_2->text().isEmpty())
    {
        QMessageBox::warning(this,tr("警告"),tr(" \n请在下方输入下载需要开辟的线程数\n"),QMessageBox::Yes);
        return ;
    }
    else
    {
        threadnum = ui->lineEdit_2->text().toInt();
        if(threadnum <1){
        QMessageBox::warning(this,tr("警告"),tr(" \nThread Num must more than 1 !"),QMessageBox::Yes);
        ui->lineEdit_2->clear();
        return ;
        }
        if(threadnum >100){
        QMessageBox::warning(this,tr("警告"),tr(" \n Don't more than 100"),QMessageBox::Yes);
        ui->lineEdit_2->clear();
        return ;
        }
    }



     //判断任务是否重复
    //判断任务是否重复
    if(findExistTask(_url,_path) == true){ //如果存在重复任务，则拒绝下载。
        qDebug()<<"This task is exist,dont need to download .";
        QMessageBox::warning(this,tr("提示"),tr(" \n 任务已经存在，无需下载\n"),QMessageBox::Yes);
        return ;
    }

    //判断磁盘空间是否足够
    qint64 totalSize = GetFileSize(_url,3);
    bool DiskSpace = Space_enough(totalSize,_path);
    if(DiskSpace == false){
        qDebug()<<"Disk Space is not enough !";
        QMessageBox::warning(this,tr("提示"),tr(" \n 硬盘空间不足\n"),QMessageBox::Yes);
        return ;
    }
    dow = new DownloadControl(this,Task_ID,configFile,_url,fileName,_path);
    dow->Write_To_ConfigFile(); //先在配置文件中声明自己已经存在。

    //把新任务的下载管理器指针与状态栏指针都保存到一个pair对象中。
    pair.first = dow;
    pair.second = new ProgressTools(this,Task_ID++,dow);


    //将pair添加到任务列表里去。
    task.append(pair);
    //在tab_1中添加当前新建下载任务的状态栏。

    downloading_layout->addWidget(&(pair.second->widget));

    //开启下载管理器，启用threadnum个线程进行下载
    dow->DownloadFile(threadnum);


    //下载管理器完成任务，发送信号表示已完成文件的下载
    connect(dow,SIGNAL(FileDownloadFinished(QString,int,qint64,QString,QUrl)),
            this,SLOT(TaskFinished(QString,int,qint64,QString,QUrl)));



    //下载管理器通过信号来更新它的状态栏。
    connect(dow,SIGNAL(send_Ui_Msg(int,QString,qint64,qint64,QString,QString)),
            this,SLOT(upDateUI(int,QString,qint64,qint64,QString,QString)));




}

//新建下载任务
void MainWindow::on_pushButton_clicked()
{
    if(ui->lineEdit->text().isEmpty()){
        QMessageBox::warning(this,tr("警告"),tr(" \n请输入下载链接\n"),QMessageBox::Yes);
        return;
    }

    QUrl url= ui->lineEdit->text();    
    QFileInfo info(url.path());
    QString fileName(info.fileName());
    //求取线程数
    int threadnum;
    if(ui->lineEdit_2->text().isEmpty())
    {
        QMessageBox::warning(this,tr("警告"),tr(" \n请在下方输入下载需要开辟的线程数\n"),QMessageBox::Yes);
        return ;
    }
    else
    {
        threadnum = ui->lineEdit_2->text().toInt();
        if(threadnum <1){
        QMessageBox::warning(this,tr("警告"),tr(" \nThread Num must more than 1 !"),QMessageBox::Yes);
        ui->lineEdit_2->clear();
        return ;
        }
        if(threadnum >100){
        QMessageBox::warning(this,tr("警告"),tr(" \n Don't more than 100"),QMessageBox::Yes);
        ui->lineEdit_2->clear();
        return ;
        }
    }


    QString dir = QFileDialog::getExistingDirectory();
    if(dir.isEmpty()){
        QMessageBox::warning(this,tr("警告"),tr(" \n 请选择下载路径\n"),QMessageBox::Yes);
        return ;
    }
    qDebug()<< "path :"<<dir;





    //判断任务是否重复
    if(findExistTask(url,dir) == true){ //如果存在重复任务，则拒绝下载。
        qDebug()<<"This task is exist,dont need to download .";
        QMessageBox::warning(this,tr("提示"),tr(" \n 任务已经存在，无需下载\n"),QMessageBox::Yes);
        return ;
    }

    //判断磁盘空间是否足够
    qint64 totalSize = GetFileSize(url,3);
    bool DiskSpace = Space_enough(totalSize,dir);
    if(DiskSpace == false){
        qDebug()<<"Disk Space is not enough !";
        QMessageBox::warning(this,tr("提示"),tr(" \n 硬盘空间不足\n"),QMessageBox::Yes);
        return ;
    }



    //任务编号，Task_ID,从0开始编号，每增加一个任务，编号+1 .
    dow = new DownloadControl(this,Task_ID,configFile,url,fileName,dir);
    dow->Write_To_ConfigFile(); //先在配置文件中声明自己已经存在。

    //把新任务的下载管理器指针与状态栏指针都保存到一个pair对象中。
    pair.first = dow;
    pair.second = new ProgressTools(this,Task_ID++,dow);


    //将pair添加到任务列表里去。
    task.append(pair);
    //在tab_1中添加当前新建下载任务的状态栏。

    downloading_layout->addWidget(&(pair.second->widget));

    //开启下载管理器，启用五个线程进行下载
    dow->DownloadFile(threadnum);


    //下载管理器完成任务，发送信号表示已完成文件的下载
    connect(dow,SIGNAL(FileDownloadFinished(QString,int,qint64,QString,QUrl)),
            this,SLOT(TaskFinished(QString,int,qint64,QString,QUrl)));

    connect(pair.second,SIGNAL(moveToRecycle(QUrl,QString,QString,qint64)),
            this,SLOT(Move_To_Recycle(QUrl,QString,QString,qint64)));


    //下载管理器通过信号来更新它的状态栏。
    connect(dow,SIGNAL(send_Ui_Msg(int,QString,qint64,qint64,QString,QString)),
            this,SLOT(upDateUI(int,QString,qint64,qint64,QString,QString)));
}

//一个下载任务彻底完成，会触发这个槽函数。
void MainWindow::TaskFinished(QString _filename,int _task_id,qint64 _totalSize,QString _path,QUrl url){
    qDebug()<<_path<<_filename<<"下载完成 !";
    //qDebug()<<"file path  :"<<_path;
    int i;
    //找到这个下载任务对应的状态栏的编号。
    for(i =0;i<task.length();i++){
        if(task[i].first->TASK_ID == _task_id)
            break;
    }
    disconnect(task[i].first,SIGNAL(send_Ui_Msg(int,QString,qint64,qint64,QString,QString)),
            this,SLOT(upDateUI(int,QString,qint64,qint64,QString,QString)));

    //将已经下载完成的任务所对应的状态栏，从正在下载界面隐藏。
    hideAll(i);


    FinishedTools *tools = new FinishedTools(this);
    tools->url = url;
    tools->file_Name = _filename;
    tools->total_size = _totalSize;
    tools->path = _path;
    tools->filename.setText(_filename);
    tools->totalSize.setText(QString::number(_totalSize/1024)+ QString("Kb"));

    //当传递参数的信号被发送时，调用MainWindow类的槽函数去创建一个回收站状态栏对象。
    connect(tools,SIGNAL(moveToRecycle(QUrl,QString,QString,qint64)),
            this,SLOT(Move_To_Recycle(QUrl,QString,QString,qint64)));

    //将这个对象的指针加入下载完成任务的队列，结束时可以析构掉。
    task_finish.append(tools);

    //在下载完成界面中，添加任务状态。
    Finished_layout->addWidget(&(tools->widget));
    qDebug()<<"Finished_layout->addWidget(&(tools->widget)) ok !";

}
void MainWindow::Move_To_Recycle(QUrl _url, QString _path, QString _filename, qint64 _totalSize){
    //创建一个回收站状态栏
    RecycleTools * tools = new RecycleTools(this);
    tools->file_Name = _filename;
    tools->path = _path;
    tools->total_size = _totalSize;
    tools->url = _url;
    tools->filename.setText(_filename);
    tools->totalSize.setText(QString::number(_totalSize));


    Recycle_layout->addWidget(&(tools->widget));
    task_recycle.append(tools);
    //当点击重新下载的时候，
    connect(tools,SIGNAL(reload_Dow(QUrl,QString)),this,SLOT(Reload_Downloading(QUrl,QString)));

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
bool MainWindow::Space_enough( qint64 totalSize,QString dir)
{
    std::string path = dir.toStdString();
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
    return (quint64)liTotalFreeBytes.QuadPart / totalSize;   //返回0无空间  返回1有空间

}
//判断磁盘空间不足的情况
bool MainWindow::findExistTask(QUrl _url,QString _fileDir){

    QString strAll;
    QStringList strList;
    if(configFile == NULL){
        qDebug()<<"configFile is NULL!  open error~";
        return  false;
    }
    if(!configFile->open(QFile::ReadOnly|QFile::Text))//打开配置文件,读取所有信息到 缓冲字符串strAll
    {
        QString errorInfo =configFile->errorString();
        configFile->close();
        configFile = NULL;
        qDebug()<<"Open file error in Change_ConfigFile() :"<<errorInfo;
        return  false;
    }
    else{
        QTextStream stream(configFile);
        strAll = stream.readAll();
        //读取完关闭文件，因为信息已经被保存到了变量中
        configFile->close();
    }
    strList = strAll.split("\n");
    for(int i =0;i<strList.count();i++)
    {
        if((strList.at(i) == _url.toString())&&(strList.at(i+1) == _fileDir))
        {
            return true;

        }
    }
    return false;

}
void MainWindow::Downloading_To_Recycle(QUrl _url,QString _path,QString _filename,qint64 _totalSize){
    //创建一个回收站状态栏
    RecycleTools * tools = new RecycleTools(this);
    tools->file_Name = _filename;
    tools->path = _path;
    tools->total_size = _totalSize;
    tools->url = _url;
    tools->filename.setText(_filename);
    tools->totalSize.setText(QString::number(_totalSize));
    Recycle_layout->addWidget(&(tools->widget));

    task_recycle.append(tools);
    //当点击重新下载的时候，


}
