#include "settings.h"
#include "ui_settings.h"
#include "volumechanger.h"
#include "logging.h"
#include "settings.h"
#include <QKeyEvent>
#include <windows.h>

/*const QString & text = QString(), bool autorep = false, ushort count = 1)
 */
SettingsConfig_t Settings::DEFAULT_CFG = {
    QKeyEvent(QEvent::KeyPress, Qt::Key_0, Qt::AltModifier|Qt::ShiftModifier, 0, 48, 517),
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

void Settings::accept()
{
    DBG("New config accepted:\n"
        "\tmute\t" << QKeySequence(_config.mute.modifiers()|_config.mute.key()) << "\n"
        "\tvolume down\t"<< QKeySequence(_config.volDown.modifiers()|_config.volDown.key()) <<"\n"
        "\tvolume up\t"<< QKeySequence(_config.volUp.modifiers()|_config.volUp.key()));
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

