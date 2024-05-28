#include "iniparser/src/iniparser.h"
#include "iniparser/src/dictionary.h"

enum LocationType {
    GCLUE,
    MANUAL,
};

dictionary *load_config(void);

enum LocationType config_get_location_type(dictionary *d);

float config_get_latitude (dictionary *d);
float config_get_longitude (dictionary *d);
