/*
  RGB LED SHIELD EXAMPLE CODE
  The RGB LED SHIELD from www.Re-Innovation.co.uk is designed to control high current RGB
  LED boards such as those available from www.phenoptix.co.uk / www.bigclive.com
  
  A WS2801 RGB LED control IC is used, which holds the state of the LEDs.
  The outputs can control up to 1.5A per channel - enough for loads of LEDs
  
  This example code will gives a number of examples of how to control the output:
  
  First is scrolls through RED GREEN BLUE for 2 seconds each
  
  It then fades between RED GREEN BLUE
  
  It then uploads a number from a random seed and displays the colour, changing every 0.5seconds
  
  The Shield has the SDI connected to Arduino pin 9 and the CKI connected to Arduino pin 8.

  
  Modified:
  4/1/13  Matt Little
    
  This is based upon code from: Nathan Seidle
  SparkFun Electronics 2011
  
  This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).  
  This was written to control the WS2801 RGB LED strip:
  Controlling an LED strip with individually controllable RGB LEDs. This stuff is awesome.

 */

int SDI = 9; //Pin 9 of the Arduino is used as the SDI (Serial Data)
int CKI = 8; //Pin 8 of the Arduino is used as the CKI (Data clock)

#define STRIP_LENGTH 1 //This is to use more than one RGB LED board. In this case only one board connected
long strip_colors[STRIP_LENGTH];

long fade = 0x000000;  // A long integer for the fade function
long fade_change = 0x000000;  // A long int to do bitwise shifting on the fade value

void setup() {
  pinMode(SDI, OUTPUT);
  pinMode(CKI, OUTPUT);
  
  //Clear out the array
  for(int x = 0 ; x < STRIP_LENGTH ; x++)
    strip_colors[x] = 0;
    
  randomSeed(analogRead(0));  // Initialise a random number seed
  
  Serial.begin(9600);  // Just for testing
  
  
}

void loop() {

  // Example 1
  //Output known bright colours to the LED.
  // Data is in the format  RRGGBB, where:
  //   RR is hexicecimal number which sets the red brightness (00-FF, or 0-256)
  //   GG is hexicecimal number which sets the green brightness (00-FF, or 0-256)
  //   BB is hexicecimal number which sets the blue brightness (00-FF, or 0-256)
  
  strip_colors[0] = 0xFF0000; //Set the data to Bright Red  
  post_frame(); //Push the current color frame to the strip
  delay(2000);  // Wait 2 seconds
  strip_colors[0] = 0x00FF00; //Set the data to Bright Green
  post_frame(); //Push the current color frame to the strip
  delay(2000);
  strip_colors[0] = 0x0000FF; //Set the data to Bright Blue
  post_frame(); //Push the current color frame to the strip
  delay(2000);
  
  //Example 2
  //Fade in red then green then blue until the output is white
  
  // First blank the output
  strip_colors[0] = fade;
  post_frame(); //Push the current color frame to the strip  
  delay(500);
  //Fade in red
  for(int i=0;i<=255;i++)
  {
    fade = i;
    fade = (fade<<16);
    Serial.println(fade, BIN);  // Just for testing
    strip_colors[0] = fade;
    post_frame(); //Push the current color frame to the strip 
    delay(5);  // Very short delay
  }
  
  //Fade in green
  for(int i=0;i<=255;i++)
  {
    fade =  i;
    fade = (fade<<8);
    fade = fade + 0xFF0000;
    Serial.println(fade, BIN);  // Just for testing
    strip_colors[0] = fade;
    post_frame(); //Push the current color frame to the strip 
    delay(5);  // Very short delay
  }  

  //Fade in blue
  for(int i=0;i<=255;i++)
  {
    fade =  i;
    fade = fade + 0xFFFF00;
    Serial.println(fade, BIN);  // Just for testing
    strip_colors[0] = fade;
    post_frame(); //Push the current color frame to the strip 
    delay(5);  // Very short delay
  }  
  
  //Example 3
  //Output a random colour every 500ms to the display
  while(1){ //Do nothing
    addRandom();  // This adds a random colour as the output
    post_frame(); //Push the current color frame to the strip
    delay(500);                  // wait for a second
  }
}

//Throws random colors down the strip array
void addRandom(void) {
  int x;
  
  //First, shuffle all the current colors down one spot on the strip
  for(x = (STRIP_LENGTH - 1) ; x > 0 ; x--)
    strip_colors[x] = strip_colors[x - 1];
    
  //Now form a new RGB color
  long new_color = 0;
  for(x = 0 ; x < 3 ; x++){
    new_color <<= 8;
    new_color |= random(0xFF); //Give me a number from 0 to 0xFF
    //new_color &= 0xFFFFF0; //Force the random number to just the upper brightness levels. It sort of works.
  }
  
  strip_colors[0] = new_color; //Add the new random color to the strip
}

//Takes the current strip color array and pushes it out
void post_frame (void) {
  //Each LED requires 24 bits of data
  //MSB: R7, R6, R5..., G7, G6..., B7, B6... B0 
  //Once the 24 bits have been delivered, the IC immediately relays these bits to its neighbor
  //Pulling the clock low for 500us or more causes the IC to post the data.

  for(int LED_number = 0 ; LED_number < STRIP_LENGTH ; LED_number++) {
    long this_led_color = strip_colors[LED_number]; //24 bits of color data

    for(byte color_bit = 23 ; color_bit != 255 ; color_bit--) {
      //Feed color bit 23 first (red data MSB)
      
      digitalWrite(CKI, LOW); //Only change data when clock is low
      
      long mask = 1L << color_bit;
      //The 1'L' forces the 1 to start as a 32 bit number, otherwise it defaults to 16-bit.
      
      if(this_led_color & mask) 
        digitalWrite(SDI, HIGH);
      else
        digitalWrite(SDI, LOW);
  
      digitalWrite(CKI, HIGH); //Data is latched when clock goes high
    }
  }

  //Pull clock low to put strip into reset/post mode
  digitalWrite(CKI, LOW);
  delayMicroseconds(500); //Wait for 500us to go into reset
}
