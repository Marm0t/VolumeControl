#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>

namespace Ui {
class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = 0);
    ~Settings();

private:
    Ui::Settings *ui;

signals:
    void volumeChanged(double value);

private slots:
    void slt_sliderValueChanged(int value);


};

#endif // SETTINGS_H
