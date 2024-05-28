/* configuration.c
 *
 * Copyright 2024 axtlos <axtlos@disroot.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License only.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "configuration.h"

#include "iniparser/src/iniparser.h"
#include "iniparser/src/dictionary.h"
#include "extString.h"
#include <string.h>
#include <glib.h>
#include <stdlib.h>
#include <unistd.h>

dictionary *
load_config(void)
{
    char *xdg_config = getenv ("XDG_CONFIG_HOME");
    char *home = getenv ("HOME");
    if (home == NULL)
        return NULL;
    if (xdg_config == NULL) {
        xdg_config = malloc (strlen(home)*sizeof(char)+strlen("/.config")*sizeof(char)+1);
        sprintf (xdg_config, "%s/.config", home);
    }
    char *config_path = malloc(strlen(xdg_config)*sizeof(char)+strlen("/autodarkmode/config.ini")*sizeof(char)+1);
    sprintf (config_path, "%s/autodarkmode/config.ini", xdg_config);

    free (xdg_config);

    if (access(config_path, F_OK) != 0) {
        g_printerr ("Config file not found. Using default values.\n");
        free (config_path);
        return NULL;
    }

    g_print ("Loading config file %s\n", config_path);

    dictionary *dict = iniparser_load (config_path);

    free (config_path);
    return dict;
}

enum LocationType
config_get_location_type(dictionary *d)
{
    if (d == NULL)
        return GCLUE;

    char *loctype = iniparser_getstring (d, "main:locationtype", "gclue");

    if (strcmp(strlwr(loctype), "gclue") == 0) {
        return GCLUE;
    } else if (strcmp(strlwr(loctype), "manual") == 0) {
        return MANUAL;
    } else {
        g_printerr("Invalid Location type %s. Defaulting to GeoClue\n", loctype);
        return GCLUE;
    }
    return GCLUE;
}

float
config_get_latitude (dictionary *d)
{
    if (d == NULL)
        return 0;

    return iniparser_getdouble (d, "manual:latitude", 0);
}

float
config_get_longitude (dictionary *d)
{
    if (d == NULL)
        return 0;

    return iniparser_getdouble (d, "manual:longitude", 0);
}

