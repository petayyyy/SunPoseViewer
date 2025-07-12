#ifndef MOCKPOSITIONER_H
#define MOCKPOSITIONER_H

#include "Positioner.h"

class MockPositioner : public Positioner
{
    Q_OBJECT

public:
    explicit MockPositioner(QObject *parent = nullptr)
        : Positioner(parent)
    {
        // Fake test coordinates
        m_currentCoordinate = QGeoCoordinate(28.396, -80.605);//cape Canaverall
        //m_currentCoordinate = QGeoCoordinate(51.5074, -0.1278); // London
        m_isValid = true;
    }

    bool updateCoordinates() override
    {
        emit coordinatesUpdated(m_currentCoordinate);
        return true;
    }

    bool hasValidCoordinates() const override { return m_isValid; }
    QGeoCoordinate getCoordinates() const override { return m_currentCoordinate; }

private:
    QGeoCoordinate m_currentCoordinate;
    bool m_isValid;
};

#endif // MOCKPOSITIONER_H
