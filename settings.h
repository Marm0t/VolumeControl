#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QKeySequenceEdit>
#include <QKeyEvent>

namespace Ui {
class Settings;
}


typedef struct
{
    QKeyEvent mute;
    QKeyEvent volDown;
    QKeyEvent volUp;

} SettingsConfig_t;

class Settings : public QDialog
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = 0, SettingsConfig_t& config = Settings::DEFAULT_CFG );
    ~Settings();

    const SettingsConfig_t& getConfig(){return _config;}

    static SettingsConfig_t DEFAULT_CFG;

private:
    Ui::Settings *ui;
    SettingsConfig_t _config = DEFAULT_CFG; //Thanks C++11!

signals:
    void volumeChanged(double value);
    void configChanged(SettingsConfig_t value);


private slots:
    void slt_sliderValueChanged(int value);

    void slt_muteChanged(QKeyEvent event);
    void slt_volDownChanged(QKeyEvent event);
    void slt_volUpChanged(QKeyEvent event);

    void accept();
};

class SettingsKeySequence : public QKeySequenceEdit
{
    Q_OBJECT
public:
    SettingsKeySequence(QWidget *parent = 0):QKeySequenceEdit(parent){}

signals:
    void hotKeysReady(QKeyEvent event);

protected:
    void keyPressEvent(QKeyEvent * event);
};

#endif // SETTINGS_H
