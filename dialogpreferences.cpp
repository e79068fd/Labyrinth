#include "dialogpreferences.h"
#include "ui_dialogpreferences.h"

DialogPreferences::DialogPreferences(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogPreferences)
{
    ui->setupUi(this);
    ui->edit_length->setValidator(new QIntValidator(0, 10));
    ui->edit_width->setValidator(new QIntValidator(0, 10));

    Preferences* pref = Preferences::instance();
    ui->edit_length->setText(QString::number(pref->value("length").toInt()));
    ui->edit_width->setText(QString::number(pref->value("width").toInt()));
    connect(this, SIGNAL(accepted()), this, SLOT(save()));
}

DialogPreferences::~DialogPreferences()
{
    delete ui;
}

void DialogPreferences::save() {
    Preferences* pref = Preferences::instance();
    pref->insert("length", QVariant(ui->edit_length->text().toInt()));
    pref->insert("width", QVariant(ui->edit_width->text().toInt()));
    pref->save();
}
