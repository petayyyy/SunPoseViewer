#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->calculateButton, &QPushButton::clicked, this, &MainWindow::calculateSunPosition);
    connect(ui->useSystemTimeCheckBox, &QCheckBox::toggled, this, &MainWindow::toggleDateTimeInput);
    connect(ui->gridCheckBox, &QCheckBox::toggled, ui->sunWidget, &SunWidget::toggleGrid);
    ui->useSystemTimeCheckBox->setChecked(true);
    toggleDateTimeInput(true);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::toggleDateTimeInput(bool useSystem) {
    ui->dateTimeEdit->setEnabled(!useSystem);
}

void MainWindow::calculateSunPosition() {
    double lat = ui->latitudeSpinBox->value();
    double lon = ui->longitudeSpinBox->value();
    double alt = ui->altitudeSpinBox->value();
    QDateTime dt = ui->useSystemTimeCheckBox->isChecked()
                   ? QDateTime::currentDateTimeUtc()
                   : ui->dateTimeEdit->dateTime().toUTC();

    auto coords = calculator.getHorizontalCoordinates(lat, lon, alt, dt);
    if (!coords.valid) {
        QMessageBox::warning(this, "Ошибка расчёта", "Неверные параметры или сбой вычислений.");
        return;
    }
    ui->azimuthLabel->setText(QString::number(coords.azimuth, 'f', 2));
    ui->altitudeLabel->setText(QString::number(coords.altitude, 'f', 2));
    ui->sunWidget->setCoords(coords.azimuth, coords.altitude);
}