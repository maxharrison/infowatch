#include "config.h"
#include "persist.h"
#include "math.h"
#include "memory_log.h"
#include "c/services/watch_services.h"

Config *g_config;

// Returns defaults as a function (not a static const) because GColor values like
// GColorBlack expand to "compound literals" — C's syntax for inline struct values.
// The C standard doesn't allow these in static variable initializers, so we use a
// function instead. See: https://gcc.gnu.org/onlinedocs/gcc/Compound-Literals.html
static Config config_defaults(void) {
    return (Config) {
        .time_lead_zero = false,
        .prev_week = true,
        .show_qt = true,
        .show_bt = true,
        .show_bt_disconnect = true,
        .vibe = false,
        .show_am_pm = false,
        .time_font = TIME_FONT_ROBOTO,
        .color_today = GColorBlack,
        .color_saturday = GColorOrange,
        .color_sunday = GColorRed,
        .color_time = GColorWhite,
        .day_night_shading = true,
        .second_city_enabled = false,
        .second_city_offset = 0,
        .second_city_label = ""
    };
}

static void config_read_or_default(Config *config) {
    *config = config_defaults();
    persist_get_config(config);
}

void config_load() {
    g_config = (Config*) malloc(sizeof(Config));
    config_read_or_default(g_config);
    MEMORY_LOG_HEAP("after_config_load");
}

void config_refresh() {
    free(g_config);  // Clear out the old config
    g_config = (Config*) malloc(sizeof(Config));
    config_read_or_default(g_config);  // Then reload
    MEMORY_LOG_HEAP("after_config_refresh");
}

void config_unload() {
    free(g_config);
}

int config_localize_temp(int temp_f) {
    // Always display temperatures in Celsius.
    return f_to_c(temp_f);
}

int config_format_time(char *s, size_t maxsize, const struct tm * tm_p) {
    // Always use a 24 hour clock.
    int res = strftime(s, maxsize, "%H:%M", tm_p);
    if (!g_config->time_lead_zero) {
        // Remove leading zero if configured as such
        if (s[0] == '0')
            memmove(s, s+1, strlen(s));
    }
    return res;
}

int config_axis_hour(int hour) {
    // Always use a 24 hour axis.
    return hour % 24;
}

int config_n_today() {
    // Returns the index of the calendar box that holds today's date.
    // The calendar now shows only the current week (a single row), so today's
    // index is simply its position within that week. Week always starts on
    // Monday. The prev_week config no longer affects the layout.

    struct tm tm_today = watch_services_localtime();
    int wday = (tm_today.tm_wday + 6) % 7;
    return wday;
}

GFont config_time_font() {
    const char *font_keys[] = {
        [TIME_FONT_ROBOTO] = FONT_KEY_ROBOTO_BOLD_SUBSET_49,
#ifdef PBL_PLATFORM_EMERY
        // emery: use larger LECO font size
        [TIME_FONT_LECO] = FONT_KEY_LECO_60_NUMBERS_AM_PM,
#else
        [TIME_FONT_LECO] = FONT_KEY_LECO_42_NUMBERS,
#endif
        [TIME_FONT_BITHAM] = FONT_KEY_BITHAM_42_MEDIUM_NUMBERS
    };
    int16_t font_index = g_config->time_font;
    const int16_t font_count = (int16_t)(sizeof(font_keys) / sizeof(font_keys[0]));
    if (font_index < 0 || font_index >= font_count)
        font_index = TIME_FONT_ROBOTO;
    return fonts_get_system_font(font_keys[font_index]);
}

bool config_highlight_sundays() {
    return !gcolor_equal(g_config->color_sunday, GColorWhite);
}

bool config_highlight_saturdays() {
    return !gcolor_equal(g_config->color_saturday, GColorWhite);
}
