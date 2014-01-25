#include <pebble.h>

static Window *window;
static Layer *display_layer;

#define NUM_OF_HOUR_ROWS 5
#define NUM_OF_MINUTE_ROWS 6

#define CIRCLE_RADIUS 10
#define SCREEN_W_PADDING 15
#define PADDING 6

//Screen Width = 144
//Screen Height w status bar = 168 - 16 = 152
//Screen Height w_o Status Bar = 168
#define SCREEN_W 144
#define SCREEN_H 168

unsigned short int hour;
unsigned short int minute;
time_t now;
struct tm *t;

unsigned short binaryHourArray[NUM_OF_HOUR_ROWS];
unsigned short binaryMinuteArray[NUM_OF_MINUTE_ROWS];

GTextAlignment textAlignment = GTextAlignmentCenter;
GTextOverflowMode overFlowMode = GTextOverflowModeFill;

//Function Declarations
void drawFillCircle(GContext* ctx, unsigned short binaryValue, GPoint point) {
	if (binaryValue == 1) {
		graphics_fill_circle(ctx, point, CIRCLE_RADIUS);
	} else {
		graphics_draw_circle(ctx, point, CIRCLE_RADIUS);
	}
}


unsigned short int powerFunction(unsigned short int base, unsigned short int toPower) {
	unsigned short int returnValue = 1;
	
	if (toPower != 0) {
		for (int powerCount = 1 ; powerCount <= toPower; powerCount++) {
			returnValue = base * returnValue;
		}
	}
	return returnValue;
}

void initailize_layout(Layer *me, GContext* ctx) {
	
	now = time(NULL);
	t = localtime(&now);
	GPoint point;
			
	if (t->tm_hour != hour) {
		hour = t->tm_hour;
		unsigned short tempValue = hour;
		for (int i = NUM_OF_HOUR_ROWS -1; i >= 0 ; i--) {

			unsigned short powValue = powerFunction(2, i);
			if (powValue > tempValue) {
				binaryHourArray[i] = 0;
			} else {
				tempValue = tempValue - powValue;
				binaryHourArray[i] = 1;
			}
		}
	}
	
	if (t->tm_min != minute) {
		minute = t->tm_min;
		unsigned short tempValue = minute;
		for (int i = NUM_OF_MINUTE_ROWS-1; i >= 0 ; i--) {

			unsigned short powValue = powerFunction(2, i);
			if (powValue > tempValue) {
				binaryMinuteArray[i] = 0;
			} else {
				tempValue = tempValue - powValue;
				binaryMinuteArray[i] = 1;
			}
		}
	}
	
	for (int i = NUM_OF_HOUR_ROWS - 1; i >= 0 ; i--) {

				//X coordinate			Y coordinate
		point = GPoint(SCREEN_W_PADDING * 2.5, 20 + (((SCREEN_H / NUM_OF_HOUR_ROWS) * i) - PADDING )) ;
		//Text here
		drawFillCircle(ctx , binaryHourArray[i], point);		
	}

	for (int i = NUM_OF_MINUTE_ROWS -1; i >= 0 ; i--) {


		point = GPoint( SCREEN_W - (SCREEN_W_PADDING * 2.5), 20 + (((SCREEN_H / NUM_OF_MINUTE_ROWS) * i) - PADDING));
		drawFillCircle(ctx , binaryMinuteArray[i] , point );
		
	}

}

void display_layer_update_callback(Layer *me, GContext* ctx) {

    //clean screen 
    graphics_context_set_fill_color(ctx, GColorBlack);

    graphics_context_set_stroke_color(ctx, GColorWhite);
    graphics_context_set_fill_color(ctx, GColorWhite);

    initailize_layout(me,ctx);

}

void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(display_layer);
}

static void init(void) {
  
  window = window_create();
  window_stack_push(window, true);

  window_set_background_color(window, GColorBlack);

  Layer *root_layer = window_get_root_layer(window);
  GRect frame = layer_get_frame(root_layer);

  // Init the layer for the display
  display_layer = layer_create(frame);
  layer_set_update_proc(display_layer, &display_layer_update_callback);
  layer_add_child(root_layer, display_layer);

  tick_timer_service_subscribe(MINUTE_UNIT, &handle_minute_tick);
}


static void deinit(void) {
  layer_destroy(display_layer);
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
