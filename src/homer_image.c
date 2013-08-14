/*

This is basically the exact copy of the demos/feature_image
using an image of Homer Simpson because he's cool.

 */

#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#include "frame_animations.h"

#define MY_UUID {0xC3, 0x0D, 0xBA, 0xF1, 0x5F, 0x6F, 0x4F, 0x22, 0xBA, 0xAA, 0x8C, 0x2A, 0x96, 0x8C, 0xFC, 0x28}
PBL_APP_INFO(
            MY_UUID, 
            "Homer Simpson", 
            "Navid Nahidi", 
            1, 
            0,
            RESOURCE_ID_IMAGE_MENU_ICON,
            APP_INFO_WATCH_FACE);
    

Window window;
FrameAnimation gif_animation;
AppTimerHandle timer_handle;
BmpContainer homer_std_image_container;
TextLayer timeLayer; // The clock
TextLayer date_layer;


void handle_timer(AppContextRef ctx, AppTimerHandle handle, uint32_t cookie) {
    (void)ctx;
    (void)handle;
    
    if (cookie == 1) {

      // Animate frames with parameters: 20 frames per second, repeat, cookie=1
      frame_animation_linear(&gif_animation, ctx, handle, 1, 10, false);
      // If you have no idea what this function is, have a look at the README in the src folder!

  }
}

// Called once per second
void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {

  //static char timeText[] = "00:00 00"; // Needs to be static because it's used by the system later.

  PblTm currentTime;
  PblTm *tickTime = t->tick_time;
  get_time(&currentTime);

  static char timeText[] = "00:00 AM";
  static char amPmText[] = "PM AM";
  static char date_text[] = "00000000";

  char *timeFormat;
  /* if (clock_is_24h_style()) {
    timeFormat = " %R";
  } else {*/
    timeFormat = "%l:%M";
  // }

  string_format_time(timeText, sizeof(timeText), timeFormat, &currentTime);
  string_format_time(amPmText, sizeof(amPmText), "%p", tickTime);
  string_format_time(date_text, sizeof(date_text), "%D", tickTime);

  strcat(timeText, " ");
  strcat(timeText, amPmText);

  text_layer_set_text(&timeLayer, timeText);
  text_layer_set_text(&date_layer, date_text);


  if(currentTime.tm_sec % 10  == 0) {
    frame_animation_linear(&gif_animation, ctx, timer_handle, 1, 10, false);  
  }

}

void handle_init(AppContextRef ctx) {

  window_init(&window, "Homer");
  window_stack_push(&window, true /* Animated */);
  window_set_background_color(&window, GColorWhite);

  resource_init_current_app(&HOMER_RESOURCES);

  // Add Homer Image
  //bmp_init_container(RESOURCE_ID_IMAGE_HOMER, &homer_std_image_container);
  //layer_add_child(&window.layer, &homer_std_image_container.layer.layer);
  frame_animation_init(&gif_animation, &window.layer, GPoint(0,20), RESOURCE_ID_FRAME_1, 6, false, false);
  //timer_handle = app_timer_send_event(ctx, 10000, 1);
  //frame_animation_linear(&gif_animation, ctx, timer_handle, 1, 10, false);  

  // Init the text layer used to show the time
  text_layer_init(&timeLayer, GRect(0, 0, 144 /* width */, 28/* height */));
  text_layer_set_text_color(&timeLayer, GColorBlack);
  text_layer_set_background_color(&timeLayer, GColorClear);
  text_layer_set_text_alignment(&timeLayer, GTextAlignmentCenter);
  text_layer_set_font(&timeLayer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SIMPSONS_28)));
  layer_add_child(&window.layer, &timeLayer.layer);

  text_layer_init(&date_layer, GRect(90, 28 , 45, 168-28-10));
  text_layer_set_text_color(&date_layer, GColorBlack);
  text_layer_set_background_color(&date_layer, GColorClear);
  text_layer_set_text_alignment(&date_layer, GTextAlignmentCenter);
  text_layer_set_font(&date_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SIMPSONS_18)));
  layer_add_child(&window.layer, &date_layer.layer);

  // Ensures time is displayed immediately (will break if NULL tick event accessed).
  // (This is why it's a good idea to have a separate routine to do the update itself.)
  handle_minute_tick(ctx, NULL);


}


void handle_deinit(AppContextRef ctx) {
  (void)ctx;

  frame_animation_deinit(&gif_animation);
  // Note: Failure to de-init this here will result in instability and
  //       unable to allocate memory errors.
  bmp_deinit_container(&homer_std_image_container);
}



void pbl_main(void *params) {
  PebbleAppHandlers handlers = {

    // Handle app start
    .init_handler = &handle_init,
    .deinit_handler = &handle_deinit,
    .timer_handler = &handle_timer,
    // Handle time updates
    .tick_info = {
      .tick_handler = &handle_minute_tick,
      .tick_units = SECOND_UNIT
    }

  };
  app_event_loop(params, &handlers);
}
