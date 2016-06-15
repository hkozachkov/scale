
//monitors the button, debouncing, and setting the indicators for it having been pressed or long-presseed
void watch_right_button(){
  // read the state of the switch into a local variable:
  int reading = digitalRead(RIGHT_BUTTON_PIN);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH),  and you've waited
  // long enough since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != previous_right_button_state) {
    // reset the debouncing timer
    right_last_debounce_time = millis();
  }
  
  if ((millis() - right_last_debounce_time) > DEBOUNCE_DELAY) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the current, debounced reading from the button is different from its previous debounced state
    if (reading != right_button_state) {
      right_button_state = reading;

      //if the button has just been released, implying it had been pressed
      if (right_button_state == BUTTON_UP) {
        right_button_pressed_indicator_state = true; 
        //if the button has been held long enough, set the indicator to true
        if ((millis()-right_last_legitimate_state_change) > RIGHT_BUTTON_HOLD_TIME){
          right_button_long_pressed_indicator_state = true;
        }     
      }
      right_last_legitimate_state_change = millis(); //if the true button state has changed, note the time
    }
  }
  
  previous_right_button_state = reading;
}




//monitors the button, debouncing, and setting the indicators for it having been pressed or long-presseed
void watch_left_button(){
  // read the state of the switch into a local variable:
  int reading = digitalRead(LEFT_BUTTON_PIN);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH),  and you've waited
  // long enough since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != previous_left_button_state) {
    // reset the debouncing timer
    left_last_debounce_time = millis();
  }
  
  if ((millis() - left_last_debounce_time) > DEBOUNCE_DELAY) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the current, debounced reading from the button is different from its previous debounced state
    if (reading != left_button_state) {
      left_button_state = reading;

      //if the button has just been released, implying it had been pressed
      if (left_button_state == BUTTON_UP) {
        left_button_pressed_indicator_state = true; 
        //if the button has been held long enough, set the indicator to true
        if ((millis()-left_last_legitimate_state_change) > LEFT_BUTTON_HOLD_TIME){
          left_button_long_pressed_indicator_state = true;
        }     
      }
      left_last_legitimate_state_change = millis(); //if the true button state has changed, note the time
    }
  }
  
  previous_left_button_state = reading;
}

