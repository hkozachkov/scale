#include <Adafruit_GFX.h>

#include <Adafruit_ST7735.h>

#include <SPI.h>
//#include <TFT.h>

#include "HX711.h"
#include <SD.h>



//***********************************************************************************************
//device state and mode definitions

//the screen display modes
#define FORCE_DISPLAY_TEXT  0
#define TEXT_DISPLAY  1
#define FORCE_DISPLAY_GRAPH 2

//the states of the scale with regard to calibration
#define CALIBRATED  0
#define TARE  1
#define STANDARD  2
#define CALIBRATION_TARE  3
#define UNCALIBRATED  4


//***********************************************************************************************
//hardware  definitions

//communication pins for the lcd. 
//the other pins for the screen are defined automatically since we are using the default SPI
//#define TFT_CS   10
//#define DC   9
//#define RESET  8

#define SD_CS    4  // Chip select line for SD card



// TFT display and SD card will share the hardware SPI interface.
// Hardware SPI pins are specific to the Arduino board type and
// cannot be remapped to alternate pins.  For Arduino Uno,
// Duemilanove, etc., pin 11 = MOSI, pin 12 = MISO, pin 13 = SCK.
#define SPI_SCK 13
#define SPI_DI  12
#define SPI_DO  11

#define SD_CS    4  // Chip select line for SD card
#define TFT_CS  10  // Chip select line for TFT display
#define TFT_DC   9  // Data/command line for TFT
#define TFT_RST  8  // Reset line for TFT (or connect to +5V)



//button definitions
#define RIGHT_BUTTON_PIN  2
#define LEFT_BUTTON_PIN 5
#define BUTTON_DOWN LOW
#define BUTTON_UP HIGH

//TFT TFTscreen = TFT(TFT_CS, DC, RESET);
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, SPI_DO, SPI_SCK, TFT_RST);


// HX711.DOUT  - pin #A1
// HX711.PD_SCK - pin #A0
// gain = 128
HX711 scale(A1, A0, 128); 


//*********************************************************************************************
//screen configurations


//color definitions for the screen
static const int BACKGROUND_COLOR =  ST7735_BLACK;
static const int HEADING_COLOR = ST7735_WHITE;
static const int BODY_TEXT_COLOR = ST7735_WHITE;
static const int
  FORCE_TEXT_COLOR[] = {tft.Color565(0x11,0xE0,0xFF),ST7735_YELLOW};
static const int
  FORCE_GRAPH_COLOR[] = {ST7735_BLUE,ST7735_GREEN};

static const int INITIAL_FORCE_DISPLAY_MODE = FORCE_DISPLAY_TEXT;

//*********************************************************************************************
//scale configurations


// the weight of the calibration standard, in grams
static const int CALIBRATION_STANDARD_WEIGHT = 100;

//number of samples to average over for each measurement
static const int REPEATS = 3;

//number of samples to average over for each caibration measurement
static const int CALIBRATION_REPEATS = 10;

//time to wait between readings, in ms
static const long DELAY_BETWEEN_READINGS = 500; 

//initial calibration .. updated when a calibration is performed
static const float INITIAL_CALIBRATION = -6819.80;

//*********************************************************************************************
//button configurations
 

static const long DEBOUNCE_DELAY = 50;    // the debounce time; increase if the output flickers
static const long RIGHT_BUTTON_HOLD_TIME = 5000;
static const long LEFT_BUTTON_HOLD_TIME = 2000;

//*********************************************************************************************
//logging configurations

//time interval between logging datapoints, in ms
static const long DELAY_BETWEEN_DATAPOINTS = 2000;

//*********************************************************************************************
//global variables


// right button variables
bool right_button_pressed_indicator_state = false;  
bool right_button_long_pressed_indicator_state = false;       
int right_button_state = BUTTON_UP;             // the current reading from the input pin
int previous_right_button_state = BUTTON_UP;   // the previous reading from the input pin

// left button variables
bool left_button_pressed_indicator_state = false;  
bool left_button_long_pressed_indicator_state = false;       
int left_button_state = BUTTON_UP;             // the current reading from the input pin
int previous_left_button_state = BUTTON_UP;   // the previous reading from the input pin

// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long right_last_debounce_time = 0;  // the last time the output pin was toggled
long right_last_legitimate_state_change = 0;
long left_last_debounce_time = 0;  // the last time the output pin was toggled
long left_last_legitimate_state_change = 0;

//scale variables
int calibration_state= CALIBRATED; 
float current_force_reading;
float calibration = INITIAL_CALIBRATION; // initial guess ... is rewritted during calibration
long last_reading_time = 0; 

//screen variables
int force_display_mode = INITIAL_FORCE_DISPLAY_MODE;
int display_mode;

int line_count; //for force text mode
bool force_color_toggle; //for force text mode
int xPos; //for force graph mode

//logging variables
long last_logging_datapoint_time = 0; 
bool logging_on = false;

//*********************************************************************************************




void setup_buttons(){
  pinMode(RIGHT_BUTTON_PIN, INPUT);
  pinMode(LEFT_BUTTON_PIN, INPUT);

}

void setup_scale(){
  //scale.power_down();             // put the ADC in sleep mode
  //delay(5000);
  //scale.power_up();
  scale.set_scale(calibration);
  scale.tare(REPEATS);
}

void setup_serial(){
  Serial.begin(38400);
}

void setup_logging(){
  Serial.println("Time (ms), Force");
}

void setup() {

  setup_serial();
  setup_screen();
  setup_sd_card();
  setup_buttons();
  setup_scale();
  setup_logging();
}



void loop() {
  
  watch_right_button();
  watch_left_button();
  watch_calibration();
  watch_scale();
  watch_logging();
}
