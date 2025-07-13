#include "MainWindow.h"
#include <QDateTime>
#include <QDebug>
#include <QTime>
#include "SunPositionCalculator.h"
#include <cmath>
#include <tuple>
#include <sstream>
#include <iomanip>
#include <string>

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

std::tuple<int, int> convertDecimalHours(double decimal_hours) {
    
    // Извлекаем целую часть как часы
    int hours = static_cast<int>(std::floor(decimal_hours));
    
    // Дробная часть часа в минуты
    double fractional_hour = decimal_hours - hours;
    double total_minutes = fractional_hour * 60.0;
    
    // Извлекаем минуты
    int minutes = static_cast<int>(std::floor(total_minutes));    
    return {hours, minutes};
}

// Функция преобразования местного времени в UTC
// Вход: часы (hours), минуты (minutes), широта (latitude), долгота (longitude)
// Выход: UTC-время в виде {часы, минуты}
std::tuple<int, int> localTimeToUTC(int hours, int minutes, double longitude) {
    // Рассчитываем часовой пояс по долготе (округление до ближайшего часа)
    double timezone_offset = ceil(longitude / 15.0);
    
    // Переводим время в минуты
    int total_minutes = hours * 60 + minutes;
    
    // Вычитаем смещение часового пояса (в минутах)
    int utc_minutes = total_minutes - static_cast<int>(timezone_offset) * 60;
    
    // Корректируем результат, если вышли за пределы [0, 24*60)
    utc_minutes %= (24 * 60);
    if (utc_minutes < 0) {
        utc_minutes += 24 * 60;
    }
    
    // Разделяем минуты обратно на часы и минуты
    return { utc_minutes / 60, utc_minutes % 60 };
}

std::string timeToString(int hours, int minutes) {
    char buffer[6]; // "hh:mm\0" — 6 символов
    snprintf(buffer, sizeof(buffer), "%02d:%02d", hours, minutes);
    return std::string(buffer);
}

std::tuple<int, int> UTCTimeToLocal(double decimal_hours, double longitude) {
    // Рассчитываем часовой пояс по долготе (округление до ближайшего часа)
    double timezone_offset = ceil(longitude / 15.0);
    auto [hours, minutes] = convertDecimalHours(decimal_hours);
    // Переводим время в минуты
    int total_minutes = hours * 60 + minutes;
    
    // Вычитаем смещение часового пояса (в минутах)
    int utc_minutes = total_minutes + static_cast<int>(timezone_offset) * 60;
    
    // Корректируем результат, если вышли за пределы [0, 24*60)
    utc_minutes %= (24 * 60);
    if (utc_minutes < 0) {
        utc_minutes += 24 * 60;
    }
    
    // Разделяем минуты обратно на часы и минуты
    return { utc_minutes / 60, utc_minutes % 60 };
}


void MainWindow::onUseSystemLocationChanged(bool checked) {
    // Enable or disable manual spin boxes depending on checkbox
    ui.spinLatitude->setEnabled(!checked);
    ui.spinLongitude->setEnabled(!checked);

    // if (checked) {
    //     // Use Positioner to get system coordinates
    //     if (positioner.hasValidCoordinates()) {
    //         gpsErrorFlag = false;
    //         QGeoCoordinate coord = positioner.getCoordinates();
    //         ui.spinLatitude->setValue(coord.latitude());
    //         ui.spinLongitude->setValue(coord.longitude());
    //     } else {
    //         //no valid coords => we possibly need an update
    //         if (!positioner.updateCoordinates()) {
    //             qWarning() << "Failed to start position update";
    //             gpsErrorFlag = true;
    //         }
    //         //and we need to re-enable manual input
    //         ui.checkUseSystemLocation ->setEnabled(false);
    //     }
    // }
    // else {
    //     if(gpsErrorFlag) {
    //         QMessageBox::warning(this, "Ошибка позиционирования", " Автоматическое позиционирование недоступно.\n Пожалуйста, введите координаты вручную или повторите позже.");
    //     }
    // }
}

void MainWindow::onCalculateClicked() {
    double lat = ui.spinLatitude->value();
    double lon = ui.spinLongitude->value();
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

    auto [hh, tt] = localTimeToUTC(time.hour(), time.minute(), lon);

    // Расчет
    calculator.calculate(date.year(), date.month(), date.day(), hh);
    auto result = calculator.getResult();
    // Вывод результата
    std::cout << "  Calculated Azimuth: " << result.azimuth_deg << " deg\n"
              << "  Time: " << date.year() << " y " << date.month() << " m " << date.day() << " d " << hh << "h\n"
              << "  Calculated Altitude: " << result.altitude_deg << " deg\n";
    SunPosition current = {static_cast<float>(result.azimuth_deg), static_cast<float>(result.altitude_deg)};

    double LT_Rise, LT_Set;  // Время восхода и заката (в часах)
    bool rises, sets, above; // Флаги: восходит ли Солнце, заходит ли, всегда ли выше горизонта

    calculator.getSunRiseAbove(date.year(), date.month(), date.day(), LT_Rise, LT_Set, rises, sets, above);

    auto [hh_rise, tt_rise] = UTCTimeToLocal(LT_Rise, lon);
    auto [hh_set, tt_set] = UTCTimeToLocal(LT_Set, lon);

    // Формирование фиктивных данных для демонстрации    
    int ccH = hh_set - hh_rise;
    std::vector<SunPosition> path;
    std::vector<std::tuple<QTime, double, double>> positions;
    QTime startTime(hh_rise, tt_rise);
    for(int i = 0; i < ccH-1; i++){
        auto [hh_local, tt_local] = localTimeToUTC(hh_rise+i, tt_rise, lon);
        calculator.calculate(date.year(), date.month(), date.day(), hh_local);
        auto resultLocal = calculator.getResult();
        std::cout << " azimuth_deg: " << resultLocal.azimuth_deg << " altitude_deg: " << resultLocal.altitude_deg << " num " << i+1 << "\n";
        SunPosition locSunP = {static_cast<float>(resultLocal.azimuth_deg), static_cast<float>(resultLocal.altitude_deg)};
        path.push_back(locSunP);
        positions.emplace_back(
            QTime(hh_rise+i, tt_rise), 
            resultLocal.altitude_deg, 
            resultLocal.azimuth_deg
        );
    }
    auto [hh_local2, tt_local2] = localTimeToUTC(hh_set, tt_set, lon);
    calculator.calculate(date.year(), date.month(), date.day(), hh_local2);
    // calculator.calculate(date.year(), date.month(), date.day(), hh_set);
    auto resultLocal2 = calculator.getResult();
    SunPosition locSunP2 = {static_cast<float>(resultLocal2.azimuth_deg), static_cast<float>(resultLocal2.altitude_deg)};
    path.push_back(locSunP2);
    positions.emplace_back(
        QTime(hh_set, tt_set), 
        resultLocal2.altitude_deg, 
        resultLocal2.azimuth_deg
    );
    //
    
    // Обновление результатов
    resultsWindow.setResults(
        QString::fromStdString(timeToString(hh_rise, tt_rise)), 
        QString::fromStdString(timeToString(hh_set, tt_set)), 
        current.azimuth, 
        current.altitude,
        positions,
        lat, 
        lon, 
        datetime
    );
    
    // Активация кнопки показа результатов
    ui.buttonShowResults->setEnabled(true);
    
    // Обновление 3D визуализации
    ui.openGLWidget->setSunPath(path);
    ui.openGLWidget->setCurrentSunPosition(current);
    ui.openGLWidget->setSunriseSunsetTimes(QString::fromStdString(timeToString(hh_rise, tt_rise)), QString::fromStdString(timeToString(hh_set, tt_set)));
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