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

        int year = 2024, month = 5, day = 28;
        double latitude = 51.01;
        double longitude = 7.563;

        float localT = calculateSunrise(year, month, day, latitude, longitude, 2);
        double hours;
        float minutes = modf(localT,&hours)*60;
        g_print("Sunrise: %.0f:%.0f\n",hours,minutes);

        float sunsetT = calculateSunset(year, month, day, latitude, longitude, 2);
        double sunHours;
        float sunMinutes = modf(sunsetT,&sunHours)*60;
        g_print("Sunset: %.0f:%.0f\n",sunHours,sunMinutes);

        return 0;

	return EXIT_SUCCESS;
}
