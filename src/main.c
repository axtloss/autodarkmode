/* main.c
 *
 * Copyright 2024 Unknown
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

#include "config.h"
#include "configuration.h"
#include "sun.h"

#include <string.h>
#include <math.h>
#include <signal.h>
#include <glib.h>
#include <stdlib.h>
#include <geoclue.h>

static GClueAccuracyLevel accuracy_level = GCLUE_ACCURACY_LEVEL_EXACT;

GClueSimple *simple = NULL;
GClueClient *client = NULL;
GMainLoop *main_loop;

enum LocationType loctype = 0;
dictionary *dict = NULL;
bool quit = false;
float lat = 0;
float lng = 0;

static void
get_gclue_location (GClueSimple *simple)
{
    GClueLocation *location;
    location = gclue_simple_get_location (simple);
    lat = gclue_location_get_latitude (location);
    lng = gclue_location_get_longitude (location);

    return;
}

void
on_dark (void)
{
    char *home = getenv ("HOME");
    char *darklock;
    if (home != NULL) {
        darklock = malloc(strlen(home)*sizeof(char)+strlen("/.cache/darkmode")*sizeof(char)+1);
        sprintf (darklock, "%s/.cache/darkmode", home);
        if (access(darklock, F_OK) == 0) {
            free (darklock);
            return;
        }
    }

    const char *dark_command = iniparser_getstring (dict, "dark:cmd", "");
    if (sizeof (dark_command) == sizeof ("")) {
        g_printerr ("No command for dark mode specified!");
        return;
    }
    int rtrn = system (dark_command);
    if (rtrn != 0) {
        g_printerr("Dark command failed with non-zero exit code!");
        free (darklock);
        return;
    }
    FILE *dark = fopen (darklock, "w");
    fclose (dark);
}

void
on_light (void)
{
    char *home = getenv ("HOME");
    char *darklock;
    if (home != NULL) {
        darklock = malloc(strlen(home)*sizeof(char)+strlen("/.cache/darkmode")*sizeof(char)+1);
        sprintf (darklock, "%s/.cache/darkmode", home);
        if (access(darklock, F_OK) != 0) {
            free (darklock);
            return;
        }

    }

    const char *light_command = iniparser_getstring (dict, "light:cmd", "");
    if (sizeof (light_command) == sizeof ("")) {
        g_printerr ("No command for light mode specified!");
        return;
    }
    int rtrn = system (light_command);
    if (rtrn != 0) {
        g_printerr("Light command failed with non-zero exit code!");
        free (darklock);
        return;
    }

    remove (darklock);
    free (darklock);
}

void
loop (void)
{
        float sunrise = getSunrise (lat, lng);
        double hours;
        float minutes = modf (sunrise, &hours)*60;
        g_print ("Sunrise: %.0f:%.0f\n", hours, minutes);

        float sunset = getSunset (lat, lng);
        minutes = modf (sunset, &hours)*60;
        g_print ("Sunset: %.0f:%.0f\n", hours, minutes);

        if (isDark (sunrise, sunset))
            on_dark ();
        else
            on_light ();
}

void
handleExit(int sig) {
    quit = true;
    signal(sig, SIG_IGN);
    return;
}

gint
main (gint   argc,
      gchar *argv[])
{

    dict = load_config ();
    loctype = config_get_location_type (dict);
    g_print("%d\n", loctype);

    if (loctype == MANUAL) {
        lat = config_get_latitude (dict);
        lng = config_get_longitude (dict);
    } else {
        get_gclue_location (gclue_simple_new_sync ("autodarkmode",
                            accuracy_level,
                            NULL,
                            NULL));
        g_print ("lat: %f\n", lat);
        g_print ("lng: %f\n", lng);
    }

    signal(SIGINT, handleExit);
    while (!quit) {
        loop();
        sleep (60);
    }

    iniparser_freedict (dict);

	return EXIT_SUCCESS;
}
