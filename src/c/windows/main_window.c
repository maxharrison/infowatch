#include "main_window.h"
#include "c/layers/time_layer.h"
#include "c/layers/second_city_layer.h"
#include "c/layers/forecast_layer.h"
#include "c/layers/weather_status_layer.h"
#include "c/layers/calendar_layer.h"
#include "c/layers/calendar_status_layer.h"
#include "c/layers/loading_layer.h"
#include "c/appendix/app_message.h"
#include "c/appendix/persist.h"
#include "c/appendix/memory_log.h"

#define FORECAST_HEIGHT 51
#define WEATHER_STATUS_HEIGHT 14
#define TIME_HEIGHT 45
#define EMERY_WINDOW_PAD_X 2
#define EMERY_WINDOW_PAD_TOP 2
#define EMERY_WINDOW_PAD_BOTTOM 4
// emery: increase top calendar status row height to fit larger month and icon alignment.
// The calendar now shows a single week (one row), so CALENDAR_HEIGHT is a fixed
// row height rather than a three-week block. SECOND_CITY_HEIGHT is the small
// secondary-time band shown just below the main clock.
#ifdef PBL_PLATFORM_EMERY
#define CALENDAR_STATUS_HEIGHT 20
#define CALENDAR_HEIGHT 30
#define SECOND_CITY_HEIGHT 22
#else
#define CALENDAR_STATUS_HEIGHT 13
#define CALENDAR_HEIGHT 20
#define SECOND_CITY_HEIGHT 16
#endif

static Window *s_main_window;

#ifdef PBL_PLATFORM_EMERY
// emery: the calendar (one week) and second-city bands are fixed-height, so the
// remaining space is split between the time and forecast using their legacy
// weights to fill the taller screen.
static void compute_content_layout(int content_h, int *time_h, int *forecast_h) {
    const int weight_sum = TIME_HEIGHT + FORECAST_HEIGHT;

    *time_h = (content_h * TIME_HEIGHT) / weight_sum;
    *forecast_h = content_h - *time_h;
}
#endif

static void main_window_load(Window *window) {
    // Get information about the Window
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);
    int w = bounds.size.w;
    int h = bounds.size.h;
    window_set_background_color(window, GColorBlack);

#ifdef PBL_PLATFORM_EMERY
    // emery: pad to avoid content getting obscured by screen edge
    int content_x = EMERY_WINDOW_PAD_X;
    int content_y = EMERY_WINDOW_PAD_TOP;
    int content_w = w - EMERY_WINDOW_PAD_X * 2;
    int forecast_w = w - content_x;
    int content_h = h - EMERY_WINDOW_PAD_TOP - EMERY_WINDOW_PAD_BOTTOM
            - CALENDAR_STATUS_HEIGHT - CALENDAR_HEIGHT - SECOND_CITY_HEIGHT - WEATHER_STATUS_HEIGHT;
    int time_h;
    int forecast_h;
    compute_content_layout(content_h, &time_h, &forecast_h);

    int calendar_y = content_y + CALENDAR_STATUS_HEIGHT;
    int time_y = calendar_y + CALENDAR_HEIGHT;
    int second_city_y = time_y + time_h;
    int weather_status_y = second_city_y + SECOND_CITY_HEIGHT;
    int forecast_y = weather_status_y + WEATHER_STATUS_HEIGHT;

    forecast_layer_create(window_layer, GRect(content_x, forecast_y, forecast_w, forecast_h));
    weather_status_layer_create(window_layer, GRect(content_x, weather_status_y, content_w, WEATHER_STATUS_HEIGHT));
    second_city_layer_create(window_layer, GRect(content_x, second_city_y, content_w, SECOND_CITY_HEIGHT));
    time_layer_create(window_layer, GRect(content_x, time_y, content_w, time_h));
    calendar_layer_create(window_layer, GRect(content_x, calendar_y, content_w, CALENDAR_HEIGHT));
    calendar_status_layer_create(window_layer, GRect(content_x, content_y, content_w, CALENDAR_STATUS_HEIGHT + 1)); // +1 to stop text clipping
    loading_layer_create(window_layer, GRect(content_x, weather_status_y, content_w, h - EMERY_WINDOW_PAD_BOTTOM - weather_status_y));
#else
    int forecast_y = h - FORECAST_HEIGHT;
    int weather_status_y = forecast_y - WEATHER_STATUS_HEIGHT;
    int second_city_y = weather_status_y - SECOND_CITY_HEIGHT;
    int time_y = second_city_y - TIME_HEIGHT;

    forecast_layer_create(window_layer,
            GRect(0, forecast_y, w, FORECAST_HEIGHT));
    weather_status_layer_create(window_layer,
            GRect(0, weather_status_y, w, WEATHER_STATUS_HEIGHT));
    second_city_layer_create(window_layer,
            GRect(0, second_city_y, w, SECOND_CITY_HEIGHT));
    time_layer_create(window_layer,
            GRect(0, time_y, bounds.size.w, TIME_HEIGHT));
    calendar_layer_create(window_layer,
            GRect(0, CALENDAR_STATUS_HEIGHT, bounds.size.w, CALENDAR_HEIGHT));
    calendar_status_layer_create(window_layer,
            GRect(0, 0, bounds.size.w, CALENDAR_STATUS_HEIGHT + 1));  // +1 to stop text clipping
    loading_layer_create(window_layer,
            GRect(0, weather_status_y, w, FORECAST_HEIGHT + WEATHER_STATUS_HEIGHT));
#endif
    loading_layer_refresh();
    app_message_send_startup_state(loading_layer_has_valid_data());
    MEMORY_LOG_HEAP("after_window_load");
}

static void main_window_unload(Window *window) {
    MEMORY_LOG_HEAP("before_window_unload");
    time_layer_destroy();
    second_city_layer_destroy();
    weather_status_layer_destroy();
    forecast_layer_destroy();
    calendar_layer_destroy();
    calendar_status_layer_destroy();
    loading_layer_destroy();
    MEMORY_LOG_HEAP("after_window_unload");
}

static void minute_handler(struct tm *tick_time, TimeUnits units_changed) {
    time_layer_tick();
    second_city_layer_tick();
    /* tm_hour==0 missed day changes from emulator time jumps (same clock, new date). */
    if (units_changed & DAY_UNIT) {
        calendar_layer_refresh();
        calendar_status_layer_refresh();
    }
    status_icons_refresh();
    loading_layer_refresh();
}

/*----------------------------
-------- EXTERNAL ------------
----------------------------*/

void main_window_create() {
    // Create main Window element and assign to pointer
    s_main_window = window_create();

    // Set handlers to manage the elements inside the Window
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload
    });

    // Register with TickTimerService
    tick_timer_service_subscribe(MINUTE_UNIT | DAY_UNIT, minute_handler);

    // Show the window on the watch with animated=true
    window_stack_push(s_main_window, true);
    time_layer_refresh();
}

void main_window_refresh() {
    time_layer_refresh();
    second_city_layer_refresh();
    weather_status_layer_refresh();
    forecast_layer_refresh();
    calendar_layer_refresh();
    calendar_status_layer_refresh();
}

void main_window_destroy() {
    // Interface for destroying the main window (implicitly unloads contents)
    window_destroy(s_main_window);
}
