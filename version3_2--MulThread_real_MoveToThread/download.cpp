#include<download.h>

Download::Download(QObject *parent, int _ID)
    :Thread_ID(_ID)
{
    startBytes = endBytes = newSize = oldSize = 0;
    last_15s_Size =0;
    file = NULL;
    pair.first = 0;
    pair.second = 0;
    speed = 0;
    leftSize = 0;
    timer = new QTimer(this);
    timer->start(500);

  //  qDebug()<<"download part :"<<Thread_ID <<"created.";

}
Download::~Download()
{   
    delete manager;
}

void Download::StartDownload( QUrl url,
                         QFile* _file,
                         qint64 _startBytes,
                         qint64 _endBytes,
                         QMutex* _mutex)
{
    if( _file == NULL ){
        qDebug()<<"file is  not created!";
        return;
    }



    qDebug()<<"Thread "<<Thread_ID <<" start downloading.";

    startBytes = _startBytes;
    endBytes   = _endBytes;
    file       = _file;
    mutex      = _mutex;
    leftSize   = endBytes - startBytes;

    //这里不能加this，因为这个对象将会在从线程里被创建，而当前对象是在主线程里被创建的，跨线程指定父子关系，程序会报警。
    //manager = new QNetworkAccessManager(this);
    manager = new QNetworkAccessManager();

    QNetworkRequest qheader;
    qheader.setUrl(url);
    QString range;
    //根据HTTP协议，写入RANGE头部，说明请求文件的范围
    range.sprintf("Bytes=%lld-%lld",startBytes, endBytes);
    qheader.setRawHeader("Range", range.toUtf8());
    //qheader.setRawHeader("Range",tr("bytes=%1-%2").arg(startBytes,endBytes).toUtf8());

    //开始下载

    reply = manager->get(QNetworkRequest(qheader));

    connect(reply,SIGNAL(finished()),this,SLOT(httpFinished()));
    connect(reply,SIGNAL(readyRead()),this,SLOT(httpReadyRead()));

    connect(timer,SIGNAL(timeout()),this,SLOT(updateSpeed()));


    // qDebug() << "Download -->Thread " <<Thread_ID << " StartDownload().";
}


void Download:: updateSpeed(){
    speed = newSize - oldSize;
    oldSize = newSize;
}

void Download::sendSpeed_leftSize(pair_2int64 &_pair,int i){
    if(i == Thread_ID){
        pair.first = speed;
        pair.second = leftSize;
        _pair = pair;
        return ;
    }
    else{
        return ;
    }
}

void Download::httpFinished(){
    file->flush();
    speed = leftSize = 0;
    reply->deleteLater();
    reply = 0;
    file = 0;
    timer->stop();
    qDebug() << "httpFinished()--> Thread :" <<Thread_ID<< " download finished";

    emit Finished_Thread();
}

void Download::httpReadyRead(){
    if ( !file )
        return;
     mutex->lock();

     buffer = reply->readAll();
     /****************************/



     file->seek(startBytes+newSize);
     file->write(buffer);
     mutex->unlock();
     /****************************/
     //newSize 与 leftSize 始终是最新的.
     newSize += buffer.size();
     leftSize = endBytes - startBytes - newSize;
}












