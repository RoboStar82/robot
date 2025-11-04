
#include "settings.h"

Settings settings;

void Settings::begin() {
    Preferences::begin("robot");
}
