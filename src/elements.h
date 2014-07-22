#define SETTINGS_KEY 0
Window *window;
GBitmap *base_image, *weather_image;
BitmapLayer *base_layer, *weather_image_layer;
TextLayer *time_layer, *month_layer, *day_layer, *time2_layer, *adv_month_layer, *weather_text_layer;
InverterLayer *theme;

static const uint8_t weather_icons[] = {
	RESOURCE_ID_IMAGE_THUNDER,
	RESOURCE_ID_IMAGE_LIGHT_RAIN,
	RESOURCE_ID_IMAGE_RAIN,
	RESOURCE_ID_IMAGE_SNOW,
	RESOURCE_ID_IMAGE_FOG,
	RESOURCE_ID_IMAGE_SUNNY,
	RESOURCE_ID_IMAGE_PARTIALLY_SUNNY,
	RESOURCE_ID_IMAGE_CLOUDY
};

typedef struct persist{
	int previoustemp;
	uint8_t previousicon;
	bool temppref;
	bool theme;
}persist;

persist settings = {
	.previoustemp = 0,
	.previousicon = 0,
	.temppref = 0,
	.theme = 1,
};