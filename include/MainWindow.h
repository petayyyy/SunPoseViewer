#pragma once

#include <QMainWindow>
#include "ui_MainWindow.h"
#include "ResultsWindow.h"
//#include "Positioner.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onCalculateClicked();
    void onUseSystemTimeChanged(int state);
    void onShowGridChanged(int state);
    void onUseSystemLocationChanged(bool checked);

private:
    Ui::MainWindow ui;
    ResultsWindow resultsWindow;
    //Positioner positioner;
    bool gpsErrorFlag = false;
};