#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>   // Needs a little change in original Adafruit library (See README.txt file)
#include <SPI.h>            // For SPI comm (needed for not getting compile error)
#include <Wire.h>           // For I2C comm, but needed for not getting compile error

#include "pitches.h"        // https://www.arduino.cc/en/Tutorial/toneMelody

boolean debug = true;

int time_green_tea = 120;  // s, 2 min
int time_black_tea = 210;  // s, 3.5 min
int time_herbal_tea = 300;  // s, 5 min

int temp_green_tea = 65;  // approx. value when brewing a cup with 80 degrees
int temp_black_tea = 87;  // approx. value when brewing a cup with 100 degrees
int temp_herbal_tea = 83;  // approx. value when brewing a cup with 100 degrees

int BUZZER = 8;
int TMP36 = A0;

const unsigned long BAUD_RATE = 115200;  // serial connection speed

int time_delay = 1000; // 1 s
#define REF_VOLTAGE 5.0
#define OLED_RESET  16  // Pin 16 -RESET digital signal
unsigned long timex;

Adafruit_SSD1306 display(OLED_RESET); // FOR I2C

void showMeasurement(float temperature, int timex);
void scrollText(String text);
void playMelody();

void initSerial();
void initOLEDDisplay();

void setup() {
  initSerial();
  initOLEDDisplay();

  pinMode(BUZZER, OUTPUT);
}

void loop()  {
  int sensor = analogRead(TMP36);
  int temperature = map(sensor, 0, 410, -50, 150); // arduino pro mini
  
  if( debug ) { Serial.print(temperature); Serial.println(" degrees Celsius");}

  if( debug ) {
    float voltage = sensor * REF_VOLTAGE;
    voltage /= 1024.0; 
    Serial.print(voltage); Serial.println(" volts");
  }
  
  timex = millis();
  timex /= 1000;

  if( debug ) { Serial.print("Time: "); Serial.print(timex); Serial.println(" s"); } // prints time since program started

  showMeasurement(temperature, timex);

  if( timex == time_green_tea || temperature <= temp_green_tea ) {
    scrollText(String("Green tea is ready."));
    playMelody();
  }
  if( timex == time_black_tea || temperature <= temp_black_tea ) {
    scrollText(String("Black tea is ready."));
    playMelody();
  }
  if( timex == time_herbal_tea || temperature <= temp_herbal_tea ) {
    scrollText(String("Herbal tea is ready."));
    playMelody();
  }

  delay(time_delay);
}

void scrollText(String text) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10, 10);
  display.clearDisplay();
  display.println(text.c_str());
  display.display();
 
  display.startscrollright(0x00, 0x0F);
  delay(1000);
  display.stopscroll();
  delay(500);
  display.startscrollleft(0x00, 0x0F);
  delay(1000);
  display.stopscroll();
  delay(500);    
}

void showMeasurement(float temperature, int timex) {
  display.clearDisplay();
  
  display.setTextSize(3);
  display.setTextColor(WHITE);

  int yline1 = 0;

  display.setCursor(5, yline1);
  display.println(temperature);

  display.setCursor(105, yline1);
  display.println("C");

  display.setTextSize(2);

  int yline3 = 50;
  display.setCursor(0, yline3);
  display.println("Time: ");

  display.setCursor(67, yline3);
  display.println(timex);

  display.setCursor(115, yline3);
  display.println("s");
  
  display.display();
}

void initOLEDDisplay() {
  // SSD1306 Init
  display.begin(SSD1306_SWITCHCAPVCC);  // Switch OLED
  display.clearDisplay();
  display.stopscroll();
  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(2000);

  // Clear the buffer.
  display.clearDisplay();
}

int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

void playMelody() {
    // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < 8; thisNote++) {

    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(BUZZER, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(BUZZER);
  }
}

// initialize serial port
void initSerial() {
  Serial.begin(BAUD_RATE);
  while (!Serial) {
    ;  // wait for serial port to initialize
  }
  if( debug ) Serial.println("Serial ready");
}

