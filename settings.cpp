#include "settings.h"
#include "ui_settings.h"
#include "volumechanger.h"
#include "logging.h"
#include "settings.h"
#include <QKeyEvent>
#include <windows.h>
#include <QFile>
#include <QMessageBox>
#include <QSettings>
#include <QDir>

#define FILE_HEADER  0x5D6A1F
#define FILE_VERSION 0x01

SettingsConfig_t Settings::DEFAULT_CFG = {
    QKeyEvent(QEvent::KeyPress, Qt::Key_0,     Qt::AltModifier|Qt::ShiftModifier, 0,  48, 517),
    QKeyEvent(QEvent::KeyPress, Qt::Key_Minus, Qt::AltModifier|Qt::ShiftModifier, 0, 189, 517),
    QKeyEvent(QEvent::KeyPress, Qt::Key_Equal, Qt::AltModifier|Qt::ShiftModifier, 0, 187, 517)
};

Settings::Settings(QWidget *parent, SettingsConfig_t& config) :
    QDialog(parent),
    ui(new Ui::Settings),
    _config(config)
{
    this->setFixedSize(this->sizeHint());
    ui->setupUi(this);

    double aVol = VolumeChanger::Instance().isMute()?0 :VolumeChanger::Instance().getVolume();
    ui->_volumeSlider->setValue( aVol*100 );
    ui->_volumeLevelLabel->setText(QString::number(aVol));

    ui->_keyMute->   setKeySequence(QKeySequence(_config.mute.modifiers()|_config.mute.key()));
    ui->_keyVolDown->setKeySequence(QKeySequence(_config.volDown.modifiers()|_config.volDown.key()));
    ui->_keyVolUp->  setKeySequence(QKeySequence(_config.volUp.modifiers()|_config.volUp.key()));

    ui->_autorunCheckBox->setChecked(isAutoRun());

    // internal connections
    connect(ui->_volumeSlider, SIGNAL(valueChanged(int)),     this, SLOT(slt_sliderValueChanged(int)));
    connect(ui->_keyMute, SIGNAL(hotKeysReady(QKeyEvent)),    this, SLOT(slt_muteChanged(QKeyEvent)));
    connect(ui->_keyVolDown, SIGNAL(hotKeysReady(QKeyEvent)), this, SLOT(slt_volDownChanged(QKeyEvent)));
    connect(ui->_keyVolUp, SIGNAL(hotKeysReady(QKeyEvent)),   this, SLOT(slt_volUpChanged(QKeyEvent)));

}

Settings::~Settings()
{
    delete ui;
}

// Private slots
void Settings::slt_sliderValueChanged(int value)
{
    double vald = (double)value/100;
    ui->_volumeLevelLabel->setText(QString::number(vald));
    emit volumeChanged(vald);
}

void Settings::slt_muteChanged(QKeyEvent value)
{
    _config.mute = value;
}

void Settings::slt_volDownChanged(QKeyEvent value)
{
    _config.volDown = value;
}

void Settings::slt_volUpChanged(QKeyEvent value)
{
    _config.volUp = value;
}


void Settings::setAutoRun(bool iAutoRun)
{
    QSettings autorun("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",QSettings::NativeFormat);
    if(iAutoRun)
    {
        autorun.setValue(QCoreApplication::applicationName(), QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));
        autorun.sync();
    }
    else
    {
        autorun.remove(QCoreApplication::applicationName());

    }
}

bool Settings::isAutoRun()
{
    QSettings autorun("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",QSettings::NativeFormat);
    return autorun.contains(QCoreApplication::applicationName());
}

void Settings::accept()
{
    if (!_config.isCorrect())
    {
        QMessageBox msgbox(QMessageBox::Warning, "Error", "All hotkeys combination must be different :)");
        msgbox.exec();
        return;
    }
    DBG("New config accepted:\n"
        "\tmute       \t" << QKeySequence(_config.mute.modifiers()|_config.mute.key()).toString() << "\n"
        "\tvolume down\t"<< QKeySequence(_config.volDown.modifiers()|_config.volDown.key()).toString() <<"\n"
        "\tvolume up  \t"<< QKeySequence(_config.volUp.modifiers()|_config.volUp.key()).toString() << "\n"
        "\tAutorun    \t"<< ui->_autorunCheckBox->isChecked());

    setAutoRun(ui->_autorunCheckBox->isChecked());

    emit configChanged(_config);
    QDialog::accept();
}


void SettingsKeySequence::keyPressEvent(QKeyEvent *event)
{
    setKeySequence(QKeySequence(event->modifiers()|event->key()).toString().trimmed());
    if (!this->keySequence().toString().isEmpty())
    {
        // Problem: NativeModifiers()&0xFF returns SHIFT instead of ALT and ALT intead of SHIFT
        // that's why we have to exchange bitsnumber 0 and 2 manually
        int num = event->nativeModifiers();
        if ((num ^ num >> 2) & 1)
                num^=1 | 1 << 2;
        //DBG("key: " << event->nativeVirtualKey() <<"\tModifiers: "<<  (num & 0xFF) << "\t Scancode:" << event->nativeScanCode());

        emit hotKeysReady(QKeyEvent(event->type(),
                                    event->key(),
                                    event->modifiers(),
                                    event->nativeScanCode(),
                                    event->nativeVirtualKey(),
                                    num,
                                    event->text()));
    }
}


QDataStream &operator<<(QDataStream &out, const SettingsConfig_t &settingsConfig)
{
    out << settingsConfig.mute.type() << settingsConfig.mute.key() << settingsConfig.mute.modifiers() <<
           settingsConfig.mute.nativeScanCode() << settingsConfig.mute.nativeVirtualKey() << settingsConfig.mute.nativeModifiers();
    out << settingsConfig.volDown.type() << settingsConfig.volDown.key() << settingsConfig.volDown.modifiers() <<
           settingsConfig.volDown.nativeScanCode() << settingsConfig.volDown.nativeVirtualKey() << settingsConfig.volDown.nativeModifiers();
    out << settingsConfig.volUp.type() << settingsConfig.volUp.key() << settingsConfig.volUp.modifiers() <<
           settingsConfig.volUp.nativeScanCode() << settingsConfig.volUp.nativeVirtualKey() << settingsConfig.volUp.nativeModifiers();
    return out;
}

QDataStream &operator>>(QDataStream &in, SettingsConfig_t &settingsConfig)
{
    int type;
    int key;
    int modifiers;
    quint32 nativeScanCode;
    quint32 nativeVirtualKey;
    quint32 nativeModifiers;
    in >> type >> key >> modifiers >> nativeScanCode >> nativeVirtualKey >> nativeModifiers;
    QKeyEvent mute(QEvent::Type(type), key, Qt::KeyboardModifiers(modifiers), nativeScanCode, nativeVirtualKey, nativeModifiers);
    in >> type >> key >> modifiers >> nativeScanCode >> nativeVirtualKey >> nativeModifiers;
    QKeyEvent volDown(QEvent::Type(type), key, Qt::KeyboardModifiers(modifiers), nativeScanCode, nativeVirtualKey, nativeModifiers);
    in >> type >> key >> modifiers >> nativeScanCode >> nativeVirtualKey >> nativeModifiers;
    QKeyEvent volUp (QEvent::Type(type), key, Qt::KeyboardModifiers(modifiers), nativeScanCode, nativeVirtualKey, nativeModifiers);
    settingsConfig = {mute, volDown, volUp};
    return in;
}

bool SettingsConfig::saveToFile(const QString &filename)
{
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly))
    {
        ERR("Cannot save config to file " << filename);
        return false;
    }
    QDataStream out(&file);
    out << (quint32)FILE_HEADER;
    out << (quint32)FILE_VERSION;
    out.setVersion(QDataStream::Qt_5_5);
    out << *this;
    file.flush();
    file.close();
    return true;
}

bool SettingsConfig::loadFromFile(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        ERR("Cannot open file " << filename << " to load config");
        return false;
    }
    QDataStream in(&file);

    // Read and check the header
    quint32 magic;
    in >> magic;
    if (magic != FILE_HEADER)
    {
        ERR("Cannot load config from file " << filename << ": bad file format");
        return false;
    }

    // Read the version
    qint32 version;
    in >> version;
    if (version != FILE_VERSION)
    {
        ERR("Cannot load config from file " << filename << ": supported file version: " << FILE_VERSION
            << ", version in file: " << version);
        return false;
    }
    in.setVersion(QDataStream::Qt_5_5);

    // Read the data
    in >> *this;

    file.close();
    return true;
}

bool SettingsConfig::isCorrect()
{
    return !((mute.key() == volUp.key() && mute.modifiers() == volUp.modifiers())        ||
            (mute.key() == volDown.key() && volDown.modifiers() == volDown.modifiers()) ||
            (volDown.key() == volUp.key() && volDown.modifiers() == volUp.modifiers()));
}


