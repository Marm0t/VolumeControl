#include <QSystemTrayIcon>
#include <QMenu>
#include <QMessageBox>
#include <QApplication>
#include <QCursor>
#include "tray.h"
#include "logging.h"
#include "volumechanger.h"

Tray::Tray(QObject *parent) : QObject(parent)
{
    // Create Icon object
    QIcon icon (":/img/tray_pic.png");
    _icon = new QSystemTrayIcon(icon, this);
    _icon->setToolTip("Volume control");
    _icon->setVisible(true);

    connect(_icon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
             this, SLOT(iconClicked(QSystemTrayIcon::ActivationReason)));

    // Create Icon Menu for right-click
    _iconMenu = new QMenu(0);
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

void Tray::showMenu()
{
    _iconMenu->exec(QCursor::pos());
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
        connect(_settingsDialog, SIGNAL(volumeChanged(double)), this, SLOT(changeVolume(double)));
    }
    _settingsDialog->show();
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

