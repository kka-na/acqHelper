#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent):QMainWindow(parent),ui(new Ui::MainWindow){
	ui->setupUi(this);
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
	region = ui->regionCombo->currentText().toStdString();
	time = ui->timeCombo->currentText().toStdString();
	weather = ui->weatherCombo->currentText().toStdString();
	road = ui->roadCombo->currentText().toStdString();
	scenario = ui->scenarioCombo->currentText().toStdString();
	car = ui->carCombo->currentText().toStdString();
	driver = ui->driverCombo->currentText().toStdString();
	assistant = ui->assistantCombo->currentText().toStdString();

	informs.clear();
	inform.clear();
	this->makeJSON();

	timer = make_shared<QTimer>();
	connect(timer.get(), SIGNAL(timeout()), this, SLOT(moveCar()));
	timer->start(500);
}
void MainWindow::stopRecord(){
	//show json text
	informs.append(inform);
	Json::StyledWriter writer;
	ofstream out(json_path.c_str());
	out<<writer.write(informs);
	out.close();
	stopCar();
}

void MainWindow::startTraffic(){
	traffic_start_time = ts.getMilliTime();
}
void MainWindow::stopTraffic(){
	string type = ui->trafficCombo->currentText().toStdString();
	string traffic_end_time = ts.getMilliTime();
	this->addJSON("traffic", type, traffic_start_time, traffic_end_time);
	traffic_start_time = "";
}
void MainWindow::startSpecialTraffic(){
	special_traffic_start_time = ts.getMilliTime();
}
void MainWindow::stopSpecialTraffic(){
	string type = ui->specialTrafficCombo->currentText().toStdString();
	string special_traffic_end_time = ts.getMilliTime();
	this->addJSON("special_traffic", type, special_traffic_start_time, special_traffic_end_time);
	special_traffic_start_time = "";
}


void MainWindow::makeJSON(){
	json_path = file_path+ts.getMilliTime()+".json";
	ifstream in(json_path.c_str());
	if(in.is_open()) in >> informs;

	inform["date"] = date;
	inform["scenario"] = scenario;
	inform["region"] = region;
	inform["time"] = time;
	inform["weather"] = weather;
	inform["road"] = road;
	inform["car"] = car;
	inform["driver"] = driver;
	inform["assistant"] = assistant;
	inform["traffic"] = traffic;
	inform["special_traffic"] =traffic;
}

void MainWindow::addJSON(string traffic_type, string type_s, string start_s, string end_s){
	Json::Value traffics;

	traffics["type"] = type_s;
	traffics["start"] = start_s;
	traffics["end"] = end_s;

	if(traffic_type == "traffic"){
		traffic.append(traffics);
		inform["traffic"] = traffic;
	}else{
		special_traffic.append(traffics);
		inform["special_traffic"] =special_traffic;
	}
	traffics.clear();
}

void MainWindow::moveCar(){
 	if(ui->carImg->x() > 500){
		ui->carImg->move(0,330);
	}else{
		ui->carImg->move(ui->carImg->x()+10,330);
	}
}
void MainWindow::stopCar(){
	timer->stop();
}