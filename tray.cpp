#include <QSystemTrayIcon>
#include <QMenu>
#include <QMessageBox>
#include <QApplication>
#include <QCursor>
#include "tray.h"
#include "logging.h"
#include "volumechanger.h"
#include <math.h>
#define VK_0 0x30

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


Tray::Tray(QObject *parent)
: QObject(parent), _iconMenu(NULL), _settingsDialog(NULL)
{
    // Create Icon object
    QIcon icon (":/img/tray_pic.png");
    _icon = new QSystemTrayIcon(icon, this);
    _icon->setToolTip("Volume control");
    _icon->setVisible(true);

    connect(_icon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
             this, SLOT(iconClicked(QSystemTrayIcon::ActivationReason)));


    // setup of key listener
    _keyLstnr.addKey(VK_0,         muteCb );
    _keyLstnr.addKey(VK_OEM_PLUS,  plusCb );
    _keyLstnr.addKey(VK_OEM_MINUS, minusCb);
    _keyLstnr.start(); // starts new thread
}


void Tray::iconClicked(QSystemTrayIcon::ActivationReason iReason)
{
    switch(iReason)
    {
    case QSystemTrayIcon::Trigger:        // single left click
        //_icon->showMessage("Volume control", "Current volume: " + QString::number(VolumeChanger::Instance().getVolume()));
        DBG("Icon clicked with left button" );
        break;

    case QSystemTrayIcon::Context:        // right click
        showMenu();
        break;

    case QSystemTrayIcon::DoubleClick:    // double click
        showSettingsWindow();
        break;

    case QSystemTrayIcon::MiddleClick:
        DBG("Icon clicked with middle button");
        VolumeChanger::Instance().setMute(!VolumeChanger::Instance().isMute());
        break;
    case QSystemTrayIcon::Unknown:
    default:
        DBG("Nothing to do for tray Icon activation");
        break;
    }
}


void Tray::showMenu()
{
    // Create Icon Menu for right-click
    if (!_iconMenu)
    {
        _iconMenu = new QMenu(0);
        _iconMenu->setAttribute( Qt::WA_DeleteOnClose );
        connect(_iconMenu, SIGNAL(destroyed(QObject*)), this, SLOT(finishMenu(QObject*)));

        _iconMenu->setTitle("Volume Cntrol");
        _statusAction = _iconMenu->addAction(QString("Volume: %1%").arg(22));
        _statusAction->setEnabled(false);
        _iconMenu->addSeparator();
        _iconMenu->addAction("Settings", this, SLOT(showSettingsWindow()));
        _iconMenu->addSeparator();
        //_iconMenu->addAction("&About", this, SLOT(showAboutWindow()));
        _iconMenu->addAction("&Quit", QApplication::instance(), SLOT(quit()), QKeySequence::Quit );
        connect(_iconMenu, SIGNAL(aboutToShow()), this, SLOT(updateStatus()));
    }
    _iconMenu->exec(QCursor::pos());
}

void Tray::finishMenu(QObject * )
{
    // don't "delete" because this slot is called on "destroyed" signal
    _iconMenu = NULL;
    DBG("FinishMenu");
}

void Tray::msgClicked()
{
    DBG("Message clicked");
}


void Tray::showAboutWindow()
{
    QMessageBox::about(0, "About", "Volume Control is a very nice tool.\n"
                                   "Thanks for using it!\n");
}


void Tray::showSettingsWindow()
{
    if(!_settingsDialog)
    {
        _settingsDialog = new Settings();
        _settingsDialog->setAttribute( Qt::WA_DeleteOnClose );
        connect(_settingsDialog, SIGNAL(volumeChanged(double)), this, SLOT(changeVolume(double)));
        connect(_settingsDialog, SIGNAL(destroyed(QObject*)), this, SLOT(finishSettingsWindow(QObject*)));
    }
    _settingsDialog->show();
}

void Tray::finishSettingsWindow(QObject*)
{
    // All signals are disconnected on delete
    // no need to "delete" because this slot is called on "destroyed" signal
    _settingsDialog = NULL;
}


void Tray::updateStatus()
{
    QString mute = (VolumeChanger::Instance().isMute())?" (MUTE)":"";
    _statusAction->setText(QString("Volume: %1%%2").arg(VolumeChanger::Instance().getVolume()*100).arg(mute));
}


void Tray::changeVolume(double val)
{
    VolumeChanger::Instance().setVolume(val);
}




