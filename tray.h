#ifndef TRAY_H
#define TRAY_H

#include <QWidget>
#include <QSystemTrayIcon>


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

signals:

public slots:
    void updateStatus();

private slots:
    void iconClicked(QSystemTrayIcon::ActivationReason iReason);
    void msgClicked();

    void showAboutWindow();
    void showSettingsWindow();

};

#endif // TRAY_H
