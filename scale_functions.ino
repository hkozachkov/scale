
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
  if(right_button_long_pressed_indicator_state==true){
    calibration_state = UNCALIBRATED;
    right_button_pressed_indicator_state = true;
    right_button_long_pressed_indicator_state = false;
  }
  
  if(calibration_state != CALIBRATED && right_button_pressed_indicator_state==true){
    right_button_pressed_indicator_state=false; // reset the button pressed indicator
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

  if(calibration_state == CALIBRATED && right_button_pressed_indicator_state==true){
    right_button_pressed_indicator_state=false; // reset the button pressed indicator
    scale.tare(CALIBRATION_REPEATS);
    calibration_state = CALIBRATED;
    update_heading("TARE");
    delay(1000);
    update_heading("Force (g):");
  }
 

  
}
