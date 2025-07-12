#pragma once

#include <QOpenGLWidget>
#include <QTimer>
#include "SunVisualizer.h"

class SunWidget : public QOpenGLWidget {
    Q_OBJECT
public:
    explicit SunWidget(QWidget* parent = nullptr);
    const SunVisualizer& getVisualizer() const { return visualizer; }
    
    void setSunPath(const std::vector<SunPosition>& path);
    void setCurrentSunPosition(const SunPosition& position);
    void setSunriseSunsetTimes(const QString& sunrise, const QString& sunset);
    void setUserLocation(double latitude, double longitude);
    void setShowGrid(bool show);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    SunVisualizer visualizer;
    QPoint lastMousePos;
    float rotationX = 20.0f;
    float rotationY = -40.0f;
    float zoom = 1.0f;
};