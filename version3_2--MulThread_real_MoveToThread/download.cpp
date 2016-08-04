#include<download.h>

Download::Download(int _ID)
    : Thread_ID(_ID)
{
    startBytes = endBytes = newSize = oldSize = 0;
    last_15s_Size =0;
    file = NULL;
    pair.first = 0;
    pair.second = 0;
    speed = 0;
    leftSize = 0;
    timer = new QTimer();
    timer->start(500);

  //  qDebug()<<"download part :"<<Thread_ID <<"created.";

}
Download::~Download(){
    if(manager){
    delete manager;
    }
    if(timer){
        delete timer;
    }

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

    //manager = _manager;


    qDebug()<<"Thread "<<Thread_ID <<" start downloading.";

    startBytes = _startBytes;
    endBytes   = _endBytes;
    file       = _file;
    mutex      = _mutex;
    leftSize   = endBytes - startBytes;

    //根据HTTP协议，写入RANGE头部，说明请求文件的范围
    manager = new QNetworkAccessManager();
    QNetworkRequest qheader;
    qheader.setUrl(url);
    QString range;
    range.sprintf("Bytes=%lld-%lld",startBytes, endBytes);
    qheader.setRawHeader("Range", range.toUtf8());
    //qheader.setRawHeader("Range",tr("bytes=%1-%2").arg(startBytes,endBytes).toUtf8());

    //开始下载

    reply = manager->get(QNetworkRequest(qheader));

    connect(reply,SIGNAL(finished()),this,SLOT(httpFinished()));
    connect(reply,SIGNAL(readyRead()),this,SLOT(httpReadyRead()));

    connect(timer,SIGNAL(timeout()),this,SLOT(updateSpeed()));

    //connect(reply,SIGNAL(error(QNetworkReply::NetworkError)),SLOT(errorSlot(QNetworkReply::NetworkError)));
    //connect(this,SIGNAL(httpFinished()),this,SLOT());


    // qDebug() << "Download -->Thread " <<Thread_ID << " StartDownload().";
}


void Download:: updateSpeed(){
    speed = newSize - oldSize;
    oldSize = newSize;
    //pair.first = speed;
    //pair.second = leftSize;
    //emit getSpeed_leftSize(pair);
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
    qDebug() << "httpFinished()--> " <<Thread_ID<< " download finished";

    emit Finished_Thread();
}

void Download::httpReadyRead(){
    if ( !file )
        return;
     buffer = reply->readAll();
     /****************************/

     mutex->lock();
     file->seek(startBytes+newSize);
     file->write(buffer);
     mutex->unlock();
     /****************************/
     //newSize 与 leftSize 始终是最新的.
     newSize += buffer.size();
     leftSize = endBytes - startBytes - newSize;
}












