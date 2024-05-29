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
bool firstrun = true;
bool isdark = false;
float lat = 0;
float lng = 0;

static void
get_gclue_location (GClueSimple *simple)
{
    GClueLocation *location;
    location = gclue_simple_get_location (simple);

    if (!location) {
      g_printerr ("Failed to get location through geoclue");
      exit (1);
    }

    lat = gclue_location_get_latitude (location);
    lng = gclue_location_get_longitude (location);

    return;
}

void
on_dark (void)
{
    char *home = getenv ("HOME");
    char *darklock = malloc(strlen(home)*sizeof(char)+strlen("/.cache/darkmode")*sizeof(char)+1);
    sprintf (darklock, "%s/.cache/darkmode", home);
    if (firstrun) {
      if (home != NULL) {
          if (access(darklock, F_OK) == 0) {
              free (darklock);
              isdark = true;
              return;
          }
      }
    } else if (isdark) {
      return;
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
    free (darklock);
    isdark = true;
}

void
on_light (void)
{
    char *home = getenv("HOME");
    char *darklock = malloc(strlen(home)*sizeof(char)+strlen("/.cache/darkmode")*sizeof(char)+1);
    sprintf (darklock, "%s/.cache/darkmode", home);
    if (firstrun) {
      if (home != NULL) {
          if (access(darklock, F_OK) != 0) {
              free (darklock);
              isdark = true;
              return;
          }
      }
    } else if (!isdark) {
      return;
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
    isdark = false;
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
        firstrun = false;
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
    //g_print ("%s\n", argv[1]);

    g_autoptr(GError) error = NULL;
    g_autoptr(GOptionContext) context = NULL;

    gdouble locLat = 100;
    gdouble locLng = 100;

    GOptionEntry entries[] = {
        { "latitude", 'l', 0, G_OPTION_ARG_DOUBLE, &locLat, "Override the latitude", "lat" },
        { "longitude", 'o', 0, G_OPTION_ARG_DOUBLE, &locLng, "Override the longitude", "long" },
        G_OPTION_ENTRY_NULL
    };

    context = g_option_context_new ("- automatically switch between dark and light mode");
    g_option_context_add_main_entries (context, entries, NULL);
    if (!g_option_context_parse (context, &argc, &argv, &error)) {
        g_print ("option parsing failed: %s\n", error->message);
        exit (1);
    }

    dict = load_config ();
    loctype = config_get_location_type (dict);

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

    lat = locLat == 100 ? lat : locLat;
    lng = locLng == 100 ? lng : locLng;

    signal(SIGINT, handleExit);
    while (!quit) {
        loop();
        sleep (60);
    }

    iniparser_freedict (dict);

	return EXIT_SUCCESS;
}
