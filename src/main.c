#include <pebble.h>
  
#define KEY_TEMPERATURE 0
#define KEY_CONDITIONS 1
#define KEY_LOCATION 2
  
static Window *s_main_window;
static TextLayer *s_time_layer;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;
static TextLayer *s_battery_layer;
static TextLayer *s_date_layer;
static TextLayer *s_weather_layer;
static TextLayer *s_location_layer;

static void handle_battery(BatteryChargeState charge_state) {
  static char battery_text[] = "---% Remaining";

  if (charge_state.is_charging) {
    snprintf(battery_text, sizeof(battery_text), "Charging");
  } else {
    snprintf(battery_text, sizeof(battery_text), "%d%% Remaining", charge_state.charge_percent);
  }
  text_layer_set_text(s_battery_layer, battery_text);
}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00 AM";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
  //   Use 24 hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
  //  Use 12 hour format
    strftime(buffer, sizeof("00:00 AM"), "%I:%M %p", tick_time);
  }
  
  // Copy date into buffer from tm structure
  static char date_buffer[16];
  strftime(date_buffer, sizeof(date_buffer), "%a %b %d %Y", tick_time);


  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
  
  // Show the date
  text_layer_set_text(s_date_layer, date_buffer);
   
}

static void main_window_load(Window *window) {
  // Create GBitmap, then set to created BitmapLayer
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CSTONE_LOGO);
  s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
  
  static GFont s_time_font;
  static GFont s_battery_font;
  static GFont s_date_font;
  static GFont s_weather_font;
  static GFont s_location_font;
  
  // Create TextLayers
  s_time_layer = text_layer_create(GRect(0, 52, 144, 38));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text(s_time_layer, "00:00 AM");
  s_date_layer = text_layer_create(GRect(0, 90, 144, 20));
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  text_layer_set_text(s_date_layer, "Mon Jan 31 2015");
  s_battery_layer = text_layer_create(GRect(72, 125, 72, 40));
  text_layer_set_background_color(s_battery_layer, GColorClear);
  text_layer_set_text_color(s_battery_layer, GColorBlack);
  text_layer_set_text(s_battery_layer, "---% Remaining");
  s_weather_layer = text_layer_create(GRect(0, 125, 72, 40));
  text_layer_set_background_color(s_weather_layer, GColorClear);
  text_layer_set_text_alignment(s_weather_layer, GTextAlignmentCenter);
  text_layer_set_text(s_weather_layer, "-- F\nLoading...");
  s_location_layer = text_layer_create(GRect(0, 108, 144, 20));
  text_layer_set_background_color(s_location_layer, GColorClear);
  text_layer_set_text_alignment(s_location_layer, GTextAlignmentCenter);
  text_layer_set_text(s_location_layer, "Loading...");
     
  // Create custom font for TextLayers
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_GARAMOND_36));
  s_battery_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_GARAMOND_16));
  s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_GARAMOND_16));
  s_weather_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_GARAMOND_16));
  s_location_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_GARAMOND_16));
    
  // Apply to TextLayers
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  text_layer_set_font(s_battery_layer, s_battery_font);
  text_layer_set_text_alignment(s_battery_layer, GTextAlignmentCenter);
  text_layer_set_font(s_date_layer, s_date_font);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  text_layer_set_font(s_weather_layer, s_weather_font);
  text_layer_set_text_alignment(s_weather_layer, GTextAlignmentCenter);
  text_layer_set_font(s_location_layer, s_location_font);
  text_layer_set_text_alignment(s_location_layer, GTextAlignmentCenter);
  
  // Add layers as child layers to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_battery_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));  
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weather_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_location_layer));
  
  
}

static void main_window_unload(Window *window) {
  // Destroy TextLayers
  text_layer_destroy(s_time_layer);
  
  // Destroy GBitmap
  gbitmap_destroy(s_background_bitmap);

  // Destroy BitmapLayer
  bitmap_layer_destroy(s_background_layer);
  
  // Destroy Battery layer
  battery_state_service_unsubscribe();
  text_layer_destroy(s_battery_layer);
  
  // Destry Date Layer
  text_layer_destroy(s_date_layer);
  
  // Destroy Weather Layer
  text_layer_destroy(s_weather_layer);
  
  // Destroy Weather Layer
  text_layer_destroy(s_location_layer);
  
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  
  //Update time
  update_time();
  
  // Battery update
  handle_battery(battery_state_service_peek());

  // Get weather update every 30 minutes
  if(tick_time->tm_min % 30 == 0) {
  
  // Begin dictionary
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  // Add a key-value pair
  dict_write_uint8(iter, 0, 0);

  // Send the message!
  app_message_outbox_send();
}
  
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Store incoming information
  static char temperature_buffer[8];
  static char conditions_buffer[32];
  static char weather_layer_buffer[32];
  static char weather_location_buffer[32];
  
  // Read first item
  Tuple *t = dict_read_first(iterator);

  // For all items
  while(t != NULL) {
    // Which key was received?
    switch(t->key) {
    case KEY_TEMPERATURE:
      snprintf(temperature_buffer, sizeof(temperature_buffer), "%d° F", (int)t->value->int32);
      break;
    case KEY_CONDITIONS:
      snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", t->value->cstring);
      break;
    case KEY_LOCATION:
      snprintf(weather_location_buffer, sizeof(weather_location_buffer), "%s", t->value->cstring);
      text_layer_set_text(s_location_layer, weather_location_buffer);
      break;
    default:
      APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
      break;
    }

    // Look for next item
    t = dict_read_next(iterator);
  }
  
  // Assemble full string and display
  snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s\n%s", temperature_buffer, conditions_buffer);
  text_layer_set_text(s_weather_layer, weather_layer_buffer);
  
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();
  

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
 
  // Make sure the time is displayed from the start
  update_time();
  
    // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  
}  
  
static void deinit() {
 // Destroy Window
    window_destroy(s_main_window);  
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}