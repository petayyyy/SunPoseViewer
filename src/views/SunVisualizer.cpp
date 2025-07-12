#include "SunVisualizer.h"
#include "stb_image.h"
#include <GL/glu.h>
#include <cmath>

SunVisualizer::SunVisualizer() {}

void SunVisualizer::initGL() {
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    
    // Отключаем освещение для текстуры
    glDisable(GL_LIGHTING);
    
    // Настройки для лучшего отображения текстуры
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_ALPHA_TEST);
    
    loadTexture();
}

void SunVisualizer::setUserLocation(double lat, double lon) {
    latitude = lat;
    longitude = lon;
}

void SunVisualizer::setShowGrid(bool show) {
    showGrid = show;
}

void SunVisualizer::setSunPath(const std::vector<SunPosition>& path) {
    sunPath = path;
}

void SunVisualizer::setCurrentSunPosition(const SunPosition& position) {
    currentSunPosition = position;
}

void SunVisualizer::setSunriseSunsetTimes(const QString& sunrise, const QString& sunset) {
    sunriseTime = sunrise;
    sunsetTime = sunset;
}

void SunVisualizer::render(float rotationX, float rotationY, float zoom) {
    // Очистка буферов
    glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Настройка вида
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Камера
    glTranslatef(0.0f, 0.0f, -40.0f * zoom);
    glRotatef(rotationX, 1.0f, 0.0f, 0.0f);
    glRotatef(rotationY, 0.0f, 1.0f, 0.0f);

    // Рисуем Солнце
    GLfloat sunPos[4];
    drawSun(sunPos);

    // Преобразуем позицию в направление (w=0)
    GLfloat lightDir[4] = { sunPos[0], sunPos[1], sunPos[2], 0.0f };

    // Усиленные параметры света
    GLfloat diffuse_light[] = { 1.5f, 1.5f, 1.35f, 1.0f };
    GLfloat ambient_light[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat specular_light[] = { 1.5f, 1.5f, 1.5f, 1.0f };

    // Настраиваем источник
    glLightfv(GL_LIGHT0, GL_POSITION, lightDir);  // Направленный свет
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular_light);

    // Отключаем затухание (для направленного источника не требуется, но для точечного полезно)
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0f);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0f);

    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);

    // Земля освещается солнцем
    drawEarth();

    // Остальное без освещения
    glDisable(GL_LIGHTING);
    if (showGrid) drawGrid();
    drawUser();
    drawSunPath();
}

void SunVisualizer::loadTexture() {
    stbi_set_flip_vertically_on_load(true);
    int width, height, channels;
    unsigned char* data = stbi_load("resources/world_map.jpg", &width, &height, &channels, 0);
    
    if (data) {
        glGenTextures(1, &worldMapTexture);
        glBindTexture(GL_TEXTURE_2D, worldMapTexture);

        // Улучшенные параметры текстуры
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Определяем формат на основе количества каналов
        GLenum format = GL_RGB;
        if (channels == 4) format = GL_RGBA;
        else if (channels == 1) format = GL_LUMINANCE;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, 
                    format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);
    } else {
        qDebug() << "Failed to load texture";
        // Создаем простую текстуру, если загрузка не удалась
        unsigned char pixels[] = {255,0,0, 0,255,0, 0,0,255, 255,255,255};
        glGenTextures(1, &worldMapTexture);
        glBindTexture(GL_TEXTURE_2D, worldMapTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    }
}

void SunVisualizer::drawEarth() {
    glPushMatrix();
    
    // Настройки материала (добавлены specular и shininess)
    GLfloat mat_ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat mat_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat mat_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    GLfloat mat_shininess[] = { 30.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    
    // Включаем текстурирование
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, worldMapTexture);

    // Используем GL_MODULATE чтобы текстура взаимодействовала со светом
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    
    // Рисуем сферу с текстурой
    GLUquadric* quad = gluNewQuadric();
    gluQuadricTexture(quad, GL_TRUE);
    gluQuadricNormals(quad, GLU_SMOOTH);
    gluSphere(quad, 10.0, 64, 64);
    gluDeleteQuadric(quad);
    
    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
}

// Общая функция преобразования координат
Point3f sphericalToCartesian(float lon, float lat, float radius = 10.0f) {
    float theta = (lon - 90) * M_PI / 180.0f;
    float phi = lat * M_PI / 180.0f;
    Point3f arrOut;
    arrOut.x = radius * cos(phi) * cos(theta);
    arrOut.y = radius * cos(phi) * sin(theta);
    arrOut.z = radius * sin(phi);
    return arrOut;
}

void SunVisualizer::drawGrid() {
    // glColor3f(0.7f, 0.7f, 0.7f);
    // glBegin(GL_LINES);
    // for (float lat = -80; lat <= 80; lat += 10.0f) {
    //     for (float lon = -180; lon <= 180; lon += 10.0f) {
    //         Point3f p = sphericalToCartesian(lon, lat);
    //         glVertex3f(p.x, p.y, p.z);
    //         glVertex3f(p.x * 1.01f, p.y * 1.01f, p.z * 1.01f);
    //     }
    // }
    // glEnd();

    glColor3f(0.7f, 0.7f, 0.7f);
    glBegin(GL_LINE_STRIP);
    for (float lat = -90; lat <= 90; lat += 10.0f) {
        for (float lon = -180; lon <= 180; lon += 10.0f) {
            Point3f p = sphericalToCartesian(lon, lat, 10.05f);
            glVertex3f(p.x, p.y, p.z);
        }
    }
    glEnd();

    glColor3f(0.7f, 0.7f, 0.7f);
    glBegin(GL_LINE_STRIP);
    for (float lon = -180; lon <= 180; lon += 10.0f) {
        for (float lat = -90; lat <= 90; lat += 10.0f) {
            Point3f p = sphericalToCartesian(lon, lat, 10.05f);
            glVertex3f(p.x, p.y, p.z);
        }
    }
    glEnd();
}

void SunVisualizer::drawUser() {
    glPushMatrix();
    
    // Преобразуем координаты пользователя в декартовы
    Point3f userPos = sphericalToCartesian(currentSunPosition.altitude, currentSunPosition.azimuth, 10.01f);
    
    // Перемещаемся к позиции пользователя
    glTranslatef(userPos.x, userPos.y, userPos.z);
    
    // Вычисляем нормаль к сфере (единичный вектор от центра к точке)
    Point3f normal = userPos;
    float length = sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
    if (length > 0) {
        normal.x /= length;
        normal.y /= length;
        normal.z /= length;
    }
    
    // Выравниваем ось Y куба по нормали
    Point3f up;
    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f; // Исходное направление куба
    float dot = up.x * normal.x + up.y * normal.y + up.z * normal.z;
    
    if (fabs(dot + 1.0f) < 0.000001f) {
        // Противоположные направления: разворот на 180°
        glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
    } else if (fabs(dot - 1.0f) > 0.000001f) {
        // Вычисляем ось и угол вращения
        Point3f axis;
        axis.x = up.y * normal.z - up.z * normal.y;
        axis.y = up.z * normal.x - up.x * normal.z;
        axis.z = up.x * normal.y - up.y * normal.x;
        
        float axisLength = sqrt(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z);
        if (axisLength > 0) {
            axis.x /= axisLength;
            axis.y /= axisLength;
            axis.z /= axisLength;
            
            float angle = acosf(dot) * 180.0f / M_PI;
            glRotatef(angle, axis.x, axis.y, axis.z);
        }
    }
    
    // Размер куба (ось Y теперь перпендикулярна сфере)
    float sizeW = 0.1f;
    float sizeH = 0.5f;
    float sizeZ = 0.1f;

    float w = sizeW / 2;
    float h = sizeH / 2;
    float d = sizeZ / 2;
    
    // Отключаем текстуру и задаем цвет
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 0.0f, 0.0f);
    
    // Рисуем куб
    glBegin(GL_QUADS);
        // Передняя грань
        glVertex3f(-w, -h, d);
        glVertex3f(w, -h, d);
        glVertex3f(w, h, d);
        glVertex3f(-w, h, d);
        
        // Задняя грань
        glVertex3f(-w, -h, -d);
        glVertex3f(-w, h, -d);
        glVertex3f(w, h, -d);
        glVertex3f(w, -h, -d);
        
        // Верхняя грань
        glVertex3f(-w, h, -d);
        glVertex3f(-w, h, d);
        glVertex3f(w, h, d);
        glVertex3f(w, h, -d);
        
        // Нижняя грань
        glVertex3f(-w, -h, -d);
        glVertex3f(w, -h, -d);
        glVertex3f(w, -h, d);
        glVertex3f(-w, -h, d);
        
        // Правая грань
        glVertex3f(w, -h, -d);
        glVertex3f(w, h, -d);
        glVertex3f(w, h, d);
        glVertex3f(w, -h, d);
        
        // Левая грань
        glVertex3f(-w, -h, -d);
        glVertex3f(-w, h, -d);
        glVertex3f(-w, h, d);
        glVertex3f(-w, -h, d);
    glEnd();
    
    glPopMatrix();
}

void SunVisualizer::drawSunPath() {
    glColor3f(1.0f, 0.8f, 0.0f);
    glBegin(GL_LINE_STRIP);
    for (const auto& pos : sunPath) {
        Point3f  p = sphericalToCartesian(pos.azimuth, pos.altitude, 12.0f);
        glVertex3f(p.x, p.y, p.z);
    }
    glEnd();
}

void SunVisualizer::drawSun(GLfloat* sunLightPosition) {
    glPushMatrix();
    Point3f  p = sphericalToCartesian(currentSunPosition.altitude, currentSunPosition.azimuth, 12.0f);

    // Возвращаем позицию солнца для света
    if (sunLightPosition) {
        sunLightPosition[0] = p.x;
        sunLightPosition[1] = p.y;
        sunLightPosition[2] = p.z;
        sunLightPosition[3] = 1.0f;  // позиционный источник
    }

    // Рисуем само солнце
    glTranslatef(p.x, p.y, p.z);
    glColor3f(1.0f, 1.0f, 0.0f);

    GLUquadric* quad = gluNewQuadric();
    gluSphere(quad, 0.25, 16, 16);
    gluDeleteQuadric(quad);

    glPopMatrix();
}