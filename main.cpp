#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define ONE_WIRE_BUS 2
#define SCREEN_WIDTH 128    // OLED display width,  in pixels
#define SCREEN_HEIGHT 32    // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C // OLED display i2c address

#define MAX_GAME_LENGTH 20

#define LED1_PIN 2
#define LED2_PIN 3
#define LED3_PIN 4
#define LED4_PIN 5

#define BUTTON1_PIN 6
#define BUTTON2_PIN 7
#define BUTTON3_PIN 8
#define BUTTON4_PIN 9

const uint8_t buttonPins[4] = {BUTTON1_PIN, BUTTON2_PIN, BUTTON3_PIN, BUTTON4_PIN};
const uint8_t ledPins[4] = {LED1_PIN, LED2_PIN, LED3_PIN, LED4_PIN}; 

uint8_t gameSequence[MAX_GAME_LENGTH] = {0};
uint8_t gameIndex = 0;

uint8_t topScore = 0;

Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);  // Initialize the OLED display using Wire library

void HW_init(void);
void displayScore(uint8_t score);
void gameOver(void);
void blinkLEDs(void);
void playSequence(void);
byte getButtons(void);
bool checkButtonPressed(void);
void LEDsOff(void);
uint8_t buttonDebounce(uint8_t buttonPin);
void disptext(char *text);

void setup()
{
     // start serial port
  Serial.begin(9600);
  Serial.println("XAASUUSO...");
  
  HW_init();

  if (!oled.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 initialization failed!"));
    delay(2000);
  }
  oled.clearDisplay();          // clear display
  oled.setTextSize(2);          // text size
  oled.setTextColor(WHITE);     // text color
  oled.setCursor(0,10);         // position to display
  oled.print("XASUUSO!");        // text to display
  oled.display();               // show on OLED

  randomSeed(analogRead(0));

  delay(1000);
}

void loop()
{
  // LEDsOff();
  displayScore(gameIndex);  
  gameSequence[gameIndex] = random(0, 3);
  gameIndex++;

  playSequence();
  if(!checkButtonPressed())
  {
    gameOver();
    gameIndex = 0;
  }
  else
  {
    displayScore(gameIndex);
  }

  if(gameIndex > 0)
  {
    oled.clearDisplay();          // clear display
    oled.setCursor(0,10);         // position to display
    oled.print("Good Job!");        // text to display
    oled.display();               // show on OLED
  }
  delay(1000);
}

void HW_init(void)
{

  for (uint8_t i = 0; i < 4; i++)
  {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], HIGH);
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
}

void displayScore(uint8_t score)
{
  oled.clearDisplay();          // clear display
  oled.setCursor(0,10);         // position to display
  oled.print("Score: ");        // text to display
  oled.print(score);        // text to display
  oled.display();               // show on OLED
}

void gameOver(void)
{
  oled.clearDisplay();          // clear display
  oled.setCursor(0,10);         // position to display
  oled.print("GAME OVER!");        // text to display
  oled.display();               // show on OLED
  blinkLEDs();
  delay(2000);
}

void blinkLEDs(void)
{
  for(uint8_t k = 0; k < 5; k++)
  {
    for(uint8_t i = 0; i < 4; i++)
    {
      digitalWrite(ledPins[i], !digitalRead(ledPins[i]));
      delay(1);
    }
    delay(250);
  }
}

void playSequence(void)
{
  LEDsOff();
  for(uint8_t i = 0; i < gameIndex; i++)
  {
    byte currentLED = gameSequence[i];
    digitalWrite(ledPins[currentLED], LOW);
    delay(500);
    digitalWrite(ledPins[currentLED], HIGH);
    delay(500);
  }
}

byte getButtons(void)
{
  while(true)
  {
    for(uint8_t i = 0; i < 4; i++)
    {
      uint8_t currentButton = buttonPins[i];
      if(buttonDebounce(currentButton) == LOW)
      {
        return i;
      }
    }
    delay(1);
  }
}

bool checkButtonPressed(void)
{
  byte expectedButton = 0;
  byte actualButtonPressed = 0;
  for(uint8_t i = 0; i < gameIndex; i++)
  {
    expectedButton = gameSequence[i];
    actualButtonPressed = getButtons();
    digitalWrite(ledPins[actualButtonPressed], HIGH);
    delay(100);
    digitalWrite(ledPins[actualButtonPressed], LOW);
    delay(100);
    digitalWrite(ledPins[actualButtonPressed], HIGH);
    delay(1000);
    if(expectedButton != actualButtonPressed)
    {
      return false;
    }
  }
  return true;
}

void LEDsOff(void)
{
  digitalWrite(LED1_PIN, HIGH);
  digitalWrite(LED2_PIN, HIGH);
  digitalWrite(LED3_PIN, HIGH);
  digitalWrite(LED4_PIN, HIGH);
}

uint8_t buttonDebounce(uint8_t buttonPin)
{
  uint8_t buttonState = digitalRead(buttonPin);
  uint8_t lastButtonState = buttonState;
  uint8_t buttonPress = 0;
  uint8_t lastDebounceTime = 0;
  uint8_t debounceDelay = 50;

  if(buttonState != lastButtonState)
  {
    lastDebounceTime = millis();
  }

  if((millis() - lastDebounceTime) > debounceDelay)
  {
    if(buttonState != buttonPress)
    {
      buttonPress = buttonState;
    }
  }
  lastButtonState = buttonState;
  return buttonPress;
}

void disptext(char *text)
{
  oled.clearDisplay();          // clear display
  oled.setTextSize(2);          // text size
  oled.setTextColor(WHITE);     // text color
  oled.setCursor(0,10);         // position to display
  oled.print(text);        // text to display
  oled.display();               // show on OLED
}
