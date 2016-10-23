#include "preferences.h"

Preferences::Preferences() {
    read("preferences.json");
}

Preferences::~Preferences() {
    save();
}

Preferences* Preferences::_instance = 0;

Preferences* Preferences::instance() {
    if(_instance == 0)
        _instance = new Preferences;
    return _instance;
}

void Preferences::byDefault() {
    clear();
    read(":/defaultPreferences.json");
}

void Preferences::read(const QString& filename) {
    QFile* file = new QFile(filename);
    if(!file->open(QFile::ReadOnly)) {
        qDebug() << "open default";
        file = new QFile(":/defaultPreferences.json");
        file->open(QFile::ReadOnly);
    }

    QJsonDocument loadDoc(QJsonDocument::fromJson(file->readAll()));
    unite(loadDoc.object().toVariantMap());
}

void Preferences::save() {
    QFile file("preferences.json");
    if(!file.open(QFile::WriteOnly)) {
        return;
    }

    QJsonDocument saveDoc(QJsonObject::fromVariantMap(*this));
    file.write(saveDoc.toJson());
}
