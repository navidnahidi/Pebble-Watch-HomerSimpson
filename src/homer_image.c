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
            APP_INFO_STANDARD_APP);
    

Window window;

BmpContainer image_container;


void handle_init(AppContextRef ctx) {

  window_init(&window, "Demo");
  window_stack_push(&window, true /* Animated */);

  resource_init_current_app(&HOMER_IMAGE_RESOURCES);

  // Note: This needs to be "de-inited" in the application's
  //       deinit handler.
  bmp_init_container(RESOURCE_ID_IMAGE_HOMER, &image_container);

  layer_add_child(&window.layer, &image_container.layer.layer);

}


void handle_deinit(AppContextRef ctx) {

  // Note: Failure to de-init this here will result in instability and
  //       unable to allocate memory errors.
  bmp_deinit_container(&image_container);
}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
    .deinit_handler = &handle_deinit
  };
  app_event_loop(params, &handlers);
}
