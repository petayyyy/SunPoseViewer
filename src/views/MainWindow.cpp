#include "MainWindow.h"
#include <QDateTime>
#include <QDebug>
#include <QTime>
#include "SunPositionCalculator.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    ui.setupUi(this);
    
    // Установка текущей даты и времени
    ui.dateTimeEdit->setDateTime(QDateTime::currentDateTime());
    
    // Подключение сигналов к слотам
    connect(ui.buttonCalculate, &QPushButton::clicked, this, &MainWindow::onCalculateClicked);
    connect(ui.checkUseSystemTime, &QCheckBox::stateChanged, this, &MainWindow::onUseSystemTimeChanged);
    connect(ui.checkShowGrid, &QCheckBox::stateChanged, this, &MainWindow::onShowGridChanged);
    connect(ui.checkUseSystemLocation, &QCheckBox::toggled, this, &MainWindow::onUseSystemLocationChanged);
    
    // Инициализация SunWidget
    ui.openGLWidget->setUserLocation(0, 0);
    ui.openGLWidget->setShowGrid(true);
    
    // Соединение кнопки показа результатов
    connect(ui.buttonShowResults, &QPushButton::clicked, [this]() {
        resultsWindow.show();
    });
}

void MainWindow::onUseSystemLocationChanged(bool checked) {
    // Enable or disable manual spin boxes depending on checkbox
    ui.spinLatitude->setEnabled(!checked);
    ui.spinLongitude->setEnabled(!checked);

    if (checked) {
        // Use Positioner to get system coordinates
        if (positioner.hasValidCoordinates()) {
            gpsErrorFlag = false;
            QGeoCoordinate coord = positioner.getCoordinates();
            ui.spinLatitude->setValue(coord.latitude());
            ui.spinLongitude->setValue(coord.longitude());
        } else {
            //no valid coords => we possibly need an update
            if (!positioner.updateCoordinates()) {
                qWarning() << "Failed to start position update";
                gpsErrorFlag = true;
            }
            //and we need to re-enable manual input
            ui.checkUseSystemLocation ->setEnabled(false);
        }
    }
    else {
        if(gpsErrorFlag) {
            QMessageBox::warning(this, "Ошибка позиционирования", " Автоматическое позиционирование недоступно.\n Пожалуйста, введите координаты вручную или повторите позже.");
        }
    }
}

void MainWindow::onCalculateClicked() {
    double lon = ui.spinLatitude->value();
    double lat = ui.spinLongitude->value();
    bool useSystemTime = ui.checkUseSystemTime->isChecked();
    
    QDateTime datetime;
    if (useSystemTime) {
        datetime = QDateTime::currentDateTime();
    } else {
        datetime = ui.dateTimeEdit->dateTime();
    }
    
    SunPositionCalculator calculator;

    // Настройка наблюдателя
    SunPositionCalculator::ObserverData obs = {
        .latitude_deg = lat,
        .longitude_deg = lon
    };
    calculator.setObserver(obs);

    QTime time = datetime.time();
    QDate date = datetime.date();
    // Расчет
    calculator.calculate(date.year(), date.month(), date.day(), time.hour());
    auto result = calculator.getResult();

    // Вывод результата
    std::cout << "  Calculated Azimuth: " << result.azimuth_deg << " deg\n"
              << "  Time: " << date.year() << " y " << date.month() << " m " << date.day() << " d " << time.hour() << "h\n"
              << "  Calculated Altitude: " << result.altitude_deg << " deg\n";

    // Формирование фиктивных данных для демонстрации
    std::vector<SunPosition> path = {
        {30.0f, 10.0f},
        {60.0f, 45.0f},
        {90.0f, 60.0f}
    };
    
    SunPosition current = {static_cast<float>(result.azimuth_deg), static_cast<float>(result.altitude_deg)};
    
    // Формирование данных для таблицы
    std::vector<std::tuple<QTime, double, double>> positions;
    QTime startTime(6, 0);
    for (size_t i = 0; i < path.size(); ++i) {
        positions.emplace_back(
            startTime.addSecs(i * 3600), 
            path[i].altitude, 
            path[i].azimuth
        );
    }
    
    // Обновление результатов
    resultsWindow.setResults(
        "06:00", 
        "18:00", 
        current.azimuth, 
        current.altitude,
        positions,
        lon, 
        lat, 
        datetime
    );
    
    // Активация кнопки показа результатов
    ui.buttonShowResults->setEnabled(true);
    
    // Обновление 3D визуализации
    ui.openGLWidget->setSunPath(path);
    ui.openGLWidget->setCurrentSunPosition(current);
    ui.openGLWidget->setUserLocation(lat, lon);
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