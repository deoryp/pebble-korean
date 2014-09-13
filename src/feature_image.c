#include "pebble.h"

static Window *window;

static Layer *layer_period;
static Layer *layer_hour;
static Layer *layer_minute;
static Layer *layer_second;

static GBitmap *image_period[2];
static GBitmap *image_kr[13];
static GBitmap *image_sino[11];
static GBitmap *image_counter[3];

static int period = 0;
static int hour = 1;
static int minute = 1;
static int second = 1;

static int height = 40;

#define PERIOD_HEIGHT  (1)
#define HOURS_HEIGHT   (40+1)
#define MINUTES_HEIGHT (40*2+1)
#define SECONDS_HEIGHT (40*3+1)
#define COL_0 (0)
#define COL_1 (36)
#define COL_2 (72)
#define COL_3 (108)

static void draw_image_at(Layer *me, GContext* ctx, GBitmap * image, int x, int y) {
  GRect bounds = image->bounds;
  graphics_draw_bitmap_in_rect(ctx, image, (GRect) { .origin = { x, y }, .size = bounds.size });
}


static void layer_period_update_callback(Layer *me, GContext* ctx) {
  draw_image_at(me, ctx, image_period[period], COL_1, PERIOD_HEIGHT);
}

static void layer_hour_update_callback(Layer *me, GContext* ctx) {
  draw_image_at(me, ctx, image_kr[hour], COL_0, HOURS_HEIGHT);
  draw_image_at(me, ctx, image_counter[0], COL_3, HOURS_HEIGHT);
  
}

static void layer_minute_update_callback(Layer *me, GContext* ctx) {
  if (minute > 9) {
    int tens = (minute - (minute % 10)) / 10;
    if (tens > 1) {
      draw_image_at(me, ctx, image_sino[tens], COL_0, MINUTES_HEIGHT);
    }
    draw_image_at(me, ctx, image_sino[10], COL_1, MINUTES_HEIGHT);
  }
  
  int ones = minute % 10;
  draw_image_at(me, ctx, image_sino[ones], COL_2, MINUTES_HEIGHT);
  
  draw_image_at(me, ctx, image_counter[1], COL_3, MINUTES_HEIGHT);
  
}

static void layer_second_update_callback(Layer *me, GContext* ctx) {
  if (second > 9) {
    int tens = (second - (second % 10)) / 10;
    if (tens > 1) {
      draw_image_at(me, ctx, image_sino[tens], COL_0, SECONDS_HEIGHT);
    }
    draw_image_at(me, ctx, image_sino[10], COL_1, SECONDS_HEIGHT);
  }
  
  int ones = second % 10;
  draw_image_at(me, ctx, image_sino[ones], COL_2, SECONDS_HEIGHT);
  
  draw_image_at(me, ctx, image_counter[2], COL_3, SECONDS_HEIGHT);
}

static void handle_second_tick(struct tm* t, TimeUnits units_changed) {
  
  // 0 = Midnight
  // 11 = Noon
  
  if(units_changed & HOUR_UNIT) {
    int new_period;
    if (t->tm_hour <= 11) { // AM
      new_period = 0;
      hour = t->tm_hour;
    } else { // PM
      new_period = 1;
      hour = t->tm_hour - 12;
    }
    // Adjust for Noon and Midnight
    if (hour == 0) {
      hour = 12;
    }
    layer_mark_dirty(layer_hour);
    
    // If the Period changed, mark it as changed.
    //
    if (period != new_period) {
      period = new_period;
      layer_mark_dirty(layer_period);
    }
  }
  
  if(units_changed & MINUTE_UNIT) {
    minute = t->tm_sec;
    layer_mark_dirty(layer_minute);
  }
  
  if(units_changed & SECOND_UNIT) {
    second = t->tm_sec;
    layer_mark_dirty(layer_second);
  } 
}

// Handle the start-up of the app
static void do_init(void) {
  window = window_create();
  window_stack_push(window, true /* Animated */);

  // Init the layer for display the image
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);
  
  layer_period = layer_create(bounds);
  layer_set_update_proc(layer_period, layer_period_update_callback);
  layer_add_child(window_layer, layer_period);
  
  layer_hour = layer_create(bounds);
  layer_set_update_proc(layer_hour, layer_hour_update_callback);
  layer_add_child(window_layer, layer_hour);
  
  layer_minute = layer_create(bounds);
  layer_set_update_proc(layer_minute, layer_minute_update_callback);
  layer_add_child(window_layer, layer_minute);
  
  layer_second = layer_create(bounds);
  layer_set_update_proc(layer_second, layer_second_update_callback);
  layer_add_child(window_layer, layer_second);

  image_period[0] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_AM);
  image_period[1] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PM);

  image_counter[0] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HOUR);
  image_counter[1] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MINUTE);
  image_counter[2] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SECOND);

  image_kr[0] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SECOND);

  // no image_kr[0]
  image_kr[1] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_KR_ONE);
  image_kr[2] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_KR_TWO);
  image_kr[3] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_KR_THREE);
  image_kr[4] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_KR_FOUR);
  image_kr[5] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_KR_FIVE);
  image_kr[6] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_KR_SIX);
  image_kr[7] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_KR_SEVEN);
  image_kr[8] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_KR_EIGHT);
  image_kr[9] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_KR_NINE);
  image_kr[10] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_KR_TEN);
  image_kr[11] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_KR_ELEVEN);
  image_kr[12] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_KR_TWELVE);
  
  image_sino[0] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SINO_ZERO);
  image_sino[1] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SINO_ONE);
  image_sino[2] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SINO_TWO);
  image_sino[3] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SINO_THREE);
  image_sino[4] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SINO_FOUR);
  image_sino[5] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SINO_FIVE);
  image_sino[6] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SINO_SIX);
  image_sino[7] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SINO_SEVEN);
  image_sino[8] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SINO_EIGHT);
  image_sino[9] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SINO_NINE);
  image_sino[10] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SINO_TEN);
  
  time_t now = time(NULL);
  struct tm *current_time = localtime(&now);
  handle_second_tick(current_time, HOUR_UNIT | MINUTE_UNIT | SECOND_UNIT);
  tick_timer_service_subscribe(HOUR_UNIT | MINUTE_UNIT | SECOND_UNIT, &handle_second_tick);
  
}

static void do_deinit(void) {  
  gbitmap_destroy(image_period[0]);
  gbitmap_destroy(image_period[1]);
  
  gbitmap_destroy(image_counter[0]);
  gbitmap_destroy(image_counter[1]);
  gbitmap_destroy(image_counter[2]);
  
  for (int i = 1; i < 13; i++) {
    gbitmap_destroy(image_kr[i]);
  }
  
  for (int i = 0; i < 12; i++) {
    gbitmap_destroy(image_sino[i]);
  }
  
  window_destroy(window);
  layer_destroy(layer_period);
  layer_destroy(layer_hour);
  layer_destroy(layer_minute);
  layer_destroy(layer_second);
}

int main(void) {
  
  do_init();    
  
  app_event_loop();
  
  do_deinit();
  
}
