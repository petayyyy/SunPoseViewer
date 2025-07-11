#include <QApplication>
#include "MainWindow.h"

int windowRun(){
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return app.exec();
}
int main(int argc, char *argv[]) {
    return windowRun();
}