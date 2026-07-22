#include "battery_layer.h"
#include "c/appendix/persist.h"
#include "c/appendix/memory_log.h"
#include "c/services/watch_services.h"

#define BATTERY_NUB_W 2
#define BATTERY_NUB_H 6
#define BATTERY_STROKE 1
#define FILL_PADDING 1
#define ICON_SPACING 3
#define BATTERY_POWER_ICON_W 7
// Width/height of the [charge icon][gap][outline][nub] battery cluster. The
// percentage number is drawn in whatever space is left to its left.
#define BATTERY_GFX_W 29
#define BATTERY_GFX_H 10
#define PERCENT_GAP 3

// emery: use a larger number to match the taller status row.
#ifdef PBL_PLATFORM_EMERY
#define PERCENT_FONT_KEY FONT_KEY_GOTHIC_18
#define PERCENT_TEXT_Y (-4)
#else
#define PERCENT_FONT_KEY FONT_KEY_GOTHIC_14
#define PERCENT_TEXT_Y (-3)
#endif


static Layer *s_battery_layer;
static GBitmap *s_battery_power_bitmap;
static GColor s_battery_palette[2];
static bool s_battery_subscribed;

static void battery_state_handler(BatteryChargeState charge) {
    battery_layer_refresh();
}

#ifdef PBL_COLOR
static GColor get_battery_color(int level) {
    if (level >= 50)
        return GColorGreen;
    else if (level >= 30)
        return GColorYellow;
    else
        return GColorRed;
}
#endif

static void ensure_battery_power_bitmap_loaded(void) {
    if (!s_battery_power_bitmap) {
        s_battery_power_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_CHARGING);
        s_battery_palette[0] = GColorWhite;
        s_battery_palette[1] = GColorClear;
        gbitmap_set_palette(s_battery_power_bitmap, s_battery_palette, false);
    }
}

static void maybe_unload_battery_power_bitmap(bool show_power_icon) {
    if (!show_power_icon && s_battery_power_bitmap) {
        gbitmap_destroy(s_battery_power_bitmap);
        s_battery_power_bitmap = NULL;
    }
}

static void draw_power_icon(GContext *ctx, int gfx_x, int gfx_y, int gfx_h, GBitmap *icon_bitmap) {
    GRect icon_bounds = gbitmap_get_bounds(icon_bitmap);
    int icon_x = gfx_x;
    int icon_y = gfx_y + (gfx_h - icon_bounds.size.h) / 2;

    graphics_context_set_compositing_mode(ctx, GCompOpSet);
    graphics_draw_bitmap_in_rect(
        ctx,
        icon_bitmap,
        GRect(icon_x, icon_y, icon_bounds.size.w, icon_bounds.size.h));
    graphics_context_set_compositing_mode(ctx, GCompOpAssign);
}

static void battery_update_proc(Layer *layer, GContext *ctx) {
    GRect bounds = layer_get_bounds(layer);
    int w = bounds.size.w;
    int h = bounds.size.h;
    BatteryChargeState battery_state = watch_services_battery_state();
    int battery_level = battery_state.charge_percent;
    bool show_power_icon = battery_state.is_charging || battery_state.is_plugged;

    maybe_unload_battery_power_bitmap(show_power_icon);

    // The battery graphic is right-aligned and vertically centered in the layer.
    // The percentage number is drawn to its left.
    int gfx_x = w - BATTERY_GFX_W;
    int gfx_y = (h - BATTERY_GFX_H) / 2;

    int battery_x = gfx_x + BATTERY_POWER_ICON_W + ICON_SPACING;
    int battery_total_w = w - battery_x;
    int battery_w = battery_total_w - BATTERY_NUB_W;

    // Fill the battery level
    GRect color_bounds = GRect(
        battery_x + BATTERY_STROKE + FILL_PADDING, gfx_y + BATTERY_STROKE + FILL_PADDING,
        battery_w - (BATTERY_STROKE + FILL_PADDING) * 2, BATTERY_GFX_H - (BATTERY_STROKE + FILL_PADDING) * 2);
    GRect color_area = GRect(
        color_bounds.origin.x, color_bounds.origin.y,
        color_bounds.size.w * (battery_level + 10) / 110, color_bounds.size.h);
#ifdef PBL_COLOR
    graphics_context_set_fill_color(ctx, get_battery_color(battery_level));
#else
    graphics_context_set_fill_color(ctx, GColorWhite);
#endif
    graphics_fill_rect(ctx, color_area, 0, GCornerNone);

    if (show_power_icon) {
        ensure_battery_power_bitmap_loaded();
        draw_power_icon(ctx, gfx_x, gfx_y, BATTERY_GFX_H, s_battery_power_bitmap);
    }

    // Draw the white battery outline
    graphics_context_set_stroke_color(ctx, GColorWhite);
    graphics_context_set_stroke_width(ctx, BATTERY_STROKE);
    graphics_draw_rect(ctx, GRect(battery_x, gfx_y, battery_w, BATTERY_GFX_H));

    // Draw the battery nub on the right
    graphics_draw_rect(
        ctx,
        GRect(battery_x + battery_w - 1, gfx_y + BATTERY_GFX_H / 2 - BATTERY_NUB_H / 2, BATTERY_NUB_W + 1, BATTERY_NUB_H));

    // Draw the battery percentage to the left of the graphic. Anchor it to the
    // battery outline so it sits snug against the symbol; only when the charging
    // icon is shown do we back off to leave that icon its slot.
    int percent_right = (show_power_icon ? gfx_x : battery_x) - PERCENT_GAP;
    char percent_buf[5];
    snprintf(percent_buf, sizeof(percent_buf), "%d", battery_level);
    graphics_context_set_text_color(ctx, GColorWhite);
    graphics_draw_text(
        ctx,
        percent_buf,
        fonts_get_system_font(PERCENT_FONT_KEY),
        GRect(0, PERCENT_TEXT_Y, percent_right, h),
        GTextOverflowModeFill,
        GTextAlignmentRight,
        NULL);
}

void battery_layer_create(Layer* parent_layer, GRect frame) {
    MemoryHeapProbe probe = MEMORY_HEAP_PROBE_START("battery_layer_create");

    s_battery_layer = layer_create(frame);
    MEMORY_HEAP_PROBE_SAMPLE("after_layer_create", &probe);

    layer_set_update_proc(s_battery_layer, battery_update_proc);
    if (!watch_services_battery_is_fixture()) {
        battery_state_service_subscribe(battery_state_handler);
        s_battery_subscribed = true;
    } else {
        s_battery_subscribed = false;
    }
    MEMORY_HEAP_PROBE_SAMPLE("after_battery_subscribe", &probe);
    layer_add_child(parent_layer, s_battery_layer);
    MEMORY_HEAP_PROBE_SAMPLE("after_layer_add_child", &probe);
    MEMORY_LOG_HEAP("after_battery_layer_create");
    MEMORY_HEAP_PROBE_LOG_MIN(&probe);
}

void battery_layer_refresh() {
    layer_mark_dirty(s_battery_layer);
}

void battery_layer_destroy() {
    MEMORY_LOG_HEAP("battery_layer_destroy:before");
    if (s_battery_subscribed) {
        battery_state_service_unsubscribe();
        s_battery_subscribed = false;
    }
    if (s_battery_power_bitmap) {
        gbitmap_destroy(s_battery_power_bitmap);
        s_battery_power_bitmap = NULL;
    }
    layer_destroy(s_battery_layer);
    MEMORY_LOG_HEAP("battery_layer_destroy:after");
}
