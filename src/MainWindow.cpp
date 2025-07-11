#include "MainWindow.h"
#include <QDateTime>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    ui.setupUi(this);
    
    // Установка текущей даты и времени
    ui.dateTimeEdit->setDateTime(QDateTime::currentDateTime());
    
    // Подключение сигналов к слотам
    connect(ui.buttonCalculate, &QPushButton::clicked, this, &MainWindow::onCalculateClicked);
    connect(ui.checkUseSystemTime, &QCheckBox::stateChanged, this, &MainWindow::onUseSystemTimeChanged);
    connect(ui.checkShowGrid, &QCheckBox::stateChanged, this, &MainWindow::onShowGridChanged);
    
    // Инициализация SunWidget
    ui.openGLWidget->setUserLocation(0, 0);
    ui.openGLWidget->setShowGrid(true);
}

void MainWindow::onCalculateClicked() {
    double lat = ui.spinLatitude->value();
    double lon = ui.spinLongitude->value();
    int alt = ui.spinAltitude->value();
    bool useSystemTime = ui.checkUseSystemTime->isChecked();
    
    QDateTime datetime;
    if (useSystemTime) {
        datetime = QDateTime::currentDateTime();
    } else {
        datetime = ui.dateTimeEdit->dateTime();
    }
    
    qDebug() << "Calculating for:";
    qDebug() << "Latitude:" << lat;
    qDebug() << "Longitude:" << lon;
    qDebug() << "Altitude:" << alt;
    qDebug() << "DateTime:" << datetime;
    
    // Пример установки данных в виджет
    std::vector<SunPosition> path = {
        {30.0f, 10.0f},
        {60.0f, 45.0f},
        {90.0f, 60.0f}
    };
    
    SunPosition current = {lat, lon};
    
    ui.openGLWidget->setSunPath(path);
    ui.openGLWidget->setCurrentSunPosition(current);
    ui.openGLWidget->setSunriseSunsetTimes("06:00", "18:00");
    ui.openGLWidget->setUserLocation(lat, lon);
}

void MainWindow::onUseSystemTimeChanged(int state) {
    bool enabled = (state != Qt::Checked);
    ui.dateTimeEdit->setEnabled(enabled);
}

void MainWindow::onShowGridChanged(int state) {
    bool show = (state == Qt::Checked);
    ui.openGLWidget->setShowGrid(show);
}