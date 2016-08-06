
#include"dthread.h"

DThread::DThread(int _ID)
{
     ID = _ID;
   // qDebug()<<"Thread " << ID <<" created.";
    download = NULL;

}
DThread::~DThread(){
    delete download;
}
//为线程设置下载任务的范围。
void DThread::SetInitValue(QUrl _url,
                           QFile *_file,
                           qint64 _startPoint,
                           qint64 _endPoint,
                           QMutex *_mutex){
    url =_url;
    file = _file;
    startPoint = _startPoint;
    endPoint = _endPoint;
    mutex = _mutex;
    tag = false;

    download = new Download(ID);
}


void DThread::changeTag(){
    tag = true;
}

void DThread::run(){

    //这个对象不能被创建在从线程中，必须得建立在主线程，也就是在类的构造函数中创建。
    //download = new Download(ID);


    download->StartDownload(url,file,startPoint,endPoint,mutex);

    //下载任务完成，会有一个信号被发送过来，改变tag的值为true.
    connect(download,SIGNAL(Finished_Thread()),this,SLOT(changeTag()));

    //不需要自己创建一个事件循环，QThread自带一个事件循环。
    // QEventLoop loop;

    //让线程进入死循环，一直到tag不为NULL，也即下载任务完成后，结束死循环，线程结束。
    while(!tag){
        QCoreApplication::processEvents();
    }

    //qDebug()<<"Thread "<<ID<<"Tag changed !";
    //qDebug()<<"Thread "<<ID <<"is keep runing.";

  }

