#include "SunVisualizer.h"
#include "stb_image.h"
#include <GL/glu.h>
#include <cmath>

struct Point3fN
{
    float x, y, z;
};


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

// void SunVisualizer::render(float rotationX, float rotationY, float zoom) {
//     // Очистка буферов
//     glClearColor(0.2f, 0.2f, 0.3f, 1.0f);  // Более светлый фон
//     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//     // Настройка вида
//     glMatrixMode(GL_MODELVIEW);
//     glLoadIdentity();

//     // Позиция камеры
//     glTranslatef(0.0f, 0.0f, -5.0f * zoom);
//     glRotatef(rotationX, 1.0f, 0.0f, 0.0f);
//     glRotatef(rotationY, 0.0f, 1.0f, 0.0f);

//     // Позиция и параметры источника света в мировых координатах (не в glLoadIdentity!)
//     GLfloat light_position[] = { 0.0f, 0.0f, 5.0f, 1.0f };
//     GLfloat diffuse_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
//     GLfloat ambient_light[] = { 0.3f, 0.3f, 0.3f, 1.0f };
//     GLfloat specular_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };

//     glLightfv(GL_LIGHT0, GL_POSITION, light_position);
//     glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
//     glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);
//     glLightfv(GL_LIGHT0, GL_SPECULAR, specular_light);

//     glEnable(GL_LIGHT0);
//     glEnable(GL_LIGHTING);

//     // Рисуем землю (будет освещённой)
//     drawEarth();

//     // Остальные элементы — без освещения
//     glDisable(GL_LIGHTING);

//     if (showGrid) drawGrid();
//     drawSunPath();
//     drawSun();
// }

void SunVisualizer::render(float rotationX, float rotationY, float zoom) {
    // Очистка буферов
    glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Настройка вида
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Камера
    glTranslatef(0.0f, 0.0f, -5.0f * zoom);
    glRotatef(rotationX, 1.0f, 0.0f, 0.0f);
    glRotatef(rotationY, 0.0f, 1.0f, 0.0f);

    // Рисуем Солнце (визуально и как источник света)
    GLfloat sunPos[4];
    drawSun(sunPos);  // передаём массив для света

    // Включаем освещение от Солнца
    GLfloat diffuse_light[] = { 1.0f, 1.0f, 0.9f, 1.0f };    // тёплый свет
    GLfloat ambient_light[] = { 0.2f, 0.2f, 0.2f, 1.0f };    // мягкий фон
    GLfloat specular_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, sunPos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular_light);

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
    gluSphere(quad, 1.0, 64, 64);
    gluDeleteQuadric(quad);
    
    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
}

// Общая функция преобразования координат
Point3fN sphericalToCartesian(float lon, float lat, float radius = 1.0f) {
    float theta = (lon - 90) * M_PI / 180.0f;
    float phi = lat * M_PI / 180.0f;
    Point3fN arrOut;
    arrOut.x = radius * cos(phi) * cos(theta);
    arrOut.y = radius * cos(phi) * sin(theta);
    arrOut.z = radius * sin(phi);
    return arrOut;
}

void SunVisualizer::drawGrid() {
    glColor3f(0.7f, 0.7f, 0.7f);
    glBegin(GL_LINES);
    for (float lat = -80; lat <= 80; lat += 10.0f) {
        for (float lon = -180; lon <= 180; lon += 10.0f) {
            Point3fN p = sphericalToCartesian(lon, lat);
            glVertex3f(p.x, p.y, p.z);
            glVertex3f(p.x * 1.01f, p.y * 1.01f, p.z * 1.01f);
        }
    }
    glEnd();
}

void SunVisualizer::drawUser() {
    glPushMatrix();
    
    // Преобразуем координаты пользователя (широта, долгота) в декартовы
    Point3fN userPos = sphericalToCartesian(currentSunPosition.altitude, currentSunPosition.azimuth, 1.01f);
    
    // Перемещаемся к позиции пользователя
    glTranslatef(userPos.x, userPos.y, userPos.z);
    
    // // Ориентируем куб по нормали к поверхности
    // glRotatef(-latitude, 0.0f, 0.0f, 1.0f);
    // glRotatef(longitude, 0.0f, 1.0f, 0.0f);
    
    // Размер куба
    float sizeW = 0.01f;
    float sizeH = 0.05f;
    float sizeZ = 0.01f;

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
        glVertex3f(-w, -h, d);
        glVertex3f(-w, h, d);
        glVertex3f(-w, h, -d);
    glEnd();
    
    glPopMatrix();
}

void SunVisualizer::drawSunPath() {
    glColor3f(1.0f, 0.8f, 0.0f);
    glBegin(GL_LINE_STRIP);
    for (const auto& pos : sunPath) {
        Point3fN  p = sphericalToCartesian(pos.azimuth, pos.altitude, 1.2f);
        glVertex3f(p.x, p.y, p.z);
    }
    glEnd();
}

// void SunVisualizer::drawSun() {
//     glPushMatrix();
//     float theta = currentSunPosition.azimuth * M_PI / 180.0f;
//     float phi = currentSunPosition.altitude * M_PI / 180.0f;
//     float r = 1.2f;
//     float x = r * cos(phi) * sin(theta);
//     float y = r * sin(phi);
//     float z = r * cos(phi) * cos(theta);
//     glTranslatef(x, y, z);
//     glColor3f(1.0f, 1.0f, 0.0f);
//     GLUquadric* quad = gluNewQuadric();
//     gluSphere(quad, 0.05, 16, 16);
//     gluDeleteQuadric(quad);
//     glPopMatrix();
// }

void SunVisualizer::drawSun(GLfloat* sunLightPosition) {
    glPushMatrix();
    Point3fN  p = sphericalToCartesian(currentSunPosition.altitude, currentSunPosition.azimuth, 1.2f);

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
    gluSphere(quad, 0.05, 16, 16);
    gluDeleteQuadric(quad);

    glPopMatrix();
}