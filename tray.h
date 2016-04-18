#ifndef TRAY_H
#define TRAY_H

#include <QSystemTrayIcon>
#include "settings.h"
#include "about.h"
#include "keylistener.h"


class QSystemTrayIcon;
class QMenu;


/**
 * @brief The Tray class.
 *        Works as "main loop" of the application and also represents tray icon.
 *        Responcible for KeyListener execution, configuration updates deployment,
 *        interaction with user.
 */
class Tray : public QObject
{
    Q_OBJECT
public:
    explicit Tray(QObject *parent = 0);
    ~Tray();

private:

    QSystemTrayIcon* _icon;
    QMenu* _iconMenu;
    QAction* _statusAction;
    Settings* _settingsDialog;
    About* _aboutDialog;

    KeyListener _keyLstnr;
    SettingsConfig_t _config;

signals:

public slots:
    void updateStatus();

private slots:
    void iconClicked(QSystemTrayIcon::ActivationReason iReason);
    void msgClicked();

    void showMenu();
    void finishMenu(QObject*);

    void showAboutWindow();
    void finishAboutWindow(QObject* );

    void showSettingsWindow();
    void finishSettingsWindow(QObject* );

    void changeVolume(double val);
    void changeConfig(SettingsConfig_t val);

};

#endif // TRAY_H
