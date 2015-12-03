#include "settings.h"
#include "ui_settings.h"
#include "volumechanger.h"
#include "logging.h"
#include "settings.h"
#include <Windows.h>
#include <QKeyEvent>

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    this->setFixedSize(this->sizeHint());
    ui->setupUi(this);

    double aVol = VolumeChanger::Instance().isMute()?0 :VolumeChanger::Instance().getVolume();
    ui->_volumeSlider->setValue( aVol*100 );
    ui->_volumeLevelLabel->setText(QString::number(aVol));

    ui->_keyMute->   setKeySequence(QKeySequence(Qt::AltModifier + Qt::ShiftModifier + Qt::Key_0));
    ui->_keyVolDown->setKeySequence(QKeySequence(Qt::AltModifier + Qt::ShiftModifier + Qt::Key_Minus));
    ui->_keyVolUp->  setKeySequence(QKeySequence(Qt::AltModifier + Qt::ShiftModifier + Qt::Key_Equal));

    //TODO: delete this when Settings dialog is ready
    ui->_keyMute->setDisabled(true);
    ui->_keyVolDown->setDisabled(true);
    ui->_keyVolUp->setDisabled(true);

    // internal connections
    connect(ui->_volumeSlider, SIGNAL(valueChanged(int)), this, SLOT(slt_sliderValueChanged(int)));

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

