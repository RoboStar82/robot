
#include "settings.h"

Settings settings;

Settings::Settings() : Preferences() {}

Settings::~Settings() {}

void Settings::begin() {
    Preferences::begin("robot");
}

void Settings::end() {
    Preferences::end();
}
