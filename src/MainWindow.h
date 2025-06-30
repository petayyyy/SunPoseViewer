#pragma once
#include <QMainWindow>
#include "SunWidget.h"
#include "AstronomyCalculator.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void calculateSunPosition();
    void toggleDateTimeInput(bool useSystem);

private:
    Ui::MainWindow *ui;
    AstronomyCalculator calculator;
};