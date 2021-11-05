#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent):QMainWindow(parent),ui(new Ui::MainWindow){
	ui->setupUi(this);
	this->setMovingLabel(0);
	this->setUsage();
	this->setFunction();
}

MainWindow::~MainWindow(){
	delete ui;
}

void MainWindow::setUsage(){
	//Setting Date
	QString date = QString::fromLocal8Bit(ts.getDate().c_str());
	ui->dateLabel->setText(date);
	//Setting Memory
	QStorageInfo storage=QStorageInfo::root();
	double total=(storage.bytesTotal()/1024/1024/1024);
    double free=(storage.bytesAvailable()/1024/1024/1024);
    double used=total-free;
    int percent=static_cast<int>((used/total)*100);
    ui->storageProgress->setValue(percent);
}

void MainWindow::setFunction(){
	connect(ui->dataStorageSearch, SIGNAL(clicked()), this, SLOT(setStorage()));
	connect(ui->recordingStart, SIGNAL(clicked()), this, SLOT(startRecord()));
	connect(ui->recordingStop, SIGNAL(clicked()), this, SLOT(stopRecord()));
	connect(ui->trafficStart, SIGNAL(clicked()), this, SLOT(startTraffic()));
	connect(ui->trafficStop, SIGNAL(clicked()), this, SLOT(stopTraffic()));
	connect(ui->specialTrafficStart, SIGNAL(clicked()), this, SLOT(startSpecialTraffic()));
	connect(ui->specialTrafficStop, SIGNAL(clicked()), this, SLOT(stopSpecialTraffic()));
}

void MainWindow::setStorage(){
	QString save_data_dir = QFileDialog::getExistingDirectory(this, "Select Directory to Stroing Information Data", QDir::currentPath(),QFileDialog::ShowDirsOnly);
    file_path = save_data_dir.toStdString();
	file_path += "/";
	ui->dataStorageLabel->setText(QString::fromLocal8Bit(file_path.c_str()));
}
void MainWindow::startRecord(){
	date = ts.getDate().c_str();
	start = stol(ts.getMilliTime().c_str());
	region = ui->regionCombo->currentText().toStdString();
	time = ui->timeCombo->currentText().toStdString();
	weather = ui->weatherCombo->currentText().toStdString();
	road = ui->roadCombo->currentText().toStdString();
	scenario = ui->scenarioCombo->currentText().toStdString();
	car = ui->carCombo->currentText().toStdString();
	driver = ui->driverCombo->currentText().toStdString();
	assistant = ui->assistantCombo->currentText().toStdString();

	informs.clear(); inform.clear(); traffic.clear(); special_traffic.clear();
	this->makeStartJSON();

	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(movingLabel()));
	timer->start(500);
}


double MainWindow::calculateTime(long start_time, long end_time){
    long start_second, end_second;
    start_second = (start_time/1000000000)*24*60*60 + ((start_time%1000000000)/10000000)*60*60 + ((start_time%10000000)/100000)*60 + start_time%100000;
	end_second = (end_time/1000000000)*24*60*60 + ((end_time%1000000000)/10000000)*60*60 + ((end_time%10000000)/100000)*60 + end_time%100000;
    int sec = end_second - start_second;
    return (double)sec/1000;
}

void MainWindow::stopRecord(){
	end = stol(ts.getMilliTime().c_str());
	long sstart, eend; 
	sstart = long(start)%10000000000;
	eend = long(end)%10000000000;
	double second = calculateTime(sstart, eend);
	minute = second/60;
	hour = minute/60;
	int fps = 0;
	if(ui->scenarioCombo->currentIndex() == 0)
		fps = 2;
	else if(ui->scenarioCombo->currentIndex() == 1)
		fps = 5;
	else if(ui->scenarioCombo->currentIndex() == 2)
		fps = 10;
	total_frame = minute*60*fps;

	this->makeEndJSON();
	stopMovingLabel();
}


void MainWindow::startTraffic(){
	this->setMovingLabel(1);
	traffic_start_time = ts.getMilliTime();
}
void MainWindow::stopTraffic(){
	this->setMovingLabel(0);
	string type = ui->trafficCombo->currentText().toStdString();
	string traffic_end_time = ts.getMilliTime();
	this->addJSON("traffic", type, traffic_start_time, traffic_end_time);
	traffic_start_time = "";
}
void MainWindow::startSpecialTraffic(){
	this->setMovingLabel(2);
	special_traffic_start_time = ts.getMilliTime();
}
void MainWindow::stopSpecialTraffic(){
	this->setMovingLabel(0);
	string type = ui->specialTrafficCombo->currentText().toStdString();
	string special_traffic_end_time = ts.getMilliTime();
	this->addJSON("special_traffic", type, special_traffic_start_time, special_traffic_end_time);
	special_traffic_start_time = "";
}


void MainWindow::makeStartJSON(){
	json_path = file_path+ts.getMilliTime()+"_"+getScenarioString()+".json";
	ifstream in(json_path.c_str());
	if(in.is_open()) in >> informs;

	inform["날짜"] = date;
	inform["시작"] = to_string(long(start));
	inform["시나리오"] = scenario;
	inform["지역"] = region;
	inform["시간대"] = time;
	inform["계절"] = weather;
	inform["도로"] = road;
	inform["차량"] = car;
	inform["운전자"] = driver;
	inform["동승자"] = assistant;
	inform["교통상황"] = traffic;
	inform["특수교통상황"] =traffic;
}

void MainWindow::makeEndJSON(){
	inform["끝"] = to_string(long(end));
	inform["총프레임"] = to_string(int(total_frame));
	inform["총분"] = to_string(minute);
	inform["총시간"] = to_string(hour);

	informs.append(inform);
	Json::StyledWriter writer;
	ofstream out(json_path.c_str());
	out<<writer.write(informs);
	out.close();
}

void MainWindow::addJSON(string traffic_type, string type_s, string start_s, string end_s){
	Json::Value traffics;

	traffics["종류"] = type_s;
	traffics["시작"] = start_s;
	traffics["끝"] = end_s;

	if(traffic_type == "traffic"){
		traffic.append(traffics);
		inform["교통상황"] = traffic;
	}else{
		special_traffic.append(traffics);
		inform["특수교통상황"] =special_traffic;
	}
	traffics.clear();
}


//show process of acquistion
void MainWindow::setMovingLabel(int status){
	if(status == 1){
		ui->movingLabel->setStyleSheet("QLabel {background-color: rgb(94, 226, 177);}");
	}else if(status == 2){
		ui->movingLabel->setStyleSheet("QLabel {background-color: rgb(255, 84, 113);}");
	}else{
		ui->movingLabel->setStyleSheet("QLabel {background-color: rgb(153, 127, 255);}");
	}
}

void MainWindow::movingLabel(){
	int tick = 500/60;
 	if(ui->movingLabel->x() > 500){
		ui->movingLabel->move(0,330);
	}else{
		ui->movingLabel->move(ui->movingLabel->x()+tick,330);
	}
}
void MainWindow::stopMovingLabel(){
	timer->stop();
}

string MainWindow::getScenarioString(){
	int time0, time1, time2; 
	int road0, road1, road2, road3, road4, road5, road6, road7;
	int time, road, weather; 
	string pre = "0";

	if(ui->roadCombo->currentIndex() == 5 || ui->roadCombo->currentIndex() == 6 || ui->roadCombo->currentIndex() == 7 ){
		time0 = 0; time1 = 15; road5= 0; road6 = 5; road7 = 10;
		if(ui->timeCombo->currentIndex() == 0)
			time = time0; 
		else if(ui->timeCombo->currentIndex() == 1)
			time = time1;
		else if(ui->timeCombo->currentIndex() == 2)
			time = time1;
		if(ui->roadCombo->currentIndex() == 5)
			road = road5; 
		if(ui->roadCombo->currentIndex() == 6)
			road = road6;
		if(ui->roadCombo->currentIndex() == 7)
			road = road7;
		pre = "S0";
	}else{
		time0 = 0; time1 = 22; time2 = 44; road0 = 0; road1 = 5; road2 = 10; road3 = 15; road4 = 20;
		if(ui->timeCombo->currentIndex() == 0)
			time = time0; 
		else if(ui->timeCombo->currentIndex() == 1)
			time = time2;
		else if(ui->timeCombo->currentIndex() == 2)
			time = time1;
		if(ui->roadCombo->currentIndex() == 0)
			road = road0; 
		else if(ui->roadCombo->currentIndex() == 1)
			road = road1;
		else if(ui->roadCombo->currentIndex() == 2)
			road = road2;
		else if(ui->roadCombo->currentIndex() == 3)
			road = road3;
		else if(ui->roadCombo->currentIndex() == 4)
			road = road4;
	}
	weather = ui->weatherCombo->currentIndex();

	int scene =  time + road + weather + 1;
	string scenes = pre+to_string(scene);
	return scenes;
}