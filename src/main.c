#include <pebble.h>
#include "elements.h"

static TextLayer* text_layer_init(GRect location, GColor background, GTextAlignment alignment, int font)
{
	TextLayer *layer = text_layer_create(location);
	text_layer_set_text_color(layer, GColorBlack);
	text_layer_set_background_color(layer, background);
	text_layer_set_text_alignment(layer, alignment);
	if(font == 1){
		text_layer_set_font(layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_REG_35)));
	}
	else if(font == 2){
		text_layer_set_font(layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	}
	else if(font == 3){
		text_layer_set_font(layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
	}
	else if(font == 4){
		text_layer_set_font(layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
	}
	return layer;
}


static void weatherPlease(void) {
  Tuplet value = TupletInteger(1, 1);

  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  if (iter == NULL) {
    return;
  }

  dict_write_tuplet(iter, &value);
  dict_write_end(iter);

  app_message_outbox_send();
}
	
void tick_handler(struct tm *t, TimeUnits units_changed){
	static char time_buffer[] = "00:00:00";
	static char month_buffer[] = "September";
	static char day_buffer[] = "26";
	static char time2_buffer[] = "00:00";
	static char adv_month_buffer[] = "September 2014.";
	if(clock_is_24h_style()){
		strftime(time_buffer, sizeof(time_buffer), "%H:%M:%S", t);
	}
	else{
		strftime(time_buffer,sizeof(time_buffer),"%I:%M:%S", t);
	}
	strftime(month_buffer,sizeof(month_buffer),"%B", t);
	strftime(day_buffer, sizeof(day_buffer), "%d", t);
	if(clock_is_24h_style()){
		strftime(time2_buffer, sizeof(time2_buffer), "%H:%M", t);
	}
	else{
		strftime(time2_buffer,sizeof(time2_buffer),"%I:%M", t);
	}
	strftime(adv_month_buffer,sizeof(adv_month_buffer),"%B %Y", t);
	
	text_layer_set_text(time_layer, time_buffer);
	text_layer_set_text(month_layer, month_buffer);
	text_layer_set_text(day_layer, day_buffer);
	text_layer_set_text(time2_layer, time2_buffer);
	text_layer_set_text(adv_month_layer, adv_month_buffer);
	
	int minute = t->tm_min;
	int second = t->tm_sec;
	
	if(minute % 30 == 0 || minute == 0){
		if(second == 0){
			weatherPlease();
		}
	}
}

void refresh_weather_data(){
	gbitmap_destroy(weather_image);
	weather_image = gbitmap_create_with_resource(weather_icons[settings.previousicon]);
	bitmap_layer_set_bitmap(weather_image_layer, weather_image);
	
	static char weather_buffer[] = "-100";
	snprintf(weather_buffer, sizeof(weather_buffer), "%d", settings.previoustemp);
	text_layer_set_text(weather_text_layer, weather_buffer);
}

void process_tuple(Tuple *t)
{
	int key = t->key;
	int value = t->value->int32;
  switch (key) {
	case 0:
	  if(settings.temppref){
		settings.previoustemp = (1.8*(value-273)+32);;
	  }
	  else{
		settings.previoustemp = (value - 273.15);
	  }
	  refresh_weather_data();
	  break;
	case 1:
	  settings.previousicon = value;
	  refresh_weather_data();
	  break;
	case 2:
	  settings.temppref = value;
	  break;
	case 3:
	  settings.theme = value;
	  layer_set_hidden(inverter_layer_get_layer(theme), settings.theme);
	  break;
  }
}

static void in_received_handler(DictionaryIterator *iter, void *context) 
{
	(void) context;

	Tuple *t = dict_read_first(iter);
	if(t)
	{
		process_tuple(t);
	}
	while(t != NULL)
	{
		t = dict_read_next(iter);
		if(t)
		{
			process_tuple(t);
		}
	}
}

void window_load(Window *window){
	Layer *window_layer = window_get_root_layer(window);
	
	base_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
	bitmap_layer_set_bitmap(base_layer, base_image);
	layer_add_child(window_layer, bitmap_layer_get_layer(base_layer));
	
	weather_image_layer = bitmap_layer_create(GRect(-46, -32, 144, 168));
	bitmap_layer_set_bitmap(weather_image_layer, weather_image);
	layer_add_child(window_layer, bitmap_layer_get_layer(weather_image_layer));
	
	weather_text_layer = text_layer_init(GRect(-19, 15, 144, 45), GColorClear, GTextAlignmentCenter, 4);
	layer_add_child(window_layer, text_layer_get_layer(weather_text_layer));
	
	time_layer = text_layer_init(GRect(0, 95, 144, 45), GColorClear, GTextAlignmentCenter, 1);
	layer_add_child(window_layer, text_layer_get_layer(time_layer));
	
	month_layer = text_layer_init(GRect(74, 18, 70, 19), GColorClear, GTextAlignmentRight, 2);
	layer_add_child(window_layer, text_layer_get_layer(month_layer));
	
	day_layer = text_layer_init(GRect(102, 40, 42, 45), GColorClear, GTextAlignmentRight, 1);
	layer_add_child(window_layer, text_layer_get_layer(day_layer));
	
	time2_layer = text_layer_init(GRect(116, -2, 28, 18), GColorClear, GTextAlignmentCenter, 3);
	layer_add_child(window_layer, text_layer_get_layer(time2_layer));
	
	adv_month_layer = text_layer_init(GRect(3, 67, 80, 18), GColorClear, GTextAlignmentCenter, 3);
	layer_add_child(window_layer, text_layer_get_layer(adv_month_layer));
	
	theme = inverter_layer_create(GRect(0, 0, 144, 168));
	layer_add_child(window_layer, inverter_layer_get_layer(theme));
	
	layer_set_hidden(inverter_layer_get_layer(theme), settings.theme);
	
	struct tm *t;
  	time_t temp;        
  	temp = time(NULL);        
  	t = localtime(&temp);
	
	tick_handler(t, SECOND_UNIT);
	
	refresh_weather_data();
}

void window_unload(Window *window){
	bitmap_layer_destroy(base_layer);
}
	
void init(){
	window = window_create();
	window_set_window_handlers(window, (WindowHandlers){
		.load = window_load,
		.unload = window_unload,
	});
	tick_timer_service_subscribe(SECOND_UNIT, &tick_handler);
	base_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BASE);
	weather_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PARTIALLY_SUNNY);
	
	app_message_register_inbox_received(in_received_handler);					 
	app_message_open(512, 512);
	weatherPlease();
	
	if(persist_exists(SETTINGS_KEY)){
		int value = persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
		APP_LOG(APP_LOG_LEVEL_INFO, "%d bytes read", value);
	}
	else{
		settings.theme = 1;
		settings.previoustemp = 666;
		settings.previousicon = 0;
		settings.temppref = 1;
	}
	
	window_stack_push(window, true);
}

void deinit(){
	gbitmap_destroy(base_image);
	tick_timer_service_unsubscribe();
	int value = persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
	APP_LOG(APP_LOG_LEVEL_INFO, "%d bytes written", value);
}

int main(){
	init();
	app_event_loop();
	deinit();
}