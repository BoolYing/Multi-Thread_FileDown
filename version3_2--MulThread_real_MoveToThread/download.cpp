#include<download.h>

Download::Download(QUrl _url,
                   QFile *_file,
                   qint64 _startPoint,
                   qint64 _endPoint,
                   QMutex *_mutex,
                   int _ID)
    :Thread_ID(_ID)
{
    url = _url;
    startBytes = _startPoint;
    endBytes   = _endPoint;
    file       = _file;
    mutex      = _mutex;
    leftSize   = endBytes - startBytes;
    newSize = oldSize = 0;
    last_15s_Size =0;
    pair.first = 0;
    pair.second = 0;
    speed = 0;



    qDebug()<<"Thread "<<Thread_ID <<" created.";


}
Download::~Download()
{   
    if(manager != NULL){
        delete manager;
        manager = NULL;
    }
    qDebug()<<"Delete Success!!!!!" ;
}
//暂停当前线程的下载任务，并调用new的的对象的析构函数。
void Download::Thread_pauseDownload(){

    disconnect(reply,SIGNAL(finished()),this,SLOT(httpFinished()));
    disconnect(reply,SIGNAL(readyRead()),this,SLOT(httpReadyRead()));
    disconnect(timer,SIGNAL(timeout()),this,SLOT(updateSpeed()));
    if(reply){
        reply->abort();
        reply->deleteLater();
        reply = NULL;
        //qDebug()<<Thread_ID<<" delete success";
    }

    if(manager){
        delete manager;
        manager = NULL;
        //qDebug()<<Thread_ID<<" delete success";
    }

    if(timer){
        timer->stop();
        delete timer;
        timer = NULL;
        //qDebug()<<Thread_ID<<" delete success";
    }
    qDebug()<<Thread_ID<<" delete success";

    /*刷新文件缓冲区
    mutex->lock();
    file->flush();
    mutex->unlock();
    */

/*
    qDebug()<< "Thread_pauseDownload() -->"<<"Thread id :"<<Thread_ID
            <<"\nStartByets:" <<startBytes
            <<"\nnewSize   :"<<newSize
            <<"\nendBytes  :"<<endBytes;
            */
    emit quitThread();

}

void Download::StartDownload()
{
    if( file == NULL ){
        qDebug()<<"file is  not created!";
        return;
    }

    timer = new QTimer;
    timer->start(990);

    qDebug()<<"Thread "<<Thread_ID <<" start downloading.";

    manager = new QNetworkAccessManager();

    QNetworkRequest qheader;
    qheader.setUrl(url);
    QString range;
    //根据HTTP协议，写入RANGE头部，说明请求文件的范围
    range.sprintf("Bytes=%lld-%lld",startBytes, endBytes);
    qheader.setRawHeader("Range", range.toUtf8());

    //开始下载

    reply = manager->get(QNetworkRequest(qheader));

    connect(reply,SIGNAL(finished()),this,SLOT(httpFinished()));
    connect(reply,SIGNAL(readyRead()),this,SLOT(httpReadyRead()));

    connect(timer,SIGNAL(timeout()),this,SLOT(updateSpeed()));


    //qDebug() << "Download -->Thread " <<Thread_ID << " StartDownload().";
}

void Download:: updateSpeed(){
    speed = newSize - oldSize;
    oldSize = newSize;
}

void Download::sendSpeed_leftSize(pair_2int64 &_pair){
        pair.first = speed;
        pair.second = leftSize;
        _pair = pair;
        return ;
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


void Download::getMessage(qint64 & _startPoint, qint64 & _newSize, qint64 &_endPoint){
    _startPoint = startBytes;
    _newSize    = newSize;
    _endPoint   = endBytes;
}









