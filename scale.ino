#include <Adafruit_GFX.h>

#include <Adafruit_ST7735.h>

#include <SPI.h>
//#include <TFT.h>

#include "HX711.h"



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
#define CS   10
#define DC   9
#define RESET  8

//button definitions
#define BUTTON_PIN  2
#define BUTTON_DOWN LOW
#define BUTTON_UP HIGH

//TFT TFTscreen = TFT(CS, DC, RESET);
Adafruit_ST7735 tft = Adafruit_ST7735(CS, DC, RESET);

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
static const long CALIBRATION_REQUEST_BUTTON_HOLD_TIME = 5000;

//*********************************************************************************************
//logging configurations

//time interval between logging datapoints, in ms
static const long DELAY_BETWEEN_DATAPOINTS = 2000;

//*********************************************************************************************
//global variables


// button variables
bool button_pressed_indicator_state = false;  
bool button_long_pressed_indicator_state = false;       
int button_state = BUTTON_UP;             // the current reading from the input pin
int previous_button_state = BUTTON_UP;   // the previous reading from the input pin

// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long last_debounce_time = 0;  // the last time the output pin was toggled
long last_legitimate_state_change = 0;


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

//*********************************************************************************************




void setup_button(){
  pinMode(BUTTON_PIN, INPUT);

}

void setup_scale(){
  //scale.power_down();             // put the ADC in sleep mode
  //delay(5000);
  //scale.power_up();
  scale.set_scale(calibration);
  scale.tare(REPEATS);
}

void setup_logging(){
  Serial.begin(38400);
  Serial.println("Time (ms), Force");
}

void setup() {
  setup_screen();
  setup_button();
  setup_scale();
  setup_logging();
}


//monitors the button, debouncing, and setting the indicators for it having been pressed or long-presseed
void watch_button(){
  // read the state of the switch into a local variable:
  int reading = digitalRead(BUTTON_PIN);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH),  and you've waited
  // long enough since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != previous_button_state) {
    // reset the debouncing timer
    last_debounce_time = millis();
  }
  
  if ((millis() - last_debounce_time) > DEBOUNCE_DELAY) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the current, debounced reading from the button is different from its previous debounced state
    if (reading != button_state) {
      button_state = reading;

      //if the button has just been released, implying it had been pressed
      if (button_state == BUTTON_UP) {
        button_pressed_indicator_state = true; 
        //if the button has been held long enough, set the indicator to true
        if ((millis()-last_legitimate_state_change) > CALIBRATION_REQUEST_BUTTON_HOLD_TIME){
          button_long_pressed_indicator_state = true;
        }     
      }
      last_legitimate_state_change = millis(); //if the true button state has changed, note the time
    }
  }
  
  previous_button_state = reading;
}



void watch_scale(){

  //if the scale is calibrated and a long enough time has passed since the last reading,
  //take a reading, update the force display
  if(calibration_state == CALIBRATED && (millis()-last_reading_time > DELAY_BETWEEN_READINGS)){
   
    current_force_reading = scale.get_units(REPEATS);

    update_force_display();
    //update_force_display_text();
    //update_force_display_graph();
    
    
   last_reading_time = millis();
  }
  

   //Serial.println(scale.read());  
}

void watch_calibration(){
  if(button_long_pressed_indicator_state==true){
    calibration_state = UNCALIBRATED;
    button_pressed_indicator_state = true;
    button_long_pressed_indicator_state = false;
  }
  
  if(calibration_state != CALIBRATED && button_pressed_indicator_state==true){
    button_pressed_indicator_state=false; // reset the button pressed indicator
    switch(calibration_state){
      case UNCALIBRATED:
        update_heading("ATTENTION");
        update_body_text("CALIBRATION MODE: Remove all weight and press button");
        calibration_state = CALIBRATION_TARE;
        current_force_reading=0;
        break;
      case CALIBRATION_TARE:
        scale.set_scale();
        scale.tare(CALIBRATION_REPEATS);
        update_body_text("CALIBRATION MODE: Add standard weight and press button");
        calibration_state = STANDARD;
        current_force_reading=100;
        break;
      case STANDARD:
        calibration = scale.get_units(CALIBRATION_REPEATS)/CALIBRATION_STANDARD_WEIGHT;
        scale.set_scale(calibration);
         
        //Serial.print("Scaling Factor: \t\t");
        //Serial.println(calibration);                 // print a raw reading from the ADC
        calibration_state = TARE;
        update_body_text("CALIBRATION MODE: Remove all weight and press button");
        current_force_reading=0;
        break;
      case TARE:
        scale.tare(CALIBRATION_REPEATS);
        calibration_state = CALIBRATED;
        update_body_text("CALIBRATED");
        update_heading("Force (g):");
        current_force_reading=0;
        break;  
    }
    
  }

  if(calibration_state == CALIBRATED && button_pressed_indicator_state==true){
    button_pressed_indicator_state=false; // reset the button pressed indicator
    scale.tare(CALIBRATION_REPEATS);
    calibration_state = CALIBRATED;
    update_heading("TARE");
    delay(1000);
    update_heading("Force (g):");
  }
 

  
}

void watch_logging(){
  if(calibration_state == CALIBRATED && (millis()-last_reading_time > DELAY_BETWEEN_READINGS)){
    String s = "" + String(last_reading_time) + "," + String(current_force_reading,3);
    Serial.println(s);
  }
  last_logging_datapoint_time = millis();
}


void loop() {
  
  watch_button();
  watch_calibration();
  watch_scale();
  watch_logging();
}
