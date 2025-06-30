#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QTimer>

/**
 * @brief Класс SunWidget — кастомный OpenGL-виджет для визуализации положения Солнца
 * в азимутальной системе координат.
 *
 * Отображает:
 * - Искусственный горизонт
 * - Сферу, представляющую положение Солнца
 * - Азимутально-альтитудную сетку (опционально)
 */
class SunWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

public:
    /**
     * @brief Конструктор
     * @param parent Родительский виджет
     */
    explicit SunWidget(QWidget* parent = nullptr);

    /**
     * @brief Установка координат Солнца
     * @param azimuth Азимут в градусах (0° = север, 90° = восток, 180° = юг, 270° = запад)
     * @param altitude Высота над горизонтом в градусах (0° = горизонт, 90° = зенит)
     */
    void setSunPosition(double azimuth, double altitude);

    /**
     * @brief Включение/отключение отображения сетки
     * @param show true — отображать сетку, false — скрыть
     */
    void setShowGrid(bool show);

protected:
    /// Инициализация OpenGL
    void initializeGL() override;

    /// Обработка изменения размеров окна
    void resizeGL(int w, int h) override;

    /// Отрисовка OpenGL-сцены
    void paintGL() override;

private:
    /// Отрисовка горизонта (замкнутая окружность в плоскости XY)
    void drawHorizon();

    /// Отрисовка Солнца в рассчитанном положении
    void drawSun();

    /// Отрисовка сетки (опционально)
    void drawGrid();

    double azimuth = 0.0;   ///< Азимут Солнца в градусах
    double altitude = 0.0;  ///< Высота Солнца в градусах
    bool showGrid = true;   ///< Флаг отображения координатной сетки
};