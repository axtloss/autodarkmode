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
#include "sun.h"
#include<math.h>

#include <glib.h>
#include <stdlib.h>
#include <locale.h>
#include <glib/gi18n.h>
#include <geoclue.h>

static gint timeout = 30; /* seconds */
static GClueAccuracyLevel accuracy_level = GCLUE_ACCURACY_LEVEL_EXACT;
static gint time_threshold;
 GClueSimple *simple = NULL;
        GClueClient *client = NULL;
        GMainLoop *main_loop;

static gboolean
on_location_timeout (gpointer user_data)
{
    g_clear_object (&client);
    g_clear_object (&simple);
    g_main_loop_quit (main_loop);

    return FALSE;
}

static void
print_location (GClueSimple *simple)
{
    GClueLocation *location;

    GDateTime *current_time = g_date_time_new_now_local();

    location = gclue_simple_get_location (simple);
    g_print ("\nNew location:\n");
    g_print ("Latitude:    %f°\nLongitude:   %f°\nAccuracy:    %f meters\n",
             gclue_location_get_latitude (location),
             gclue_location_get_longitude (location),
             gclue_location_get_accuracy (location));

    int year, month, day;
    g_date_time_get_ymd (current_time, &year, &month, &day);
    g_print ("%d - %d - %d\n", year, month, day);

    int utc_offset = g_date_time_get_utc_offset (current_time)*2.7778*pow(10.0, -10.0);

    float localT = calculateSun(year, month, day, gclue_location_get_latitude (location), gclue_location_get_longitude (location), utc_offset, 0);
    double hours;
    float minutes = modf(localT,&hours)*60;
    g_print("Sunrise: %.0f:%.0f\n",hours,minutes);

    float sunsetT = calculateSun(year, month, day, gclue_location_get_latitude (location), gclue_location_get_longitude (location), utc_offset, 1);
    double sunHours;
    float sunMinutes = modf(sunsetT,&sunHours)*60;
    g_print("Sunset: %.0f:%.0f\n",sunHours,sunMinutes);

    g_free (current_time);
}

static void
on_client_active_notify (GClueClient *client,
                         GParamSpec *pspec,
                         gpointer    user_data)
{
    if (gclue_client_get_active (client))
        return;

    g_print ("Geolocation disabled. Quitting..\n");
    on_location_timeout (NULL);
}

static void
on_simple_ready (GObject      *source_object,
                 GAsyncResult *res,
                 gpointer      user_data)
{
    GError *error = NULL;

    simple = gclue_simple_new_with_thresholds_finish (res, &error);
    if (error != NULL) {
        g_critical ("Failed to connect to GeoClue2 service: %s", error->message);
        exit (-1);
    }
    client = gclue_simple_get_client (simple);
    if (client) {
        g_object_ref (client);
        g_print ("Client object: %s\n",
                g_dbus_proxy_get_object_path (G_DBUS_PROXY (client)));

        g_signal_connect (client,
                          "notify::active",
                          G_CALLBACK (on_client_active_notify),
                          NULL);
    }
    print_location (simple);

    g_signal_connect (simple,
                      "notify::location",
                      G_CALLBACK (print_location),
                      NULL);
}

gint
main (gint   argc,
      gchar *argv[])
{
	g_autoptr(GOptionContext) context = NULL;
	g_autoptr(GError) error = NULL;
	gboolean version = FALSE;
	GOptionEntry main_entries[] = {
		{ "version", 0, 0, G_OPTION_ARG_NONE, &version, "Show program version" },
		{ NULL }
	};

	context = g_option_context_new ("- my command line tool");
	g_option_context_add_main_entries (context, main_entries, NULL);

	if (!g_option_context_parse (context, &argc, &argv, &error))
	{
		g_printerr ("%s\n", error->message);
		return EXIT_FAILURE;
	}

	if (version)
	{
		g_printerr ("%s\n", PACKAGE_VERSION);
		return EXIT_SUCCESS;
	}

    g_timeout_add_seconds (timeout, on_location_timeout, NULL);

    gclue_simple_new_with_thresholds ("autodarkmode",
                                      accuracy_level,
                                      time_threshold,
                                      0,
                                      NULL,
                                      on_simple_ready,
                                      NULL);

    main_loop = g_main_loop_new (NULL, FALSE);
    g_main_loop_run (main_loop);

	return EXIT_SUCCESS;
}
