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
    static Tray& Instance();

private:
    explicit Tray(QObject *parent = 0);
    ~Tray();

    QSystemTrayIcon* _icon;
    QMenu* _iconMenu;
    QAction* _statusAction;
    Settings* _settingsDialog;
    About* _aboutDialog;
    QWidget *_popupWidget;
    QTimer *_popupTimer;

    KeyListener _keyLstnr;
    SettingsConfig_t _config;

    static void muteCb();
    static void minusCb();
    static void plusCb();

signals:
    void volumeChanged(double value);
    void muteChanged(bool value);

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

    void showVolumePopup();
    void showMutePopup();
    void showMsgPopup(const QString& iMessage);
    void finishPopupWidget(QObject* );

};

#endif // TRAY_H
