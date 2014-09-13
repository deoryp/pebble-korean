/*

   Demonstrate how to display a two color, black and white bitmap
   image with no transparency.

   The original source image is from:

      <http://openclipart.org/detail/26728/aiga-litter-disposal-by-anonymous>

   The source image was converted from an SVG into a RGB bitmap using
   Inkscape. It has no transparency and uses only black and white as
   colors.

 */

#include "pebble.h"

static Window *window;

static Layer *layer_period;
static Layer *layer_hour;
static Layer *layer_minute;
static Layer *layer_second;

static GBitmap *image_period;
static GBitmap *image_hour;
static GBitmap *image_minute;
static GBitmap *image_second;

static int period = 0;
static int hour = 1;
static int minute = 1;
static int second = 1;

static int height = 40;

static void draw_image_at(Layer *me, GContext* ctx, GBitmap * image, int x, int y) {
  GRect bounds = image->bounds;
  graphics_draw_bitmap_in_rect(ctx, image, (GRect) { .origin = { x, y }, .size = bounds.size });
}


static void layer_second_update_callback(Layer *me, GContext* ctx) {
  // We make sure the dimensions of the GRect to draw into
  // are equal to the size of the bitmap--otherwise the image
  // will automatically tile. Which might be what *you* want.

  //GRect bounds_am = image_am->bounds;
  //GRect bounds_hours = image_hours->bounds;
  //GRect bounds_minutes = image_minutes->bounds;
  //GRect bounds_seconds = image_seconds->bounds;
  
  //graphics_draw_bitmap_in_rect(ctx, image_am, (GRect) { .origin = { -1 * 144/4, 0 }, .size = bounds_am.size });
  //graphics_draw_bitmap_in_rect(ctx, image_hours, (GRect) { .origin = { 0, height*1+1 }, .size = bounds_hours.size });
  //graphics_draw_bitmap_in_rect(ctx, image_minutes, (GRect) { .origin = { 0, height*2+1 }, .size = bounds_minutes.size });
  //graphics_draw_bitmap_in_rect(ctx, image_seconds, (GRect) { .origin = { 0, height*3+1 }, .size = bounds_seconds.size });
  
  if (second % 2 == 0) {
    draw_image_at(me, ctx, image_second, 0, height*3+1);
  } else {
    draw_image_at(me, ctx, image_minute, 0, height*3+1);
  }
      
}


// Called once per second
static void handle_second_tick(struct tm* t, TimeUnits units_changed) {  
  if(units_changed & MINUTE_UNIT) {
    
  }
  
  if(units_changed & HOUR_UNIT) {
    
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
  layer_second = layer_create(bounds);
  layer_set_update_proc(layer_second, layer_second_update_callback);
  layer_add_child(window_layer, layer_second);

  image_period = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_AM);
  image_hour = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HOURS);
  image_minute = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MINUTES);
  image_second = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SECONDS);
  
  // Ensures time is displayed immediately (will break if NULL tick event accessed).
  // (This is why it's a good idea to have a separate routine to do the update itself.)
  time_t now = time(NULL);
  struct tm *current_time = localtime(&now);
  handle_second_tick(current_time, SECOND_UNIT);
  tick_timer_service_subscribe(SECOND_UNIT, &handle_second_tick);
  
}

static void do_deinit(void) {
  gbitmap_destroy(image_period);
  gbitmap_destroy(image_hour);
  gbitmap_destroy(image_minute);  
  gbitmap_destroy(image_second);
  
  window_destroy(window);
  layer_destroy(layer_second);
}

int main(void) {
  
  do_init();    
  
  app_event_loop();
  
  do_deinit();
  
}

/*
#include "pebble.h"

static Window *window;

static BitmapLayer *image_layer;

static GBitmap *image;

int main(void) {
  window = window_create();
  window_stack_push(window, true);

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  // This needs to be deinited on app exit which is when the event loop ends
  image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_NO_LITTER);

  // The bitmap layer holds the image for display
  image_layer = bitmap_layer_create(bounds);
  
  graphics_draw_bitmap_in_rect
  
  
  bitmap_layer_set_bitmap(image_layer, image);
  bitmap_layer_set_alignment(image_layer, GAlignCenter);
  layer_add_child(window_layer, bitmap_layer_get_layer(image_layer));

  app_event_loop();

  gbitmap_destroy(image);

  bitmap_layer_destroy(image_layer);
  window_destroy(window);
}
*/
