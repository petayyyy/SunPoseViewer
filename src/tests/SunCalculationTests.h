#ifndef SUNCALCULATIONTESTS_H
#define SUNCALCULATIONTESTS_H

#include <vector>
#include <cmath>
#include <iostream>
#include "models/SunPositionCalculator.h"

struct TestCase {
    int year;
    int month;
    int day;
    double hour;
    double longitude_deg;
    double latitude_deg;
    double expected_azimuth_deg;
    double expected_altitude_deg;
};

class SunCalculationTests {
public:
    void runTests();

private:
    void runTest(const TestCase& test);

    bool isClose(double a, double b, double tolerance = 1e-3) {
        return std::abs(a - b) <= tolerance;
    }
};

#endif // SUNCALCULATIONTESTS_H