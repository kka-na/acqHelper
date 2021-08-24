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
	start = stod(ts.getMilliTime().c_str());
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

	timer = make_shared<QTimer>();
	connect(timer.get(), SIGNAL(timeout()), this, SLOT(movingLabel()));
	timer->start(500);
}
void MainWindow::stopRecord(){
	end = stod(ts.getMilliTime().c_str());
	minute = (end-start)*0.00001;
	hour = minute/60;
	total_frame = minute*120;

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
	json_path = file_path+ts.getMilliTime()+".json";
	ifstream in(json_path.c_str());
	if(in.is_open()) in >> informs;

	inform["날짜"] = date;
	inform["시작"] = to_string(start);
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
	inform["끝"] = to_string(end);
	inform["총프레임"] = to_string(total_frame);
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
 	if(ui->movingLabel->x() > 500){
		ui->movingLabel->move(0,330);
	}else{
		ui->movingLabel->move(ui->movingLabel->x()+5,330);
	}
}
void MainWindow::stopMovingLabel(){
	timer->stop();
}