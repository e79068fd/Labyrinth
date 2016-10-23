#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QDebug>
#include <QMap>
#include <QString>
#include <QVariant>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QByteArray>

class Preferences : public QVariantMap {
public:
    ~Preferences();
    static Preferences* instance();
    void byDefault();
    void save();
protected:
    Preferences();
private:
    static Preferences* _instance;
    void read(const QString& filename);
};

#endif // PREFERENCES_H
