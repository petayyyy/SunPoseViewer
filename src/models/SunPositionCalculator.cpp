#include "SunPositionCalculator.h"

#ifndef Radd
inline double Radd(double Deg)
{
    const double Rad = M_PI / 180.0;
    return Deg * Rad;
}
#endif

#ifndef Degg
inline double Degg(double Rad)
{
    return Rad * (180.0 / M_PI);
}
#endif

SunPositionCalculator::SunPositionCalculator() : observer({0.0, 0.0}), result({0.0, 0.0}) {}

void SunPositionCalculator::setObserver(const ObserverData &obs)
{
    observer = obs;
}

void SunPositionCalculator::calculate(int year, int month, int day, double hour)
{
    int h = static_cast<int>(hour);
    int m = static_cast<int>((hour - h) * 60);
    double s = ((hour - h) * 60 - m) * 60;
    MJD = Mjd(year, month, day, h, m, s);
    Time = (MJD + 2400000.5 - JD_J2000) / 36525;

    computeSunPosition();
    convertToHorizontalCoordinates();
}

void SunPositionCalculator::computeSunPosition()
{
    Vec3D sunEcl = SunPos(Time);
    Mat3D ecl2equ = Ecl2EquMatrix(Time);
    Vec3D sunEqu = ecl2equ * sunEcl;

    double ra = atan2(sunEqu[y], sunEqu[x]);
    double r_xy = sqrt(sunEqu[x] * sunEqu[x] + sunEqu[y] * sunEqu[y]);
    double dec = atan2(sunEqu[z], r_xy);

    double gmst = GMST(MJD);
    double lst = Modulo(gmst + Radd(observer.longitude_deg), 2 * M_PI);
    double tau = Modulo(lst - ra + 5 * M_PI, 2 * M_PI) - M_PI;

    result.azimuth_deg = ra;
    result.altitude_deg = dec;
    result.azimuth_deg = tau;
}

void SunPositionCalculator::convertToHorizontalCoordinates()
{
    double dec = result.altitude_deg;
    double tau = result.azimuth_deg;

    double h_rad, Az_rad;
    Equ2Hor(dec, tau, Radd(observer.latitude_deg), h_rad, Az_rad); // APC_Spheric.h

    Az_rad = Modulo(Az_rad, 2 * M_PI);
    if (tau > 0)
    {
        Az_rad = Modulo(Az_rad + M_PI, 2 * M_PI);
    }
    else
    {
        Az_rad = Modulo(Az_rad - M_PI, 2 * M_PI);
    }

    result.azimuth_deg = Degg(Az_rad);
    result.altitude_deg = Degg(h_rad);
}

SunPositionCalculator::SolarPosition SunPositionCalculator::getResult() const
{
    return result;
}

//------------------------------------------------------------------------------
//
// FindEvents: Search for rise/set/twilight events of Sun or Moon
//
// Input:
//
//   Event     Indicates event to search for
//   MJD0h     0h at desired date as Modified Julian Date
//   lambda    Geographic east longitude of the observer in [rad]
//   phi       Geographic latitude of the observer in [rad]
//
// Output:
//
//   LT_Rise   Local time of rising or beginning of twilight
//   LT_Set    Local time of setting or end of twilight
//   rises     Event takes place
//   sets      Event takes place
//   above     Sun or Moon is circumpolar
//
//------------------------------------------------------------------------------
void SunPositionCalculator::FindEvents(enEvent Event, double MJD0h, double lambda, double phi,
                                       double &LT_Rise, double &LT_Set,
                                       bool &rises, bool &sets, bool &above)
{
    //
    // Constants
    //
    static const double sinh0[5] = {

        sin(Rad * (+8.0 / 60.0)),  // Moonrise              at h= +8'
        sin(Rad * (-50.0 / 60.0)), // Sunrise               at h=-50'
        sin(Rad * (-6.0)),         // Civil twilight        at h=-6 deg
        sin(Rad * (-12.0)),        // Nautical twilight     at h=-12deg
        sin(Rad * (-18.0)),        // Astronomical twilight at h=-18deg
    };

    const double Cphi = cos(phi);
    const double Sphi = sin(phi);

    //
    // Variables
    //
    double hour = 1.0;
    double y_minus, y_0, y_plus;
    double xe, ye, root1, root2;
    int nRoot;

    // Initialize for search
    y_minus = SinAlt(Event, MJD0h, hour - 1.0, lambda, Cphi, Sphi) - sinh0[Event];

    above = (y_minus > 0.0);
    rises = false;
    sets = false;

    // loop over search intervals from [0h-2h] to [22h-24h]
    do
    {

        y_0 = SinAlt(Event, MJD0h, hour, lambda, Cphi, Sphi) - sinh0[Event];
        y_plus = SinAlt(Event, MJD0h, hour + 1.0, lambda, Cphi, Sphi) - sinh0[Event];

        // find parabola through three values y_minus,y_0,y_plus
        Quad(y_minus, y_0, y_plus, xe, ye, root1, root2, nRoot);

        if (nRoot == 1)
        {
            if (y_minus < 0.0)
            {
                LT_Rise = hour + root1;
                rises = true;
            }
            else
            {
                LT_Set = hour + root1;
                sets = true;
            }
        }

        if (nRoot == 2)
        {
            if (ye < 0.0)
            {
                LT_Rise = hour + root2;
                LT_Set = hour + root1;
            }
            else
            {
                LT_Rise = hour + root1;
                LT_Set = hour + root2;
            }
            rises = true;
            sets = true;
        }

        y_minus = y_plus; // prepare for next interval
        hour += 2.0;

    } while (!((hour == 25.0) || (rises && sets)));
}

//------------------------------------------------------------------------------
//
// SinAlt: Sine of the altitude of Sun or Moon
//
// Input:
//
//   Event     Indicates event to find
//   MJD0      0h at date to investigate (as Modified Julian Date)
//   Hour      Hour
//   lambda    Geographic east longitude in [rad]
//   Cphi      Cosine of geographic latitude
//   Sphi      Sine of geographic latitude
//
// <return>:   Sine of the altitude of Sun or Moon at instant of Event
//
//------------------------------------------------------------------------------
double SunPositionCalculator::SinAlt(enEvent Event, double MJD0, double Hour,
                                     double lambda, double Cphi, double Sphi)
{
    //
    // Variables
    //
    double MJD, T, RA, Dec, tau;

    MJD = MJD0 + Hour / 24.0;
    T = (MJD - 51544.5) / 36525.0;

    if (Event == Moon)
        MiniMoon(T, RA, Dec);
    else
        MiniSun(T, RA, Dec);

    tau = GMST(MJD) + lambda - RA;

    return (Sphi * sin(Dec) + Cphi * cos(Dec) * cos(tau));
}

void SunPositionCalculator::getSunRiseAbove(
    int year,
    int month,
    int day,
    double &LT_Rise,
    double &LT_Set,
    bool &rises,
    bool &sets,
    bool &above)
{
    double hour = 0.0;
    int h = static_cast<int>(hour);
    int m = static_cast<int>((hour - h) * 60);
    double s = ((hour - h) * 60 - m) * 60;
    MJD = Mjd(year, month, day, h, m, s);

    FindEvents(Sun, MJD, Radd(observer.longitude_deg), Radd(observer.latitude_deg), LT_Rise, LT_Set, rises, sets, above);
}