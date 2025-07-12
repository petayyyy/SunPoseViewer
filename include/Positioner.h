#ifndef POSITIONER_H
#define POSITIONER_H

#include <QObject>
#include <QGeoPositionInfoSource>
#include <QGeoCoordinate>

class Positioner : public QObject
{
    Q_OBJECT

public:
    explicit Positioner(QObject *parent = nullptr);
    ~Positioner();

    bool updateCoordinates();
    bool hasValidCoordinates() const;
    QGeoCoordinate getCoordinates() const;

signals:
    void coordinatesUpdated(const QGeoCoordinate &coordinate);
    void errorOccurred(const QString &errorMessage);

private slots:
    void handlePositionUpdate(const QGeoPositionInfo &info);
    void handlePositionError(QGeoPositionInfoSource::Error error);

private:
    QGeoPositionInfoSource *m_positionSource = nullptr;
    QGeoCoordinate m_currentCoordinate = QGeoCoordinate(0.0, 0.0);  // default position
    bool m_isValid = false;
};

#endif // POSITIONER_H
