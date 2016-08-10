#include"downloadcontrol.h"

DownloadControl::DownloadControl(QObject *parent,
                                 int _TASK_ID,
                                 QFile *_configFile,
                                 QUrl _url,
                                 QString _saveFile,
                                 QString _FileDir):QObject(parent)
{
    Thread_Finished_Num = 0;
    speed = time_left = 0;
    totalSize = readySize = leftSize =0;

    file = NULL;
    configFile = _configFile;
    url = _url;
    saveFile = _saveFile;  //应该改成 fileName
    FileDir = _FileDir;

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

void DownloadControl::DownloadFile(int _ThreadNum)
{
    if(file != NULL)
    {
        qDebug()<<" error file";
        return ;
    }
    ThreadNum = _ThreadNum;

     qDebug()<<"DownloadFile(int)-> ThreadNum :" <<ThreadNum;
     qDebug()<<"DownloadFile(int)-> url       :" <<url.toString();
     qDebug()<<"DownloadFile(int)-> saveFile  :" <<saveFile;

    totalSize = GetFileSize(url,3);//尝试获取文件大小，最多尝试三次,失败则返回-1
     qDebug()<<"totalSize  "<<totalSize;
    if(totalSize == -1)
    {
       // return false;
        qDebug()<<"totalSize == -1";
        return ;
    }
    //将dir保存起来，在下载完成后将当做信号参数进行传递。
    QString fulPath = FileDir+ "/"+ saveFile;
    file = new QFile(fulPath);
    if(!file->open(QFile::WriteOnly | QFile::Append))
    {
        errorInfo = "can not open file : \n" + file->errorString();
        file->close();
        file = NULL;
        qDebug()<<"errorInfo :"<<errorInfo;
        return ;
    }

    //threads是一个 vector<Download *> ，存放当前任务的各个线程的进度
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
         qint64 endPoint = (totalSize * (i + 1) / ThreadNum)-1;

         //DThread * thread = new DThread(this,i);

         QThread * thread = new QThread;
         Download *download = new Download(url,file,startPoint,endPoint,mutex,i);
         download->moveToThread(thread);
         thread->start();

         connect(thread, SIGNAL(started()),download, SLOT(StartDownload()));
         connect(download,SIGNAL(Finished_Thread()),thread,SLOT(quit()));
         connect(download,SIGNAL(Finished_Thread()),this,SLOT(SubPartFinished()));
         connect(thread,SIGNAL(finished()),this,SLOT(PrintThreadEnd()));

         //点击暂停按钮，暂停当前正在下载的任务，保存进度到配置文件。
         connect(this,SIGNAL(pause_Sig()),download,SLOT(Thread_pauseDownload()));
         //任务被暂停的时候，发送信号结束线程。但是要注意，这时候有些线程可能已经完成了下载任务，已经结束退出。
         connect(download,SIGNAL(quitThread()),thread,SLOT(quit()));

         threads.append(download);

     }
     /*****************************************/

     return;
}

 //通过配置文件继续下载
void DownloadControl::DownloadFile(void){

    totalSize = GetFileSize(url,3);
    QString fulpath = FileDir+ "/" +saveFile;
    file = new QFile(fulpath);
    qDebug()<<"DownloadFile(void)-> ThreadNum :" <<ThreadNum;
    qDebug()<<"DownloadFile(void)-> url       :" <<url.toString();
    qDebug()<<"DownloadFile(void)-> saveFile  :" <<saveFile;
    /**********************************修改*****************************************/

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
    //threads是一个 vector<Download *> ，存放当前任务的各个线程的进度
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

         qint64 startPoint = startArray[i];
         qint64 newSize    = newArray[i];
         qint64 endPoint   = endArray[i];

         QThread * thread = new QThread;
         Download *download = new Download(url,file,startPoint+newSize-1,endPoint,mutex,i);
         download->moveToThread(thread);
         thread->start();

         connect(thread, SIGNAL(started()),download, SLOT(StartDownload()));
         //connect(download,SIGNAL(Finished_Thread()),thread,SLOT(quit()));
         connect(download,SIGNAL(Finished_Thread()),this,SLOT(SubPartFinished()));
         connect(thread,SIGNAL(finished()),this,SLOT(PrintThreadEnd()));

         //点击暂停按钮，暂停当前正在下载的任务，保存进度到配置文件。
         connect(this,SIGNAL(pause_Sig()),download,SLOT(Thread_pauseDownload()));
         //任务被暂停的时候，发送信号结束线程。但是要注意，这时候有些线程可能已经完成了下载任务，已经结束退出。
         connect(download,SIGNAL(quitThread()),thread,SLOT(quit()));

         threads.append(download);

     }
     /*****************************************/
     return;

}


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
    //delete timer;

    for(int i =0;i<ThreadNum;i++){
        threads[i]->getMessage(startArray[i],newArray[i],endArray[i]);
    }               //将每一个线程的具体数据都读取出来，存到数组的相应位置中。

    file->close();  //统计完毕，关闭文件。

    for(int i = 0;i<ThreadNum;i++)
        threads[i]->deleteLater();
    threads.clear();
                     //析构所有download对象。

    /*********************打开配置文件并写入下载任务的当前状态信息************************/

    bool to;
    to = Write_To_ConfigFile();
    if(to == false){
        qDebug()<<"pause error!!! some file open error~";
        return ;
    }

    /*******************************************************************************/


}

//继续下载
void DownloadControl::startAgain(){
    qDebug()<<"Continue download....";
    /*********************从配置文件读取任务的暂停数据**************************/

    bool to;
    to = Read_From_ConfigFile();
    if(to == false){
        qDebug()<<"pause error!!! some file open error~";
        return;
    }

    /***********************************************************************/
    DownloadFile();

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


 //有线程完成下载的时候，会发送信号到这个函数，当所有线程都完成下载任务，则槽函数发送下载完成信号。
void DownloadControl::SubPartFinished()
{
    Thread_Finished_Num ++;


    //如果完成数等于文件段数，则说明文件下载完毕，关闭文件，发射信号
    if( Thread_Finished_Num == ThreadNum )
    {
        emit pause_Sig();
        file->close();
        timer->stop();
        qDebug() << "DownloadControl::SubPartFinished()--> Download finished";
        emit FileDownloadFinished(saveFile,TASK_ID,totalSize,FileDir);
        QString str1("");
        QString str2("下载完成");
        emit send_Ui_Msg(TASK_ID,saveFile,totalSize,totalSize,str1,str2);
        //delete this;
        for(int i = 0;i<ThreadNum;i++)
            threads[i]->deleteLater();
        threads.clear();

        /*****************删除配置文件中与此任务相关的信息********************/
        bool to =DelFrom_ConfigFile();
        if(to == false){
            qDebug()<<"pause error!!! some file open error~";
            return;
        }
        /****************************************************************/
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
bool DownloadControl::Write_To_ConfigFile(){

    QString strAll;
    QStringList strList;
    bool isFirstPause = true;
    if(!configFile->open(QFile::ReadOnly|QFile::Text))//打开配置文件,读取所有信息到 缓冲字符串strAll
    {   //打开失败
        QString errorInfo =configFile->errorString();
        configFile->close();
        configFile = NULL;
        qDebug()<<"Open file error in Change_ConfigFile() :"<<errorInfo;
        return  false;
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
        return  false;
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
    return true;

}
bool DownloadControl::Read_From_ConfigFile(){
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

    //因为是继续下载，只需要从配置文件中读取到数据就行，所以不需要再次打开文件进行写入，直接从读取到的
    //数据中提取目标信息就行。
    strList = strAll.split("\n");
    for(int i =0;i<strList.count();i++)
    {
        if(strList.at(i).contains(url.toString())&&
                strList.at(i+1).contains(FileDir))
        {//如果匹配到了url与目标路径都符合的条目，则读取下边的内容。
            i+=2;//跳转到下下一行，下下一行内容为线程数。
            QString tempStr = strList.at(i);//读取线程数存到字符串
            int Thread_Num = tempStr.toInt();//把读取到的线程数(字符串)转换为整数int
            ThreadNum = Thread_Num;
            for(int j = 0;j<Thread_Num;j++) //每一个线程都有三条数据（开始点，已下载量，结束点）
            {
                //读取每一个线程的三个数据，再将它们写入文件
                QString tempStr=strList.at(i+j*3+1);
                startArray[j] = tempStr.toLongLong();
                tempStr=strList.at(i+j*3+2);
                newArray[j] = tempStr.toLongLong();
                tempStr=strList.at(i+j*3+3);
                endArray[j] = tempStr.toLongLong();
            }
            i = i + Thread_Num*3 ;
        }
    }
    return true;
}
bool DownloadControl::DelFrom_ConfigFile(){

    QString strAll;
    QStringList strList;
    if(!configFile->open(QFile::ReadOnly|QFile::Text))//打开配置文件,读取所有信息到 缓冲字符串strAll
    {
        QString errorInfo =configFile->errorString();
        configFile->close();
        configFile = NULL;
        qDebug()<<"Open file error in Change_ConfigFile() :"<<errorInfo;
        return false;
    }
    else
    {
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
        return  false;
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
                {//如果匹配到了url与目标路径都符合的条目，则删除这个任务的一切信息。
                    i += 2;//跳转到下下一行，下下一行内容为线程数。
                    QString tempStr = strList.at(i);//读取线程数存到字符串
                    int Thread_Num = tempStr.toInt();//转换为整数int
                    i += Thread_Num*3;
                }
                else{
                    stream<<strList.at(i)<<"\n";
                }
            }
        }
         configFile->close();
    }
    return true;
}
