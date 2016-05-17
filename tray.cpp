#include <QSystemTrayIcon>
#include <QMenu>
#include <QMessageBox>
#include <QApplication>
#include <QCursor>
#include <QStandardPaths>
#include <QDir>
#include "tray.h"
#include "logging.h"
#include "volumechanger.h"
#include <math.h>
#define VK_0 0x30
#define CFG_FILENAME QDir(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)).filePath("vol_ctrl.cfg")

inline double round(double x) { return (floor(x + 0.5)); }

void Tray::muteCb()
{
    bool newVal = !VolumeChanger::Instance().isMute();
    VolumeChanger::Instance().setMute(newVal);
    emit Instance().muteChanged(newVal);
    DBG( (VolumeChanger::Instance().isMute()? "Mute" : "Unmute") );
}

void Tray::minusCb()
{
    double vol = round(VolumeChanger::Instance().getVolume() * 100) / 100;
    if (vol >0)
    {
        VolumeChanger::Instance().setVolume(vol-0.01);
        emit Instance().volumeChanged(vol);
    }
    DBG("Volume decreased. New volume is " << VolumeChanger::Instance().getVolume());
}
void Tray::plusCb()
{
    double vol = round(VolumeChanger::Instance().getVolume() * 100) / 100;
    if (vol <1)
    {
        VolumeChanger::Instance().setVolume(vol+0.01);
        emit Instance().volumeChanged(vol);
    }
    DBG("Volume increased. New volume is " << VolumeChanger::Instance().getVolume());
}


Tray::Tray(QObject *parent)
    : QObject(parent), _iconMenu(NULL), _settingsDialog(NULL), _aboutDialog(NULL), _config(Settings::DEFAULT_CFG)
{
    // Create Icon object
    QIcon icon (":/img/tray_pic.png");
    _icon = new QSystemTrayIcon(icon, this);
    _icon->setToolTip("Volume control");
    _icon->setVisible(true);

    connect(_icon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
             this, SLOT(iconClicked(QSystemTrayIcon::ActivationReason)));

    DBG("Config file: " << CFG_FILENAME);
    if (!_config.loadFromFile(CFG_FILENAME))
        _config = Settings::DEFAULT_CFG;


    // setup of key listener
    _keyLstnr.addKey( _config.mute.nativeVirtualKey(), _config.mute.nativeModifiers(), Tray::muteCb );
    _keyLstnr.addKey( _config.volDown.nativeVirtualKey(), _config.volDown.nativeModifiers(), Tray::minusCb);
    _keyLstnr.addKey( _config.volUp.nativeVirtualKey(), _config.volUp.nativeModifiers(), Tray::plusCb );

    //setup popup windows slots
    connect(this, SIGNAL(volumeChanged(double)), this, SLOT(showVolumePopup()));
    connect(this, SIGNAL(muteChanged(bool)), this, SLOT(showMutePopup()));

    // start key listener in separated thread
    _keyLstnr.start();
}

Tray::~Tray()
{
    _icon->setVisible(false);
}


Tray& Tray::Instance()
{
    static Tray singleton;
    return singleton;
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
        _iconMenu->addAction("&About", this, SLOT(showAboutWindow()));
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
    //QMessageBox::about(0, "About", "Volume Control is a very nice tool.\n"
    //                               "Thanks for using it!\n");
    if(!_aboutDialog)
    {
        _aboutDialog = new About(NULL);
        _aboutDialog->setAttribute( Qt::WA_DeleteOnClose );
        _aboutDialog->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
        connect(_aboutDialog, SIGNAL(destroyed(QObject*)), this, SLOT(finishAboutWindow(QObject*)));
    }
    _aboutDialog->show();
}

void Tray::finishAboutWindow(QObject*)
{
    // All signals are disconnected on delete
    // no need to "delete" because this slot is called on "destroyed" signal
    _aboutDialog = NULL;
    DBG("Finish About");
}

void Tray::showSettingsWindow()
{
    if(!_settingsDialog)
    {
        _settingsDialog = new Settings(NULL, _config);
        _settingsDialog->setAttribute( Qt::WA_DeleteOnClose );
        connect(_settingsDialog, SIGNAL(volumeChanged(double)), this, SLOT(changeVolume(double)));
        connect(_settingsDialog, SIGNAL(destroyed(QObject*)), this, SLOT(finishSettingsWindow(QObject*)));
        connect(_settingsDialog, SIGNAL(configChanged(SettingsConfig_t)), this, SLOT(changeConfig(SettingsConfig_t)));
    }
    _settingsDialog->show();
}

void Tray::finishSettingsWindow(QObject*)
{
    // All signals are disconnected on delete
    // no need to "delete" because this slot is called on "destroyed" signal
    _settingsDialog = NULL;
    DBG("Finish Settings");
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


void Tray::changeConfig(SettingsConfig_t val)
{
    _config = val;
    _keyLstnr.stop();
    _keyLstnr.clear();
    _keyLstnr.addKey( _config.mute.nativeVirtualKey(), _config.mute.nativeModifiers(), muteCb );
    _keyLstnr.addKey( _config.volDown.nativeVirtualKey(), _config.volDown.nativeModifiers(), minusCb);
    _keyLstnr.addKey( _config.volUp.nativeVirtualKey(), _config.volUp.nativeModifiers(), plusCb );
    _keyLstnr.start();

    if (!_keyLstnr.isRunning())
    {
        QMessageBox msgbox(QMessageBox::Warning, "Error", "It seems your hotkeys combination cannot be taken\n"
                                                          "Please choose another hotkey combination",
                           QMessageBox::Ok);
        msgbox.exec();
    }
    if (!QDir(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)).exists())
    {
        if (!QDir().mkdir(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)))
        {
            QMessageBox msgbox(QMessageBox::Warning, "Error", "Cannot create config dir\n"
                                                              "Make sure you have enough space and your filesystem is not read-only",
                               QMessageBox::Ok);
            msgbox.exec();
        }
    }
    if (!_config.saveToFile(CFG_FILENAME))
    {
        QMessageBox msgbox(QMessageBox::Warning, "Error", "Cannot save your config to file\n"
                                                          "Make sure you have enough space and your filesystem is not read-only",
                           QMessageBox::Ok);
        msgbox.exec();
    }
}

void Tray::showVolumePopup()
{
    //QMessageBox::information(0, "Volume Changer", "Volume: "+QString::number(VolumeChanger::Instance().getVolume()));
}

void Tray::showMutePopup()
{
    //QMessageBox::information(0, "Volume Changer", (VolumeChanger::Instance().isMute())?"Mute":"UnMute");
}
