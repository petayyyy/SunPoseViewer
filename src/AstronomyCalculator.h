#pragma once
#include <QDateTime>

struct SolarCoords {
    double azimuth;
    double altitude;
    bool valid;
};

class AstronomyCalculator {
public:
    SolarCoords getHorizontalCoordinates(double latitude, double longitude, double altitude, const QDateTime& dt);
};