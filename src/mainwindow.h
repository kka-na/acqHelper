#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCoreApplication>
#include <QObject>

#include <QListWidgetItem>
#include <QMessageBox>
#include <QWidget>
#include <QLabel>

#include <QFileDialog>
#include <QFile>
#include <QString>
#include <QDir>

#include <QDebug>
#include <QThread>
#include <QTimer>
#include <QMetaType>
#include <QStorageInfo>

#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>	

#include "jsoncpp/json/json.h"

#include "Timestamp.h" //basic header included

using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public: 
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
	Ui::MainWindow *ui;
    Timestamp ts;
    bool stop_flag=false;
    Json::Value informs;
    Json::Value inform;
    Json::Value traffic, special_traffic;
    string file_path, json_path;
    string date, scenario, region, time, weather, road, car, driver, assistant; 	
    double start, end, minute, hour, total_frame;
    string traffic_start_time, special_traffic_start_time;

    shared_ptr<QTimer> timer;

private:
    void setFunction();
    void setMovingLabel(int status);
    void makeStartJSON();
    void makeEndJSON();
    void addJSON(string traffic_type, string type_s, string start_s, string end_s);
    
    void stopMovingLabel();

private slots:
    void setUsage();

    void setStorage();
    void startRecord();
    void stopRecord();

    void startTraffic();
    void stopTraffic();
    void startSpecialTraffic();
    void stopSpecialTraffic();

    void movingLabel();

};
#endif //MAINWINDOW_H
