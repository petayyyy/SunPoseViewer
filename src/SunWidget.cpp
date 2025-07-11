#include "SunWidget.h"
#include <QMouseEvent>
#include <QMatrix4x4>

SunWidget::SunWidget(QWidget* parent) : QOpenGLWidget(parent) {
    setFocusPolicy(Qt::StrongFocus);
}

void SunWidget::initializeGL() {
    visualizer.initGL();
}

void SunWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);

    QMatrix4x4 projection;
    projection.perspective(45.0f, GLfloat(w) / h, 0.1f, 100.0f);

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(projection.constData());
    glMatrixMode(GL_MODELVIEW);
}

void SunWidget::paintGL() {
    visualizer.render(rotationX, rotationY, zoom);
}

void SunWidget::mousePressEvent(QMouseEvent* event) {
    lastMousePos = event->pos();
}

void SunWidget::mouseMoveEvent(QMouseEvent* event) {
    float dx = event->x() - lastMousePos.x();
    float dy = event->y() - lastMousePos.y();
    rotationX -= dy * 0.5f;
    rotationY -= dx * 0.5f;
    lastMousePos = event->pos();
    update();
}

void SunWidget::wheelEvent(QWheelEvent* event) {
    zoom *= (event->angleDelta().y() > 0) ? 0.9f : 1.1f;
    update();
}

void SunWidget::setSunPath(const std::vector<SunPosition>& path) {
    visualizer.setSunPath(path);
    update();
}

void SunWidget::setCurrentSunPosition(const SunPosition& position) {
    visualizer.setCurrentSunPosition(position);
    update();
}

void SunWidget::setSunriseSunsetTimes(const QString& sunrise, const QString& sunset) {
    visualizer.setSunriseSunsetTimes(sunrise, sunset);
}

void SunWidget::setUserLocation(double latitude, double longitude) {
    visualizer.setUserLocation(latitude, longitude);
}

void SunWidget::setShowGrid(bool show) {
    visualizer.setShowGrid(show);
    update();
}