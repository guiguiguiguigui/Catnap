
#include <pebble.h>


static Window *s_main_window;
static TextLayer *s_time_layer;

char *itoa(int num){
    static char buff[20] = {};
    int i = 0, temp_num = num, length = 0;
    char *string = buff;
    if(num >= 0) {
      // count how many characters in the number
      while(temp_num) {
        temp_num /= 10;
        length++;
      }
      // assign the number to the buffer starting at the end of the 
      // number and going to the begining since we are doing the
      // integer to character conversion on the last number in the
      // sequence
      for(i = 0; i < length; i++) {
        buff[(length-1)-i] = '0' + (num % 10);
        num /= 10;
      }
      buff[i] = '\0'; // can't forget the null byte to properly end our string
      return buff;
    }
    else return "Unsupported Number";
    return string;
}

static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Create the TextLayer with specific bounds
  s_time_layer = text_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(58, 52), bounds.size.w, 50));

  int stepsToday = 0;
  #if defined(PBL_HEALTH)

    // Make a timestamp for now
  time_t end = time(NULL);

  // Make a timestamp for the last hour's worth of data
  time_t start = end - SECONDS_PER_HOUR;

  // Check data is available
  HealthServiceAccessibilityMask result = 
      health_service_metric_accessible(HealthMetricStepCount, start, end);
  if(result & HealthServiceAccessibilityMaskAvailable) {
    // Data is available! Read it
    HealthValue steps = health_service_sum(HealthMetricStepCount, start, end);
    stepsToday = (int)steps;
  } 
  #endif

  char* totalSteps = itoa(stepsToday);

  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_time_layer, GColorBlack);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text(s_time_layer, totalSteps);
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
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