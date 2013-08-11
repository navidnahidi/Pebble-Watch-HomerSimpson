/*

This is basically the exact copy of the demos/feature_image
using an image of Homer Simpson because he's cool.

 */

#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

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

BmpContainer homer_std_image_container;

TextLayer timeLayer; // The clock

// Called once per second
void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {

  static char timeText[] = "00:00"; // Needs to be static because it's used by the system later.

  PblTm currentTime;

  get_time(&currentTime);

  string_format_time(timeText, sizeof(timeText), "%T", &currentTime);

  text_layer_set_text(&timeLayer, timeText);

}

void handle_init(AppContextRef ctx) {

  window_init(&window, "Homer");
  window_stack_push(&window, true /* Animated */);

  resource_init_current_app(&HOMER_IMAGE_RESOURCES);

  // Add Homer Image
  bmp_init_container(RESOURCE_ID_IMAGE_HOMER, &homer_std_image_container);
  layer_add_child(&window.layer, &homer_std_image_container.layer.layer);

  // Init the text layer used to show the time
  text_layer_init(&timeLayer, GRect(40, 27, 144-40 /* width */, 168-54 /* height */));
  text_layer_set_text_color(&timeLayer, GColorBlack);
  text_layer_set_background_color(&timeLayer, GColorClear);
  text_layer_set_font(&timeLayer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SIMPSONS_18)));

  // Ensures time is displayed immediately (will break if NULL tick event accessed).
  // (This is why it's a good idea to have a separate routine to do the update itself.)
  handle_minute_tick(ctx, NULL);

  layer_add_child(&window.layer, &timeLayer.layer);

}


void handle_deinit(AppContextRef ctx) {

  // Note: Failure to de-init this here will result in instability and
  //       unable to allocate memory errors.
  bmp_deinit_container(&homer_std_image_container);
}



void pbl_main(void *params) {
  PebbleAppHandlers handlers = {

    // Handle app start
    .init_handler = &handle_init,
    .deinit_handler = &handle_deinit,
    // Handle time updates
    .tick_info = {
      .tick_handler = &handle_minute_tick,
      .tick_units = MINUTE_UNIT
    }

  };
  app_event_loop(params, &handlers);
}
