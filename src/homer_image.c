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
            "Homer Image", 
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

  static char timeText[] = "00:00"; // Needs to be static because it's used by the system later.

  PblTm currentTime;

  get_time(&currentTime);

  string_format_time(timeText, sizeof(timeText), "%T", &currentTime);
  //if (firstTime || currentTime.tm_sec == 0) {
      text_layer_set_text(&timeLayer, timeText);
  //}

  if(currentTime.tm_sec % 10  == 0) {
    frame_animation_linear(&gif_animation, ctx, timer_handle, 1, 10, false);  
  }

}

void handle_init(AppContextRef ctx) {

  window_init(&window, "Homer");
  window_stack_push(&window, true /* Animated */);
  window_set_background_color(&window, GColorWhite);

  resource_init_current_app(&HOMER_IMAGE_RESOURCES);

  // Add Homer Image
  //bmp_init_container(RESOURCE_ID_IMAGE_HOMER, &homer_std_image_container);
  //layer_add_child(&window.layer, &homer_std_image_container.layer.layer);
  frame_animation_init(&gif_animation, &window.layer, GPoint(0,20), RESOURCE_ID_FRAME_1, 6, false, false);
  //timer_handle = app_timer_send_event(ctx, 10000, 1);
  frame_animation_linear(&gif_animation, ctx, timer_handle, 1, 10, false);  

  // Init the text layer used to show the time
  // text_layer_init(&timeLayer, GRect(40, 27, 144-40 /* width */, 168-54 /* height */));
  text_layer_init(&timeLayer, GRect(25, 0, 144-40 /* width */, 168-54 /* height */));
  text_layer_set_text_color(&timeLayer, GColorBlack);
  text_layer_set_background_color(&timeLayer, GColorClear);
  text_layer_set_font(&timeLayer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SIMPSONS_32)));
  layer_add_child(&window.layer, &timeLayer.layer);

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
