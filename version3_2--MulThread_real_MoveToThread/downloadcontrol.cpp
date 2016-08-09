#include"downloadcontrol.h"

//暂时没有传MainWindow的指针，还没有做同步界面进度的功能。
//DownloadControl::DownloadControl(MainWindow * _window, QObject * parent = 0):window(_window),QObject(parent)
DownloadControl::DownloadControl(QObject *parent,int _TASK_ID):QObject(parent)
{
   //manager = new QNetworkAccessManager(this);
    Thread_Finished_Num = 0;
    speed = time_left = 0;
    totalSize = readySize = leftSize =0;
    state = Waiting;
    file = NULL;

    mutex = new QMutex;
    //初始化当前下载管理器的任务编号.
    TASK_ID = _TASK_ID;

}

DownloadControl::~DownloadControl(){
    if(mutex){
        delete mutex;
        mutex = NULL;
    }
    /*
    for(int i =0;i<ThreadNum;i++)
    {
        if(threads[i]){
            delete threads[i];
        }
    }
    */
}

QString DownloadControl::errorString()
{
    return errorInfo;

}

void DownloadControl::DownloadFile(QUrl url,
                                   QString saveFile,
                                   int ThreadNum,
                                   QString dir,
                                   QFile *_configFile)
{
    configFile = _configFile;
    //标志当前状态。
    //if(state == )
    if(file != NULL)
    {
        errorInfo = "unknow error";
        qDebug()<<" errorInfo :" <<errorInfo;
        //emit error(0,errorInfo);
        return ;
    }
    this->ThreadNum = ThreadNum;
    this->url = url;
    this->saveFile = saveFile;
     qDebug()<<"DownloadFile-> ThreadNum :" <<ThreadNum;
     qDebug()<<"DownloadFile-> url       :" <<url;
     qDebug()<<"DownloadFile-> saveFile  :" <<saveFile;

    totalSize = GetFileSize(url,5);
     qDebug()<<"totalSize  "<<totalSize;
    if(totalSize == -1)
    {
       // return false;
        qDebug()<<"totalSize == -1";
        return ;
    }
    //将dir保存起来，在下载完成后将当做信号参数进行传递。
    FileDir = dir;
    dir = dir + '/'+ saveFile;
    file = new QFile(dir);
    if(!file->open(QFile::WriteOnly | QFile::Append))
    {
        errorInfo = "can not open file : \n" + file->errorString();
        file->close();
        file = NULL;
        qDebug()<<"errorInfo :"<<errorInfo;
        return ;
    }
    //重置文件大大小为totalSize
    file->resize(totalSize);
    //threads是一个 vector<DThread *> ，存放当前任务的各个线程的信息。
    //清空threads
     threads.clear();

     timer = new QTimer(this);
     timer->start(1000);

      //这个定时器只需要定时发送信号，让槽函数统计每一个线程的下载进度。应该只对NetSpeed()负责。
     connect(timer,SIGNAL(timeout()),this,SLOT(NetSpeed()));
     //暂时未开启网络故障检测功能。
     //timer_15s = new QTimer(this);
     //timer_15s->start(15000);

     /*****************************************/
     for(int i = 0;i < ThreadNum;i++)
     {

         //qDebug()<<" for() is in Thread :"<< i;
         qint64 startPoint = totalSize * i / ThreadNum;
         qint64 endPoint = totalSize * (i + 1) / ThreadNum;

         DThread * thread = new DThread(this,i);
         thread->SetInitValue(url,file,startPoint,endPoint,mutex);
         threads.append(thread);

         thread->start();

        //qDebug()<<"thread :"<<thread<<"  thread->download :"<<thread->download;

         //主线程通过信号将引用类型的参数传递到对应的从线程，从线程通过改变这个引用类型的参数，来达到跨线程发送接收数据。
         connect(this,SIGNAL(getPair(pair_2int64&,int)),thread->download,
                SLOT(sendSpeed_leftSize(pair_2int64&,int)));

         //下载任务完成，发送信号到主线程的槽函数。
         connect(thread->download,SIGNAL(Finished_Thread()),this,SLOT(SubPartFinished()));

         //应该在每一个线程内部，都设置一个定时器，独立的更新自己的数据，并且应该实时的更新自己已经下载的字节大小。
         //已经修正，目前每一个下载任务都自带一个计时器。
         //connect(timer,SIGNAL(timeout()),thread->download,SLOT(updateSpeed()));

         //从线程结束，发送信号到主线程。
         //connect(thread,SIGNAL(finished()),this,SLOT(PrintThreadEnd()));


     }

     //下载状态
     //state = Downloading;
     return;
}

//一个线程下载任务完结的时候，会发送这个信号到 下载控制器，通知自己即将结束。。
void DownloadControl::PrintThreadEnd(){
    //qDebug()<<"DownloadControl::Print_ThreadEnd()-->Thread is end.";
}

//暂停下载
void DownloadControl::pause(){
    qDebug()<<"Pause download....";

    for(int i =0;i<ThreadNum;i++){
        threads[i]->getMessage(startArray[i],newArray[i],endArray[i]);

    }


}

//继续下载
void DownloadControl::startAgain(){
    qDebug()<<"Continue download....";
}

void DownloadControl::NetSpeed(){
    speed = 0;
    leftSize = 0;

    for(int i =0;i < ThreadNum;i++){
        //会将pair的引用，与目标线程编号当做信号参数传递，只有目标线程，才会改变这个引用的数值。
        emit getPair(pair,i);
        speed += (pair.first)*2;
        leftSize += pair.second;
     }

    //求出当前网络速度，与计算剩余时间大小，需要考虑如果下载速度为0的时候可能会造成的除0错误。
    QString str1,str2;
    str1 = QString::number(speed/1024,10)+" K/s";
    if(speed != 0){
        time_left = leftSize/speed;
        str2 = QString::number(time_left,10)+" s";
     }
    else  if(leftSize != 0 ){
            str2 = "暂无网络.";
        }
    qDebug()<<"speed = "<<str1<<"  time  =" <<str2;

    //每一个任务都有一个相对应的进度栏，包括进度条，文件名，下载速度，剩余时间等。
    //这个信号用来更新任务对应的进度栏。
    emit send_Ui_Msg(TASK_ID,saveFile,totalSize-leftSize,totalSize,str1,str2);


}

/*
 //通过配置文件继续下载
void DownloadControl::DownloadFile(QString configFile){

}
*/

 //有线程完成下载的时候，会发送信号到这个函数，当所有线程都完成下载任务，则槽函数发送下载完成信号。
void DownloadControl::SubPartFinished()
{
    Thread_Finished_Num ++;


    //如果完成数等于文件段数，则说明文件下载完毕，关闭文件，发射信号
    if( Thread_Finished_Num == ThreadNum )
    {
        file->close();
        timer->stop();
        qDebug() << "DownloadControl::SubPartFinished()--> Download finished";
        emit FileDownloadFinished(saveFile,TASK_ID,totalSize,FileDir);
        QString str1("");
        QString str2("下载完成");
        emit send_Ui_Msg(TASK_ID,saveFile,totalSize,totalSize,str1,str2);
        //delete this;
    }
}


//通过url获取目标文件的大小。
qint64 DownloadControl::GetFileSize(QUrl url,int tryTimes)
{
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


