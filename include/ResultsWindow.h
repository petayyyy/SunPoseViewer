#pragma once

#include <QDialog>
#include <QTime>
#include <vector>
#include <tuple>

class QGroupBox;
class QLabel;
class QTableWidget;

class ResultsWindow : public QDialog {
    Q_OBJECT

public:
    explicit ResultsWindow(QWidget *parent = nullptr);
    void setResults(const QString& sunrise, const QString& sunset,
                   double azimuth, double altitude,
                   const std::vector<std::tuple<QTime, double, double>>& positions,
                   double latitude, double longitude, const QDateTime& datetime);

private:
    QLabel* latitudeLabel;
    QLabel* longitudeLabel;
    QLabel* datetimeLabel;
    QLabel* sunriseLabel;
    QLabel* sunsetLabel;
    QLabel* azimuthLabel;
    QLabel* altitudeLabel;
    QTableWidget* table;
};