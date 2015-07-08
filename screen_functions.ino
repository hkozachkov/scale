void clear_screen(){
  tft.fillScreen(BACKGROUND_COLOR);
}

void clear_body(){
  tft.fillRect(0,33,128,128,BACKGROUND_COLOR);
}

void update_heading(String s){
  tft.fillRect(0,0,128,32,BACKGROUND_COLOR);
  tft.setTextSize(2);
  tft.setCursor(0, 0);
  tft.setTextColor(HEADING_COLOR);
  tft.print(s);
  tft.setCursor(0, 50);
}

void update_body_text(String s){
  clear_body();
  tft.setTextSize(1);
  tft.setCursor(0, 50);
  tft.setTextColor(BODY_TEXT_COLOR);
  tft.setTextWrap(1);
  tft.println(s);
  display_mode = TEXT_DISPLAY;
}


//this doesn't work right yet
void update_force_display_graph(){
  if(display_mode != FORCE_DISPLAY_GRAPH){
    clear_body();
    
  }
  if (xPos >= 128) {
    xPos = 0;
    //TFTscreen.background(250,16,200); 
    force_color_toggle= !force_color_toggle;
  }else {
    xPos++;
  }
  display_mode = FORCE_DISPLAY_GRAPH;
  int graph_height = 160-map(abs(current_force_reading),0,200,0,128);
  //TFTscreen.line(xPos, TFTscreen.height() - graphHeight, xPos, TFTscreen.height());
  tft.drawLine(xPos,graph_height,xPos,160,FORCE_GRAPH_COLOR[force_color_toggle]);
  //delay(16);
  String s = "  " + String(current_force_reading,3) + "      ";
  tft.setCursor(0, 30);
  tft.setTextSize(2);
  tft.setTextColor(HEADING_COLOR,BACKGROUND_COLOR );
  tft.setTextWrap(0);
  tft.println(s);
}



void update_force_display_text(){
  String s = "  " + String(current_force_reading,3) + "      ";
  if(line_count > 5 || display_mode != FORCE_DISPLAY_TEXT){
    force_color_toggle= !force_color_toggle;
    //clear_body();
    tft.setCursor(0, 50);
    line_count=0;
    display_mode = FORCE_DISPLAY_TEXT;
  }
  tft.setTextSize(2);
  tft.setTextColor(FORCE_TEXT_COLOR[force_color_toggle],BACKGROUND_COLOR );
  tft.setTextWrap(0);
  tft.println(s);
  line_count++;
}


void update_force_display(){
  if(force_display_mode==FORCE_DISPLAY_GRAPH){
    update_force_display_graph();
  }else{
    update_force_display_text();
  }
}

void setup_screen(){
  
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  

  tft.setRotation(2);
  clear_screen();
  update_heading("Force (g):");

  
}
