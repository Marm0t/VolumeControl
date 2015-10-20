#include <QApplication>
#include <math.h>
#include "logging.h"
#include "tray.h"
#include "VolumeChanger.h"
#include "KeyListener.h"
#define VK_0 0x30

using namespace std;

inline double round(double x) { return (floor(x + 0.5)); }

void muteCb()
{//case 0x30: // 0 - MUTE
    VolumeChanger::Instance().setMute(!VolumeChanger::Instance().isMute());
    DBG( (VolumeChanger::Instance().isMute()? "Mute" : "Unmute") );
}

void minusCb()
{// case VK_OEM_MINUS:
    double vol = round(VolumeChanger::Instance().getVolume() * 100) / 100;
    if (vol >0) VolumeChanger::Instance().setVolume(vol-0.01);
    DBG("Volume decreased. New volume is " << VolumeChanger::Instance().getVolume());
}
void plusCb()
{// case VK_OEM_PLUS:
    double vol = round(VolumeChanger::Instance().getVolume() * 100) / 100;
    if (vol <1) VolumeChanger::Instance().setVolume(vol+0.01);
    DBG("Volume increased. New volume is " << VolumeChanger::Instance().getVolume());
}

int main(int argc, char **argv)
{

    DBG("Process started");
    QApplication app (argc, argv);
    app.setQuitOnLastWindowClosed(false);

    Tray tray;

    KeyListener aKeyLstnr;
    aKeyLstnr.addKey(VK_0,         muteCb );
    aKeyLstnr.addKey(VK_OEM_PLUS,  plusCb );
    aKeyLstnr.addKey(VK_OEM_MINUS, minusCb);
    aKeyLstnr.start(); // starts new thread



    int a = app.exec();
    DBG("Process stopped");
    return a;

    //return 0;
 }
