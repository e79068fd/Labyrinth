#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QMap>
#include <QString>
#include <QVariant>

class Preferences : public QMap<QString, QVariant>{
public:
    static Preferences* instance();
protected:
    Preferences();
private:
    static Preferences* _instance;
};

#endif // PREFERENCES_H
