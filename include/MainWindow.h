#pragma once

#include <QMainWindow>
#include "ui_MainWindow.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onCalculateClicked();
    void onUseSystemTimeChanged(int state);
    void onShowGridChanged(int state);

private:
    Ui::MainWindow ui;
};