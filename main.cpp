#include <QApplication>
#include "logging.h"
#include "tray.h"


int main(int argc, char **argv)
{
    DBG("Process started");
    QApplication app (argc, argv);
    app.setQuitOnLastWindowClosed(false);

    Tray tray;

    int a = app.exec();
    DBG("Process stopped");
    return a;

    //return 0;
 }
