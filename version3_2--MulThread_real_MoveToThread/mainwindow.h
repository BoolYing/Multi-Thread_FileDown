#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include"downloadcontrol.h"
#include"download.h"
#include"dthread.h"

#include <QMainWindow>
#include<QNetworkAccessManager>
#include<QNetworkReply>
#include<QNetworkRequest>
#include<QDebug>
#include<QFile>
#include<QUrl>
#include<QString>
#include<QPair>

#include<QTabWidget>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
     void TaskFinished();

     void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
    DownloadControl *dow;

};

#endif // MAINWINDOW_H
