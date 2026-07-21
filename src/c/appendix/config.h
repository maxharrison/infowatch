#pragma once

#include <pebble.h>

enum TimeFont {
    TIME_FONT_ROBOTO = 0,
    TIME_FONT_LECO = 1,
    TIME_FONT_BITHAM = 2,
};

// Max length (including null terminator) for the user-defined second city label.
#define SECOND_CITY_LABEL_MAX 8

typedef struct {
    bool time_lead_zero;
    bool prev_week;
    bool show_qt;
    bool show_bt;
    bool show_bt_disconnect;
    bool vibe;
    bool show_am_pm;
    int16_t time_font;
    GColor color_today;
    GColor color_saturday;
    GColor color_sunday;
    GColor color_time;
    bool day_night_shading;
    // Second city time (a user-defined timezone shown below the main clock).
    // New fields are appended to the end of the struct so older persisted
    // configs (written before these existed) stay backward compatible.
    bool second_city_enabled;
    int16_t second_city_offset;  // Minutes from UTC (e.g. Bangkok = +420)
    char second_city_label[SECOND_CITY_LABEL_MAX];
} Config;

extern Config *g_config;

void config_load();

void config_refresh();

void config_unload();

int config_localize_temp(int temp_f);

int config_format_time(char *s, size_t maxsize, const struct tm * tm_p);

int config_axis_hour(int hour);

int config_n_today();

GFont config_time_font();

bool config_highlight_sundays();

bool config_highlight_saturdays();
