#include "preferences.h"

Preferences::Preferences() : QMap<QString,QVariant>() {

}

Preferences* Preferences::_instance = 0;

Preferences* Preferences::instance() {
    if(_instance == 0)
        _instance = new Preferences;
    return _instance;
}
