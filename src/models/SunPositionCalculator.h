#ifndef SUNPOSITIONCALCULATOR_H
#define SUNPOSITIONCALCULATOR_H

#include <cmath>
#include "APC_VecMat3D.h"
#include "APC_Math.h"
#include "APC_PrecNut.h"
#include "APC_Sun.h"
#include "APC_Time.h"
#include "APC_Spheric.h"

class SunPositionCalculator {
public:
    struct ObserverData {
        double latitude_deg;
        double longitude_deg;
    };

    struct SolarPosition {
        double azimuth_deg;
        double altitude_deg;
    };

    SunPositionCalculator();

    void setObserver(const ObserverData& obs);
    void calculate(int year, int month, int day, double hour);
    SolarPosition getResult() const;

private:
    ObserverData observer;
    SolarPosition result;

    double MJD;
    double Time;
    const double JD_J2000 = 2451545.0;
    const double c_light = 173.14;

    void computeSunPosition();
    void convertToHorizontalCoordinates();
};

#endif //SUNPOSITIONCALCULATOR_H