#include "time_layer.h"
#include "second_city_layer.h"
#include "c/appendix/config.h"
#include "c/appendix/memory_log.h"
#include "c/services/watch_services.h"

// MT = Margin Top
#define MT_TIME 14
#define MT_TIME_LECO 2


static TextLayer *s_container_layer;
static TextLayer *s_time_layer;

void time_layer_create(Layer* parent_layer, GRect frame) {
    s_container_layer = text_layer_create(frame);
    s_time_layer = text_layer_create(GRect(0, 0, frame.size.w, frame.size.h));

    text_layer_set_background_color(s_container_layer, GColorClear);

    // Main time formatting
    text_layer_set_background_color(s_time_layer, GColorClear);
    text_layer_set_text(s_time_layer, "00:00");
    text_layer_set_text_alignment(s_time_layer, GTextAlignmentLeft);

    layer_add_child(text_layer_get_layer(s_container_layer), text_layer_get_layer(s_time_layer));
    layer_add_child(parent_layer, text_layer_get_layer(s_container_layer));
    MEMORY_LOG_HEAP("after_time_layer_create");

}

// 12:30 -> 12:30
// 13:30 -> 1:30
// 00:30 -> 12:30

static void text_layer_move_frame(TextLayer *text_layer, GRect frame) {
    layer_set_frame(text_layer_get_layer(text_layer), frame);
}

void time_layer_tick() {
    // Get a tm structure
    struct tm tick_time = watch_services_localtime();

    // Format the time into a buffer
    static char s_buffer[8];
    config_format_time(s_buffer, 8, &tick_time);

    // Update the time
    text_layer_set_text(s_time_layer, s_buffer);

    // Reposition everything
    GRect bounds = layer_get_bounds(text_layer_get_layer(s_container_layer));
    text_layer_move_frame(s_time_layer, GRect(0, 0, bounds.size.w, bounds.size.h)); // Reset for size calculation
    GSize time_size = text_layer_get_content_size(s_time_layer);

    // Calculate some landmarks. When a second-city block is shown it occupies a
    // fixed strip on the left, so the main time is centered in the space that
    // remains to its right instead of across the whole layer.
    int content_w = time_size.w;
    int left_margin = g_config->second_city_enabled ? SECOND_CITY_BLOCK_W : 0;
    int text_h = time_size.h - MT_TIME; // Remove top margin, approximately
    int text_top = -MT_TIME + (bounds.size.h/2 - text_h/2);
    int text_left = left_margin + (bounds.size.w - left_margin) / 2 - content_w / 2;
    if (text_left < left_margin) {
        // The clock is too wide to clear the block; keep it flush against the
        // block rather than letting it slide back under the label.
        text_left = left_margin;
    }

    // emery: nudge LECO time text upward slightly to keep optical centering.
#ifdef PBL_PLATFORM_EMERY
    if (g_config->time_font == TIME_FONT_LECO) {
        text_top -= MT_TIME_LECO;
    }
#endif

    // Update layer position
    text_layer_move_frame(s_time_layer, GRect(text_left, text_top, content_w, time_size.h));
}

void time_layer_refresh() {
    text_layer_set_font(s_time_layer, config_time_font());
    text_layer_set_text_color(s_time_layer, PBL_IF_COLOR_ELSE(g_config->color_time, GColorWhite));
    time_layer_tick();  // Update main time text and layer positions
}

void time_layer_destroy() {
    MEMORY_LOG_HEAP("time_layer_destroy:before");
    text_layer_destroy(s_time_layer);
    text_layer_destroy(s_container_layer);
    MEMORY_LOG_HEAP("time_layer_destroy:after");
}
