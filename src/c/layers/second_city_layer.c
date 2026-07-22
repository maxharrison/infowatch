#include "second_city_layer.h"
#include "c/appendix/config.h"
#include "c/appendix/memory_log.h"

// MINUTES_PER_DAY is provided by pebble.h.

// emery: use a larger secondary font to match the taller strip on Emery.
#ifdef PBL_PLATFORM_EMERY
#define SECOND_CITY_FONT_KEY FONT_KEY_GOTHIC_18
#else
#define SECOND_CITY_FONT_KEY FONT_KEY_GOTHIC_14
#endif

static TextLayer *s_second_city_layer;
// The strip allotted to this block (set at creation). The rendered text is
// vertically centered within it so it lines up beside the main clock.
static GRect s_frame;

// Format the second city's wall-clock time by applying a fixed UTC offset.
// Uses the raw UTC epoch (time()) rather than the watch's local time so the
// configured offset is absolute and independent of the phone's timezone.
static void format_second_city_time(char *buffer, size_t size) {
    time_t utc = time(NULL);
    time_t shifted = utc + (time_t)g_config->second_city_offset * 60;

    // Reduce to a minute-of-day, guarding against negative offsets wrapping past
    // midnight. Integer math only (no gmtime dependency).
    long total_minutes = (long)(shifted / 60);
    int minute_of_day = (int)(total_minutes % MINUTES_PER_DAY);
    if (minute_of_day < 0) {
        minute_of_day += MINUTES_PER_DAY;
    }

    struct tm t = (struct tm){0};
    t.tm_hour = minute_of_day / 60;
    t.tm_min = minute_of_day % 60;

    // Reuse the main clock formatter so leading-zero preference stays consistent.
    config_format_time(buffer, size, &t);
}

void second_city_layer_tick() {
    bool enabled = g_config->second_city_enabled;
    layer_set_hidden(text_layer_get_layer(s_second_city_layer), !enabled);
    if (!enabled) {
        return;
    }

    // Two stacked lines: the city label on top and the time in small text below
    // it ("BKK" / "07:34"). With no label configured, only the time is shown.
    static char s_time_buffer[8];
    static char s_buffer[SECOND_CITY_LABEL_MAX + 10];
    format_second_city_time(s_time_buffer, sizeof(s_time_buffer));

    if (g_config->second_city_label[0] != '\0') {
        snprintf(s_buffer, sizeof(s_buffer), "%s\n%s",
                 g_config->second_city_label, s_time_buffer);
    } else {
        snprintf(s_buffer, sizeof(s_buffer), "%s", s_time_buffer);
    }
    text_layer_set_text(s_second_city_layer, s_buffer);

    // Vertically center the block within its strip so it aligns with the clock.
    Layer *layer = text_layer_get_layer(s_second_city_layer);
    layer_set_frame(layer, GRect(s_frame.origin.x, s_frame.origin.y, s_frame.size.w, s_frame.size.h));
    GSize content = text_layer_get_content_size(s_second_city_layer);
    int top = s_frame.origin.y + (s_frame.size.h - content.h) / 2;
    if (top < s_frame.origin.y) {
        top = s_frame.origin.y;
    }
    layer_set_frame(layer, GRect(s_frame.origin.x, top, s_frame.size.w, content.h));
}

void second_city_layer_create(Layer* parent_layer, GRect frame) {
    s_frame = frame;
    s_second_city_layer = text_layer_create(frame);
    text_layer_set_background_color(s_second_city_layer, GColorClear);
    text_layer_set_text_color(s_second_city_layer, GColorWhite);
    text_layer_set_text_alignment(s_second_city_layer, GTextAlignmentLeft);
    text_layer_set_font(s_second_city_layer, fonts_get_system_font(SECOND_CITY_FONT_KEY));
    second_city_layer_tick();
    layer_add_child(parent_layer, text_layer_get_layer(s_second_city_layer));
    MEMORY_LOG_HEAP("after_second_city_layer_create");
}

void second_city_layer_refresh() {
    second_city_layer_tick();
}

void second_city_layer_destroy() {
    MEMORY_LOG_HEAP("second_city_layer_destroy:before");
    text_layer_destroy(s_second_city_layer);
    MEMORY_LOG_HEAP("second_city_layer_destroy:after");
}
