#ifndef SUNPOSITIONCALCULATOR_H
#define SUNPOSITIONCALCULATOR_H

#include <cmath>
#include "APC_VecMat3D.h"
#include "APC_Math.h"
#include "APC_PrecNut.h"
#include "APC_Sun.h"
#include "APC_Time.h"
#include "APC_Spheric.h"
#include "APC_Moon.h"

class SunPositionCalculator
{
public:
    struct ObserverData
    {
        double latitude_deg;
        double longitude_deg;
    };

    struct SolarPosition
    {
        double azimuth_deg;
        double altitude_deg;
    };

    SunPositionCalculator();

    void setObserver(const ObserverData &obs);
    void calculate(int year, int month, int day, double hour);
    void getSunRiseAbove(int year,
                         int month,
                         int day,
                         double &LT_Rise,
                         double &LT_Set,
                         bool &rises,
                         bool &sets,
                         bool &above);
    SolarPosition getResult() const;

private:
    // Events to search for
    enum enEvent
    {
        Moon,     // indicates moonrise or moonset
        Sun,      // indicates sunrise or sunset
        CivilTwi, // indicates Civil twilight
        NautiTwi, // indicates nautical twilight
        AstroTwi  // indicates astronomical twilight
    };

    ObserverData observer;
    SolarPosition result;

    double MJD;
    double Time;
    const double JD_J2000 = 2451545.0;
    const double c_light = 173.14;
    const double Rad = M_PI / 180.0;

    void computeSunPosition();
    void convertToHorizontalCoordinates();
    void FindEvents(enEvent Event, double MJD0h, double lambda, double phi,
                    double &LT_Rise, double &LT_Set,
                    bool &rises, bool &sets, bool &above);
    double SinAlt(enEvent Event, double MJD0, double Hour,
                  double lambda, double Cphi, double Sphi);
};

#endif // SUNPOSITIONCALCULATOR_H