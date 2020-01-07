 
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define TEXT_VERTICAL 25

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define TRIGGER1 12 //D2 4
#define ECHO1 14  //D1 5
#define TRIGGER2 2 //D4
#define ECHO2 0 //D3

const uint16_t port = 8090;
const char * host = "212.235.180.207";

void setup() {
  Serial.begin (9600);
  pinMode(TRIGGER1, OUTPUT);
  pinMode(ECHO1, INPUT);
  pinMode(TRIGGER2, OUTPUT);
  pinMode(ECHO2, INPUT);
  pinMode(BUILTIN_LED, OUTPUT);

   if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  WiFi.begin("7370", "12Test12");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("...");
  }
 
  Serial.print("WiFi connected with IP: ");
  Serial.println(WiFi.localIP());
  display.display();
  delay(500);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
}

long calculate_distance(int trigger, int echo) {
  long duration, distance;
  digitalWrite(trigger, LOW);  
  delayMicroseconds(2); 
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10); 
  digitalWrite(trigger, LOW);
  
  duration = pulseIn(echo, HIGH);
  distance = (duration/2) * 0.0343;
  if (distance > 100)
  distance = 100;
  return distance;
}

int text_position(const char* text) {
 return (SCREEN_WIDTH - 12*strlen(text) - 2)/2; 
}
 
void loop() {
  long distanceRight, distanceLeft;
  WiFiClient client;
  if (!client.connect(host, port)) {
    Serial.println("Connection to host failed");
    delay(1000);
    return;
  }
  distanceLeft = calculate_distance(TRIGGER1,ECHO1);  
  distanceRight = calculate_distance(TRIGGER2,ECHO2);
  Serial.println(distanceRight);
  Serial.println(distanceLeft);
  Serial.println("RIGHT -- LEFT");
  //Pause gesture
  //Detect both hands
  if ((distanceLeft > 15 && distanceRight > 15) && (distanceLeft < 40 && distanceRight < 40)) {
//    Serial.println("Play/Pause"); 
    display.clearDisplay();
    client.print("Play/Pause");
    int x = text_position("PLAY/PAUSE");
    display.setCursor(x,TEXT_VERTICAL);
    display.println("PLAY/PAUSE");
    display.display();
    delay(1300);
  }

  distanceLeft = calculate_distance(TRIGGER1,ECHO1);  
  distanceRight = calculate_distance(TRIGGER2,ECHO2);

  //Control gesture
  //Left
  if (distanceLeft >= 14 && distanceLeft <= 30) {
    delay(500); //Hand Hold Time
    distanceLeft = calculate_distance(TRIGGER1,ECHO1);  
    if (distanceLeft >= 14 && distanceLeft <= 30) {
//      Serial.println("Left Locked");
      while(distanceLeft <= 40) {
        //Hand pushed in
        if (distanceLeft < 14) { 
//          Serial.println ("Vdown"); 
          client.print("Vdown");
          display.clearDisplay();
          int x = text_position("VOLUME -");
          display.setCursor(x,TEXT_VERTICAL);
          display.println("VOLUME -");
          display.display();
          delay(1300);
        }
        //Hand pulled out
        if (distanceLeft > 18) {
//          Serial.println ("Vup"); 
          display.clearDisplay();
          client.print("Vup");
          int x = text_position("VOLUME +");
          display.setCursor(x,TEXT_VERTICAL);
          display.println("VOLUME +");
          display.display();
          delay(1300);
        }      
        distanceLeft = calculate_distance(TRIGGER1,ECHO1);  
      }
    }
  }
  //Lock Right - Control Mode
  if (distanceRight >= 14 && distanceRight <= 28) {
    delay(500); //Hand Hold Time
    distanceRight = calculate_distance(TRIGGER2,ECHO2);
    if (distanceRight >= 14 && distanceRight <= 28) {
//      Serial.println("Right Locked");
      while(distanceRight <= 40) {
        //Right hand pushed in
        if (distanceRight < 14) {
//          Serial.println ("Rewind"); 
          display.clearDisplay();
          client.print("Rewind");
          int x = text_position("REWIND");
          display.setCursor(x,TEXT_VERTICAL);
          display.println("REWIND");
          display.display();
          delay (1300);
        }
        //Right hand pulled out
        if (distanceRight>18) {
//          Serial.println ("Forward"); 
          client.print("Forward");
          display.clearDisplay();
          int x = text_position("FORWARD");
          display.setCursor(x,TEXT_VERTICAL);
          display.println("FORWARD");
          display.display();
          delay(1300);
        }  
      distanceRight = calculate_distance(TRIGGER2,ECHO2);
      }
    }
  } 
  display.clearDisplay();
  display.display(); 
  client.stop();
  delay(500);  
}
