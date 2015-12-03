#ifndef TRAY_H
#define TRAY_H

#include <QWidget>
#include <QSystemTrayIcon>
#include "settings.h"
#include "keylistener.h"


class QSystemTrayIcon;
class QMenu;

class Tray : public QObject
{
    Q_OBJECT
public:
    explicit Tray(QObject *parent = 0);

private:

    QSystemTrayIcon* _icon;
    QMenu* _iconMenu;
    QAction* _statusAction;
    Settings* _settingsDialog;

    KeyListener _keyLstnr;



signals:

public slots:
    void updateStatus();

private slots:
    void iconClicked(QSystemTrayIcon::ActivationReason iReason);
    void msgClicked();

    void showMenu();
    void finishMenu(QObject*);

    void showAboutWindow();

    void showSettingsWindow();
    void finishSettingsWindow(QObject* );

    void changeVolume(double val);

};

#endif // TRAY_H
