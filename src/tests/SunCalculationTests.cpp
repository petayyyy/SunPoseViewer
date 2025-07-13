#include "SunCalculationTests.h"

void SunCalculationTests::runTests()
{
    // Подготовленные тестовые данные
    std::vector<TestCase> tests = {
        // Test 1 (azimuth < PI):
        {2025, 7, 1, 23.0, 37.40370, 55.49740, 19.89, -9.46},
        // 108818, Южное ст Ватутинки, Москва, 02.07.2025 02:00
        // UTC+0 format: 01.07.2025 23:00

        // Test 2 (azimuth > PI):
        // 108818, Южное ст Ватутинки, Москва, 02.07.2025 15:00
        // UTC+0 format: 02.07.2025 12:00
        {2025, 7, 2, 16.0, 37.40370, 55.49740, 291.16, 13.51},

        // Test 3 (Ilja)
        {2025, 7, 12, 3.0, 37.0, 55.0, 106.81, 38.01},

        // More tests ?
    };

    std::cout << "Running SunPositionCalculator tests...\n";

    for (size_t i = 0; i < tests.size(); ++i)
    {
        std::cout << "Test " << (i + 1) << ":\n";
        runTest(tests[i]);
    }

    std::cout << "\n\nAll tests completed.\n";
}

void SunCalculationTests::runTest(const TestCase &test)
{
    SunPositionCalculator calculator;

    // Настройка наблюдателя
    SunPositionCalculator::ObserverData obs = {
        .latitude_deg = test.latitude_deg,
        .longitude_deg = test.longitude_deg};
    calculator.setObserver(obs);

    // Расчет
    calculator.calculate(test.year, test.month, test.day, test.hour);
    auto result = calculator.getResult();

    // Вывод результата
    std::cout << "  Calculated Azimuth: " << result.azimuth_deg << " deg\n"
              << "  Expected Azimuth:   " << test.expected_azimuth_deg << " deg\n"
              << "  Calculated Altitude: " << result.altitude_deg << " deg\n"
              << "  Expected Altitude:   " << test.expected_altitude_deg << " deg\n";

    // Проверка
    bool azimuth_ok = isClose(result.azimuth_deg, test.expected_azimuth_deg);
    bool altitude_ok = isClose(result.altitude_deg, test.expected_altitude_deg);

    if (azimuth_ok && altitude_ok)
    {
        std::cout << "  Test passed ✅\n\n";
    }
    else
    {
        std::cout << "  Test failed ❌\n\n";
    }

    std::cout << "testing rise and above\ntest data: 2025 july 12 coords lng 37.0 ltd 55.0\n";
    SunPositionCalculator::ObserverData newObs = {
        .latitude_deg = 55.0,
        .longitude_deg = 37.0};
    calculator.setObserver(newObs);
    double LT_Rise;
    double LT_Set;
    bool rises;
    bool sets;
    bool above;
    calculator.getSunRiseAbove(2025, 7, 12, LT_Rise, LT_Set, rises, sets, above);
    std::cout << "results: LT_Rise:" << Time(LT_Rise, HHMM) << ", LT_Set:" << Time(LT_Set, HHMM) << ", rises=" << rises << ", sets=" << sets << ", above=" << above;
}
