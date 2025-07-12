#include "ResultsWindow.h"
#include <QGroupBox>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QTableWidget>
#include <QHeaderView>

ResultsWindow::ResultsWindow(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Результаты расчета");
    setMinimumSize(600, 400);
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    
    // Секция входных данных
    QGroupBox *inputGroup = new QGroupBox("Входные данные");
    QFormLayout *inputLayout = new QFormLayout;
    
    latitudeLabel = new QLabel;
    longitudeLabel = new QLabel;
    datetimeLabel = new QLabel;
    
    inputLayout->addRow("Широта:", latitudeLabel);
    inputLayout->addRow("Долгота:", longitudeLabel);
    inputLayout->addRow("Дата/время:", datetimeLabel);
    inputGroup->setLayout(inputLayout);
    
    // Секция результатов солнца
    QGroupBox *sunGroup = new QGroupBox("Положение солнца");
    QFormLayout *sunLayout = new QFormLayout;
    
    sunriseLabel = new QLabel;
    sunsetLabel = new QLabel;
    azimuthLabel = new QLabel;
    altitudeLabel = new QLabel;
    
    sunLayout->addRow("Восход:", sunriseLabel);
    sunLayout->addRow("Закат:", sunsetLabel);
    sunLayout->addRow("Азимут:", azimuthLabel);
    sunLayout->addRow("Высота:", altitudeLabel);
    sunGroup->setLayout(sunLayout);
    
    // Таблица позиций
    table = new QTableWidget;
    table->setColumnCount(3);
    table->setHorizontalHeaderLabels({"Время", "Высота", "Азимут"});
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
    layout->addWidget(inputGroup);
    layout->addWidget(sunGroup);
    layout->addWidget(table);
}

void ResultsWindow::setResults(const QString& sunrise, const QString& sunset,
                              double azimuth, double altitude,
                              const std::vector<std::tuple<QTime, double, double>>& positions,
                              double latitude, double longitude, const QDateTime& datetime) {
    // Заполнение входных данных
    latitudeLabel->setText(QString::number(latitude, 'f', 6));
    longitudeLabel->setText(QString::number(longitude, 'f', 6));
    datetimeLabel->setText(datetime.toString("yyyy-MM-dd HH:mm:ss"));
    
    // Заполнение данных солнца
    sunriseLabel->setText(sunrise);
    sunsetLabel->setText(sunset);
    azimuthLabel->setText(QString::number(azimuth, 'f', 2));
    altitudeLabel->setText(QString::number(altitude, 'f', 2));
    
    // Заполнение таблицы
    table->setRowCount(positions.size());
    for (size_t i = 0; i < positions.size(); ++i) {
        const auto& [time, alt, azm] = positions[i];
        table->setItem(i, 0, new QTableWidgetItem(time.toString("HH:mm")));
        table->setItem(i, 1, new QTableWidgetItem(QString::number(alt, 'f', 2)));
        table->setItem(i, 2, new QTableWidgetItem(QString::number(azm, 'f', 2)));
    }
}