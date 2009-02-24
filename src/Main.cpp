
#include "MainWindow.h"

int main(int argc, char * argv[]) {

    // Application
    QApplication app(argc, argv);

    // Window
    MainWindow myWindow;
    myWindow.show();

    return app.exec();

}
