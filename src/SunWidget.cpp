#include "SunWidget.h"
#include <QtMath>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QPainter>

SunWidget::SunWidget(QWidget* parent)
    : QOpenGLWidget(parent), azimuth(0), altitude(0), showGrid(true) {}

void SunWidget::setSunPosition(double az, double alt) {
    azimuth = az;
    altitude = alt;
    update();
}

void SunWidget::setShowGrid(bool show) {
    showGrid = show;
    update();
}

void SunWidget::initializeGL() {
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
}

void SunWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, GLfloat(w)/GLfloat(h), 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void SunWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(0, 0, 5, 0, 0, 0, 0, 1, 0);

    if (showGrid) drawGrid();
    drawHorizon();
    drawSun();
}

void SunWidget::drawHorizon() {
    glColor3f(0.3f, 0.7f, 0.3f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 360; ++i) {
        double rad = qDegreesToRadians(double(i));
        glVertex3f(cos(rad), 0.0f, sin(rad));
    }
    glEnd();
}

void SunWidget::drawGrid() {
    glColor3f(0.4f, 0.4f, 0.4f);
    for (int i = 10; i < 90; i += 10) {
        glBegin(GL_LINE_LOOP);
        double r = cos(qDegreesToRadians(i));
        for (int j = 0; j < 360; ++j) {
            double rad = qDegreesToRadians(double(j));
            glVertex3f(r * cos(rad), sin(qDegreesToRadians(i)), r * sin(rad));
        }
        glEnd();
    }
}

void SunWidget::drawSun() {
    double az_rad = qDegreesToRadians(azimuth);
    double alt_rad = qDegreesToRadians(altitude);
    float x = cos(alt_rad) * sin(az_rad);
    float y = sin(alt_rad);
    float z = cos(alt_rad) * cos(az_rad);

    glColor3f(1.0f, 1.0f, 0.0f);
    glPointSize(10.0f);
    glBegin(GL_POINTS);
    glVertex3f(x, y, z);
    glEnd();
}