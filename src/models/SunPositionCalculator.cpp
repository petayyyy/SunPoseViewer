#include "SunPositionCalculator.h"

#ifndef Radd
inline double Radd(double Deg) {
    const double Rad = M_PI / 180.0;
    return Deg * Rad;
}
#endif

#ifndef Degg
inline double Degg(double Rad) {
    return Rad * (180.0 / M_PI);
}
#endif

SunPositionCalculator::SunPositionCalculator() : observer({0.0, 0.0}), result({0.0,0.0}) {}

void SunPositionCalculator::setObserver(const ObserverData& obs) {
    observer = obs;
}

void SunPositionCalculator::calculate(int year, int month, int day, double hour) {
    int h = static_cast<int>(hour);
    int m = static_cast<int>((hour - h) * 60);
    double s = ((hour - h) * 60 - m) * 60;
    MJD = Mjd(year, month, day, h, m, s);
    Time = (MJD + 2400000.5 - JD_J2000) / 36525;

    computeSunPosition();
    convertToHorizontalCoordinates();
}

void SunPositionCalculator::computeSunPosition() {
    Vec3D sunEcl = SunPos(Time);
    Mat3D ecl2equ = Ecl2EquMatrix(Time);
    Vec3D sunEqu = ecl2equ * sunEcl;

    double ra = atan2(sunEqu[y], sunEqu[x]);
    double r_xy = sqrt(sunEqu[x]*sunEqu[x] + sunEqu[y]*sunEqu[y]);
    double dec = atan2(sunEqu[z], r_xy);

    double gmst = GMST(MJD);
    double lst = Modulo(gmst + Radd(observer.longitude_deg), 2*M_PI);
    double tau = Modulo(lst - ra + 5*M_PI, 2*M_PI) - M_PI;

    result.azimuth_deg = ra;
    result.altitude_deg = dec;
    result.azimuth_deg = tau;
}

void SunPositionCalculator::convertToHorizontalCoordinates() {
    double dec = result.altitude_deg;
    double tau = result.azimuth_deg;

    double h_rad, Az_rad;
    Equ2Hor(dec, tau, Radd(observer.latitude_deg), h_rad, Az_rad); // APC_Spheric.h

    Az_rad = Modulo(Az_rad, 2*M_PI);
    if (tau >0) {
        Az_rad = Modulo(Az_rad + M_PI, 2*M_PI);
    } else {
        Az_rad = Modulo(Az_rad - M_PI, 2*M_PI);
    }

    result.azimuth_deg = Degg(Az_rad);
    result.altitude_deg = Degg(h_rad);
}

SunPositionCalculator::SolarPosition SunPositionCalculator::getResult() const {
    return result;
}