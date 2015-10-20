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
    QIcon icon ("D:\\tmp\\icon.png");
    _icon = new QSystemTrayIcon(icon, this);
    _icon->setToolTip("Volume control");
    _icon->setVisible(true);

    //connect(_icon, SIGNAL(messageClicked()), this, SLOT(msgClicked()));
    connect(_icon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconClicked(QSystemTrayIcon::ActivationReason)));

    // Create Icon Menu for right-click
    _iconMenu = new QMenu(0);
    _iconMenu->setTitle("Volume Cntrol");
    _statusAction = _iconMenu->addAction(QString("Volume: %1%").arg(22));
    _statusAction->setEnabled(false);
    _iconMenu->addSeparator();
    _iconMenu->addAction("Settings", this, SLOT(showSettingsWindow()));
    _iconMenu->addSeparator();
    _iconMenu->addAction("&About", this, SLOT(showAboutWindow()));
    _iconMenu->addAction("&Quit", QApplication::instance(), SLOT(quit()), QKeySequence::Quit );
    connect(_iconMenu, SIGNAL(aboutToShow()), this, SLOT(updateStatus()));
}


void Tray::iconClicked(QSystemTrayIcon::ActivationReason iReason)
{
    switch(iReason)
    {
    case QSystemTrayIcon::Trigger:        // single left click
        //_icon->showMessage("Volume controle", "Current volume: ");
        DBG("Icon clicked with left button" );

        break;

    case QSystemTrayIcon::Context:        // right click
        // show menu
        _iconMenu->exec(QCursor::pos());
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
    QMessageBox::about(0, "Volume Control", "Settings will be here!\n\n"
                                            "Now you can enjoy hardcoded hotkeys:\n\n"
                                            "\t<Alt>+<Shift>+<=> - increase the volume\t\n"
                                            "\t<Alt>+<Shift>+<-> - decrease the volume\t\n"
                                            "\t<Alt>+<Shift>+<0> - mute/unmute\t\n");
}


void Tray::updateStatus()
{
    QString mute = (VolumeChanger::Instance().isMute())?" (MUTE)":"";
    _statusAction->setText(QString("Volume: %1%%2").arg(VolumeChanger::Instance().getVolume()*100).arg(mute));
}


