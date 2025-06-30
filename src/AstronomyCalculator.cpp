#include "AstronomyCalculator.h"
#include <cmath>
#include <algorithm>

constexpr double deg2rad(double d){ return d*M_PI/180.0; }
constexpr double rad2deg(double r){ return r*180.0/M_PI; }

static double toJulianDate(qint64 secs){
    return secs/86400.0 + 2440587.5;
}

SolarCoords AstronomyCalculator::getHorizontalCoordinates(double lat_deg, double lon_deg, double /*alt_m*/, const QDateTime& dt) {
    SolarCoords res{0,0,false};
    if (!dt.isValid()  lat_deg<-90lat_deg>90lon_deg<-180lon_deg>180)
        return res;

    double J = toJulianDate(dt.toSecsSinceEpoch());
    double D = J - 2451545.0;
    double g = fmod(357.529 + 0.98560028*D, 360.0);
    double C = 1.9148*sin(deg2rad(g)) + 0.02*sin(deg2rad(2*g)) + 0.0003*sin(deg2rad(3*g));
    double L = fmod(g + C + 180 + 102.9372, 360.0);
    double sin_delta = sin(deg2rad(L)) * sin(deg2rad(23.4397));
    double delta = asin(std::clamp(sin_delta, -1.0, 1.0));
    double time_utc = (dt.time().hour()*3600 + dt.time().minute()*60 + dt.time().second())/86400.0;
    double H = fmod((time_utc + lon_deg/360.0 - 0.5)*360 + 180 - L, 360.0);
    double lat = deg2rad(lat_deg), Hrad = deg2rad(H);
    double sin_alt = sin(lat)*sin(delta) + cos(lat)*cos(delta)*cos(Hrad);
    double altitude = asin(std::clamp(sin_alt,-1.0,1.0));
    double cos_az = (sin(delta) - sin(altitude)*sin(lat)) / (cos(altitude)*cos(lat));
    double azimuth = acos(std::clamp(cos_az,-1.0,1.0));
    if (sin(Hrad) > 0) azimuth = 2*M_PI - azimuth;
    res.azimuth = rad2deg(azimuth);
    res.altitude = rad2deg(altitude);
    res.valid = true;
    return res;
}