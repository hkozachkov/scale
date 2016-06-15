char filename[16];
File root;

void setup_sd_card(){
  Serial.print("Initializing SD card...");
  //if (!SD.begin(SD_CS, SPI_DO, SPI_DI, SPI_SCK)) {
  if (!SD.begin(SD_CS)) {
    Serial.println("failed!");
    return;
  }
  Serial.println("SD card OK!");

  //prints the directory structure of the sd card
  root = SD.open("/");
  printDirectory(root, 0);

  
  increment_file();
  
}

//incriments to a new file
void increment_file(){
  
  int n = 0;
  snprintf(filename, sizeof(filename), "data%03d.txt", n); // includes a three-digit sequence number in the file name
  while(SD.exists(filename)) {
    n++;
    snprintf(filename, sizeof(filename), "data%03d.txt", n);
  }
  File dataFile = SD.open(filename,FILE_READ);
  Serial.println(n);
  Serial.println(filename);
  dataFile.close();
  //now filename[] contains the name of a new file
}


void watch_logging(){
  if(left_button_long_pressed_indicator_state==true){
    increment_file();
    update_body_text("Next File");
    left_button_long_pressed_indicator_state=false;
    left_button_pressed_indicator_state=false;
  }

  
  if(left_button_pressed_indicator_state==true){
    
    if(!logging_on){
      logging_on = true;
      
      update_heading("Logging");
    }else{
      logging_on = false;
      
      update_heading("NOT Logging");
    }
    left_button_pressed_indicator_state = false;
  }
  
  if(calibration_state == CALIBRATED && (millis()-last_reading_time > DELAY_BETWEEN_READINGS) && logging_on){
    String s = "" + String(last_reading_time) + "," + String(current_force_reading,3);
    //Serial.println(s);
    write_line_to_file(s);
  }
  last_logging_datapoint_time = millis();
}



void write_line_to_file(String dataString){


  // open the file stored in the filename[] global variable.
  //note that only one file can be open at a time,
  // so you have to close this one before opening another.

  File dataFile = SD.open(filename, FILE_WRITE);
  
  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening file");
  }
}


//recursivly print directory sturcture
//with indentation
void printDirectory(File dir, int numTabs) {
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}

