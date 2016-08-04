
#include"dthread.h"

DThread::DThread(int _ID)
{
     ID = _ID;
   // qDebug()<<"Thread " << ID <<" created.";
    download = NULL;

}
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

    //qDebug()<<"Thread "<<ID <<" Task Domaining";
}
void DThread::changeTag(){
    tag = true;
}

void DThread::run(){


    //download = new Download(ID);


    download->StartDownload(url,file,startPoint,endPoint,mutex);

    //connect(download,SIGNAL(Finished_Thread()),this,SLOT(quit()));
    connect(download,SIGNAL(Finished_Thread()),this,SLOT(changeTag()));

    // QEventLoop loop;

    while(!tag){
        QCoreApplication::processEvents();

    }
    //qDebug()<<"Thread "<<ID<<"Tag changed !";

    //connect(download,SIGNAL(Finished_Thread()),&loop,SLOT(exit()));
    //connect(this,SIGNAL(finished()),&loop,SLOT(quit()));
    //connect(timer,SIGNAL(timeout()),download,SLOT(updateSpeed()));



    //qDebug()<<"Thread "<<ID <<"is keep runing.";

    //loop.exec();
    //qDebug()<<"loop exec";
     //exec();
    //return ;
  }

