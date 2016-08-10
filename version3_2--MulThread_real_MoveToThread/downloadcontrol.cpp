#include"downloadcontrol.h"

DownloadControl::DownloadControl(QObject *parent,int _TASK_ID):QObject(parent)
{

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

    if(file != NULL)
    {
        qDebug()<<" error file";
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

      //这个定时器只需要定时发送信号，让槽函数统计每一个线程的下载进度。应该只对NetSpeed()负责。
     timer = new QTimer(this);
     timer->start(1000);
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

         //DThread * thread = new DThread(this,i);

         QThread * thread = new QThread;
         Download *download = new Download(url,file,startPoint,endPoint,mutex,i);
         download->moveToThread(thread);
         thread->start();

         connect(thread, SIGNAL(started()),download, SLOT(StartDownload()));
         connect(download,SIGNAL(Finished_Thread()),thread,SLOT(quit()));
         connect(download,SIGNAL(Finished_Thread()),this,SLOT(SubPartFinished()));
         connect(thread,SIGNAL(finished()),this,SLOT(PrintThreadEnd()));

         connect(download,SIGNAL(quitThread()),thread,SLOT(quit()));
         connect(this,SIGNAL(pause_Sig()),download,SLOT(Thread_pauseDownload()));
         threads.append(download);

     }
     return;
}
/*
 //通过配置文件继续下载
void DownloadControl::DownloadFile(QString configFile){

}
*/

//一个线程下载任务完结的时候，会发送这个信号到 下载控制器，通知自己即将结束。。
void DownloadControl::PrintThreadEnd(){
        qDebug()<<"DownloadControl::Print_ThreadEnd()-->Thread is end.";
}

/*
 *  临时保存所有线程下载状态的数组
    qint64 startArray[50];
    qint64 newArray[50];
    qint64 endArray[50];
 *
*/

//暂停下载
void DownloadControl::pause(){
    qDebug()<<"Pause download....";
    disconnect(timer,SIGNAL(timeout()),this,SLOT(NetSpeed()));

    emit pause_Sig();//发送暂停信号，每一个线程都会接收到它

    timer->stop();   //暂停计时器

    for(int i =0;i<ThreadNum;i++){
        threads[i]->getMessage(startArray[i],newArray[i],endArray[i]);
    }               //将每一个线程的具体数据都读取出来，存到数组的相应位置中。

    file->close();  //统计完毕，关闭文件。

    for(int i = 0;i<ThreadNum;i++)
        threads[i]->deleteLater();
                     //析构所有download对象。

    /*********************打开配置文件并写入下载任务的当前状态信息************************/

    Write_To_ConfigFile();

    /*******************************************************************************/


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
       threads[i]->sendSpeed_leftSize(pair);
        speed += pair.first;
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
void DownloadControl::Write_To_ConfigFile(){

    QString strAll;
    QStringList strList;
    bool isFirstPause = true;
    if(!configFile->open(QFile::ReadOnly|QFile::Text))//打开配置文件,读取所有信息到 缓冲字符串strAll
    {   //打开失败
        QString errorInfo =configFile->errorString();
        configFile->close();
        configFile = NULL;
        qDebug()<<"Open file error in Change_ConfigFile() :"<<errorInfo;
        return ;
    }
    else{
        //打开成功
        QTextStream stream(configFile);
        strAll = stream.readAll();
        //读取完关闭文件，因为信息已经被保存到了变量中
        configFile->close();
    }
    //以截断模式打开配置文件，清空了原本配置文件中的内容。
    if(!configFile->open(QFile::WriteOnly|QFile::Text))//打开配置文件
    {
        QString errorInfo =configFile->errorString();
        configFile->close();
        configFile = NULL;
        qDebug()<<"Open file error in Change_ConfigFile() :"<<errorInfo;
        return ;
    }
    else{
        QTextStream stream(configFile);
        strList = strAll.split("\n");
        for(int i =0;i<strList.count();i++)
        {
            if(i == strList.count()-1)
            {
                //最后一行不需要换行
                stream<<strList.at(i);
            }
            else
            {
                if(strList.at(i).contains(url.toString())&&
                        strList.at(i+1).contains(FileDir))
                {//如果匹配到了url与目标路径都符合的条目，则修改条目下边的内容。


                    isFirstPause = false; //如果匹配到，则说明这个任务不是第一次点击暂停

                    stream<<strList.at(i)<<"\n";    //写入url
                    stream<<strList.at(i+1)<<"\n";  //写入路径

                    i += 2;//跳转到下下一行，下下一行内容为线程数。
                    QString tempStr = strList.at(i);//读取线程数存到字符串
                    stream<<tempStr<<"\n";
                    int Thread_Num = tempStr.toInt();//转换为整数int
                    for(int j = 0;j<Thread_Num;j++)
                    {
                        stream << startArray[j]<<"\n";
                        stream << newArray[j]<<"\n";
                        stream << endArray[j]<<"\n";
                    }
                    i =i + Thread_Num*3 ;
                }
                else{
                    stream<<strList.at(i)<<"\n";
                }
            }
        }
        if(isFirstPause == true){//如果是第一次暂停，则无法再上述循环中匹配到url，所以将任务信息添加到文本末尾。

            stream<<"\n";
            QString _url(url.toString());
            stream << _url<<"\n";
            stream << FileDir <<"\n";
            stream << ThreadNum<<"\n";
            for(int i = 0;i<ThreadNum;i++){

                stream << startArray[i]<<"\n";
                stream << newArray[i]<<"\n";
                stream << endArray[i]<<"\n";
            }
        }
         configFile->close();
    }

}

