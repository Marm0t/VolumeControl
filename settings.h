#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QKeySequenceEdit>
#include <QKeyEvent>
#include <QDataStream>

namespace Ui {
class Settings;
}


typedef struct SettingsConfig
{
    QKeyEvent mute;
    QKeyEvent volDown;
    QKeyEvent volUp;

    bool isCorrect();
    bool saveToFile(const QString& filename);
    bool loadFromFile(const QString& filename);

} SettingsConfig_t;

QDataStream &operator<<(QDataStream &out, const SettingsConfig_t &settingsConfig);
QDataStream &operator>>(QDataStream &in, SettingsConfig_t &settingsConfig);

class Settings : public QDialog
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = 0, SettingsConfig_t& config = Settings::DEFAULT_CFG );
    ~Settings();

    const SettingsConfig_t& getConfig(){return _config;}

    static SettingsConfig_t DEFAULT_CFG;
    static void setAutoRun(bool autorun);
    static bool isAutoRun();

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
