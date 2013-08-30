/* ======================================================================
This code is for the Homer Simpson App who blinks every 10 seconds.  

This app uses the clasio demo code and the Nyan Watch for animations

Nyan Watch 
@ http://www.mypebblefaces.com/apps/1790/1529/

@todo Vertically adjust font for date
@todo Create custom animations script to remove dependency on Nyan Watch

** ======================================================================
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
    
// Our Global Objects
Window window; // Window object
FrameAnimation gif_animation; // FrameAnimation required by Nyan Watch
AppTimerHandle timer_handle; // Timer Handle since we can't use usleep and required by Nyan Watch
TextLayer timeLayer; // The clock 
TextLayer date_layer; // The date layer


/*
  Our timer handle required by Nyan Watch
*/
void handle_timer(AppContextRef ctx, AppTimerHandle handle, uint32_t cookie) {
    (void)ctx;
    (void)handle;
    
    if (cookie == 1) {
      /*
      Arguments to frame_animation_linear
      @FrameAnimation (Object)
      @AppContextRef (Object),
      @timer_handle (object),
      @cookie (int),
      @fps (int),
      @continuous (bool)

      Everything is standard here, but we don't want continuous animation so we set this false (last argument)
      and we set the frame rate to be 10 frames per second.  We have only 6.
      */
      frame_animation_linear(&gif_animation, ctx, handle, 1, 10, false);
    }
}

/*
  Called once per second (even though it says minute).  Here we do a hack where we see if the current second is divisible by 10 to
  fire off our animation.  We animate every 10 seconds.
*/  
void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {

  PblTm currentTime;
  PblTm *tickTime = t->tick_time;
  // Get the current time
  get_time(&currentTime);

  char *timeFormat;

  // Display time format, am/pm and date text
  static char timeText[] = "00:00 AM";
  static char amPmText[] = "PM AM";
  static char date_text[] = "00000000";

  // Below here we need to enable 24 hour style
   if (clock_is_24h_style()) {
    timeFormat = "%R   ";
   } else {
    timeFormat = "%l:%M";
   }

  string_format_time(timeText, sizeof(timeText), timeFormat, &currentTime);
  string_format_time(amPmText, sizeof(amPmText), "%p", tickTime);
  string_format_time(date_text, sizeof(date_text), "%D", tickTime);

  // Concatenate the AM/PM text to the current time
  if (!clock_is_24h_style()) {
    strcat(timeText, " ");
    strcat(timeText, amPmText);
  }

  // Add time and date to our two layers
  text_layer_set_text(&timeLayer, timeText);
  text_layer_set_text(&date_layer, date_text);

  // If we the current times seconds parameter is divisible by 10 fire off our animation
  if(currentTime.tm_sec % 10  == 0) {
    frame_animation_linear(&gif_animation, ctx, timer_handle, 1, 10, false);  
  }

}

/*
  Our main initializer
*/
void handle_init(AppContextRef ctx) {
  window_init(&window, "Homer");
  window_stack_push(&window, true /* Animated */);
  window_set_background_color(&window, GColorWhite);

  // Add our images, fonts and the rest of our resources to our app
  resource_init_current_app(&HOMER_RESOURCES);

  /*
  Arguments are 

  FrameAnimation (Object), 
  Layer (Object), 
  GPoint (Object), 
  first_resource_id (int), 
  num_frames (int), 
  is_transparent (bool), 
  starts_hidden (bool)

  We set its position slightly lower to leave room for our clock (GPoint ...)
  We use the first image to be our first and last state
  6 total frames
  Image is not transparent (2nd to last false) and we want to display the image right away (last false)
  */
  frame_animation_init(&gif_animation, &window.layer, GPoint(0,20), RESOURCE_ID_FRAME_1, 6, false, false);
 

  // Set the time layer, use the Simpsons font size 28 and add to Window layer
  text_layer_init(&timeLayer, GRect(0, 0, 144 /* width */, 28/* height */));
  text_layer_set_text_color(&timeLayer, GColorBlack);
  text_layer_set_background_color(&timeLayer, GColorClear);
  text_layer_set_text_alignment(&timeLayer, GTextAlignmentCenter);
  text_layer_set_font(&timeLayer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SIMPSONS_28)));
  layer_add_child(&window.layer, &timeLayer.layer);

  // Set date text to be short width and to use custom Simpsons Font size 18 and add to our Window layer
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


/*
De-initialize (aka C's version of a destructor)
*/
void handle_deinit(AppContextRef ctx) {
  (void)ctx;
  frame_animation_deinit(&gif_animation);
}

/*
 Our main method which sets a struct to all of our appropriate handlers
*/
void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    // Handle app start
    .init_handler = &handle_init,
    .deinit_handler = &handle_deinit,
    // App timer required by Nyan Watch
    .timer_handler = &handle_timer,
    // Handle time updates
    .tick_info = {
      .tick_handler = &handle_minute_tick,
      // Call our tick handler every second by argument below
      .tick_units = SECOND_UNIT
    }

  };
  app_event_loop(params, &handlers);
}
