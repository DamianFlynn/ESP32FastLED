
#define FASTLED_SHOW_CORE 0
#include <FastLED.h>

#define dataPin 16

static const int16_t numLeds = 120;
CRGB leds[numLeds];  //what actually gets displayed

static TaskHandle_t FastLEDshowTaskHandle2 = 0;
static TaskHandle_t userTaskHandle = 0;

void FastLEDShowESP32()
{
  if (userTaskHandle == 0)
  {
    const TickType_t xMaxBlockTime = pdMS_TO_TICKS(200);
    // -- Store the handle of the current task, so that the show task can
    //    notify it when it's done
    // noInterrupts();
    userTaskHandle = xTaskGetCurrentTaskHandle();

    // -- Trigger the show task
    xTaskNotifyGive(FastLEDshowTaskHandle2);
    //to the contrary to the other one we do not wait for the display task to come back
  }
}

void FastLEDshowTask2(void *pvParameters)
{
  const TickType_t xMaxBlockTime = pdMS_TO_TICKS(500);
  // -- Run forever...
  for (;;)
  {
    // -- Wait for the trigger
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    FastLED.show();
    // controller.showPixels();
    userTaskHandle = 0; //so we can't have two display tasks at the same time
  }
}

void setup()
{
  Serial.begin(115200);
	Serial.println("ESP32 FastLED");

  // Initial pin states
  pinMode(dataPin, OUTPUT);
  digitalWrite(dataPin, LOW);

  //
  xTaskCreatePinnedToCore(FastLEDshowTask2, "FastLEDshowTask2", 1000, NULL, 3, &FastLEDshowTaskHandle2, FASTLED_SHOW_CORE);
  FastLED.addLeds<WS2812B, dataPin, GRB>(leds, numLeds);
  FastLED.setBrightness(255);
  FastLED.clear();
}


void fadeall() { for(int i = 0; i < numLeds; i++) { leds[i].nscale8(250); } }


void effectCylon() {
// Cylon Effect

  static uint8_t hue = 0;
	Serial.print("  >>  ");
	// First slide the led in one direction
	for(int i = 0; i < numLeds; i++) {
		// Set the i'th led to red 
		leds[i] = CHSV(hue++, 255, 255);
		// Show the leds
	  FastLEDShowESP32();
		// now that we've shown the leds, fade to black
		fadeall();
		// Wait a little bit before we loop again
		delay(10);
	}

	// Now go in the other direction.  
  Serial.print("  <<  ");
	for(int i = (numLeds)-1; i >= 0; i--) {
		// Set the i'th led to red 
		leds[i] = CHSV(hue++, 255, 255);
		// Show the leds
		FastLEDShowESP32();
		// now that we've shown the leds, fade to black
		fadeall();
		// Wait a little bit before we loop again
		delay(10);
    
	}

}

void effectBounce() {
  // Let try a loop to move a LED Dot
  for(int dot = 0; dot < numLeds; dot++) { 
    leds[dot] = CRGB::Blue;
    FastLEDShowESP32();
    // clear this led for the next time around the loop
    leds[dot] = CRGB::Black;
    delay(30);
  }
}


void loop()
{
  effectCylon();
}