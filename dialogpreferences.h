#ifndef DIALOGPREFERENCES_H
#define DIALOGPREFERENCES_H

#include "preferences.h"

#include <QDialog>
#include <QValidator>

namespace Ui {
class DialogPreferences;
}

class DialogPreferences : public QDialog
{
    Q_OBJECT

public:
    explicit DialogPreferences(QWidget *parent = 0);
    ~DialogPreferences();

public slots:
    void save();

private:
    Ui::DialogPreferences *ui;
};

#endif // DIALOGPREFERENCES_H
