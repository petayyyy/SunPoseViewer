#pragma once

#include <vector>
#include <QOpenGLFunctions>
#include <QString>

struct SunPosition {
    float azimuth;
    float altitude;
};

struct Point3fN
{
    float x, y, z;
};

class SunVisualizer : protected QOpenGLFunctions {
public:
    SunVisualizer();

    void initGL();
    void render(float rotationX, float rotationY, float zoom);

    void setUserLocation(double latitude, double longitude);
    void setShowGrid(bool show);

    void setSunPath(const std::vector<SunPosition>& path);
    void setCurrentSunPosition(const SunPosition& position);
    void setSunriseSunsetTimes(const QString& sunrise, const QString& sunset);

private:
    double latitude = 0.0;
    double longitude = 0.0;
    bool showGrid = true;

    std::vector<SunPosition> sunPath;
    SunPosition currentSunPosition;
    QString sunriseTime;
    QString sunsetTime;

    GLuint worldMapTexture = 0;

    void drawEarth();
    void drawGrid();
    void drawUser();
    void drawSunPath();
    void drawSun(GLfloat* sunLightPosition);
    void loadTexture();
};