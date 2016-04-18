#include "about.h"
#include "ui_about.h"
#include <QMessageBox>

About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);
    QString aVersion = "Version: ";
    aVersion.append(APP_VERSION);
    aVersion.append(" (build ");
    aVersion.append(__DATE__);
    aVersion.append(" ");
    aVersion.append(__TIME__);
    aVersion.append(")");
    ui->_versionLabel->setText(aVersion);
    ui->_versionLabel->adjustSize();

    ui->_titleLabel->setText(QString("Volume Control"));
    ui->_titleLabel->adjustSize();

    connect(ui->_qtButton, SIGNAL(clicked(bool)), this, SLOT(showAboutQt()));
    connect(ui->_closeButton, SIGNAL(clicked(bool)), this, SLOT(close()));
}

About::~About()
{
    delete ui;
}

void About::showAboutQt()
{
    QMessageBox::aboutQt(this, "Volume Control");
}
