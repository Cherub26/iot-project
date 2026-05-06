#include "thingProperties.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>

// Pin Definitions
#define SW420_PIN      12   
#define TCRT5000_PIN   34   
#define MOTOR_PIN      18   
#define RED_PIN         2   
#define GREEN_PIN       4   
#define BLUE_PIN        5   

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

volatile unsigned long startTime = 0, endTime = 0;
volatile bool hitDetected = false;
bool isWaitingForHit = false;
bool isCountingDown = false;
bool isFalseStartActive = false; 
unsigned long targetGoTime = 0;
unsigned long handStartTime = 0;
int roundCount = 0;
long totalTime = 0;

void IRAM_ATTR handleImpact() {
  hitDetected = true;
  if (isWaitingForHit) {
    endTime = millis();
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(SW420_PIN, INPUT);
  pinMode(TCRT5000_PIN, INPUT);
  pinMode(MOTOR_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT); pinMode(GREEN_PIN, OUTPUT); pinMode(BLUE_PIN, OUTPUT);
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  initProperties();
  ArduinoCloud.begin(Arduino_ConnectionHandler);
  attachInterrupt(digitalPinToInterrupt(SW420_PIN), handleImpact, RISING);
}

void loop() {
  ArduinoCloud.update();
  bool handPresent = (digitalRead(TCRT5000_PIN) == LOW);

  if (isCountingDown && hitDetected) {
    processFalseStart();
  }

  if (!isWaitingForHit && !isCountingDown) {
    if (handPresent) {
      setRGB(0, 0, 255); 
      if (!isFalseStartActive) {
        gameStatus = "[READY]";
        falseStart = false;
      }
    } else {
      setRGB(0, 0, 0); 
      gameStatus = "WAITING";
      isFalseStartActive = false; 
      falseStart = false;
    }
  }

  if (handPresent && !isWaitingForHit && !isCountingDown && !isFalseStartActive) {
    if (autoMode) {
      if (handStartTime == 0) handStartTime = millis();
      if (millis() - handStartTime > 1500) startCountdown();
    } else if (triggerRound) {
      startCountdown();
    }
    hitDetected = false; 
  } else if (!handPresent) {
    handStartTime = 0;
  }

  if (isCountingDown && millis() >= targetGoTime) {
    triggerGoSignal();
  }

  if (isWaitingForHit && hitDetected) {
    processHit();
  }
  
  updateOLED();
}

void startCountdown() {
  isCountingDown = true;
  triggerRound = false;
  isFalseStartActive = false;
  falseStart = false;
  gameStatus = "GET SET...";
  setRGB(255, 255, 0); 
  targetGoTime = millis() + random(1000, 3000); 
}

void processFalseStart() {
  isCountingDown = false;
  isFalseStartActive = true; 
  falseStart = true;
  gameStatus = "FALSE START";
  setRGB(255, 0, 0); 
  digitalWrite(MOTOR_PIN, HIGH); 
  delay(1000);
  digitalWrite(MOTOR_PIN, LOW);
  hitDetected = false;
}

void triggerGoSignal() {
  isCountingDown = false;
  
  // 1. Record start time at the VERY beginning for 100% accuracy[cite: 1, 2]
  startTime = millis(); 
  
  // 2. Start the "Go!" signal
  setRGB(0, 255, 0); // Green
  digitalWrite(MOTOR_PIN, HIGH); 
  
  // 3. Buzz for 150ms[cite: 2]
  delay(150); 
  digitalWrite(MOTOR_PIN, LOW);
  
  // 4. THE INSURANCE: Clear any 'ghost hits' caused by the motor[cite: 2]
  noInterrupts();
  hitDetected = false; 
  interrupts();
  
  // 5. Now, and only now, start listening for the user[cite: 2]
  isWaitingForHit = true; 
}

void processHit() {
  isWaitingForHit = false;
  hitDetected = false;
  reflexTime = endTime - startTime; 
  
  if (bestTime == 0 || reflexTime < bestTime) bestTime = reflexTime;
  roundCount++;
  cloudRoundCount = roundCount;
  totalTime += reflexTime;
  avgTime = totalTime / roundCount;
  
  gameStatus = "HIT!";
  setRGB(255, 255, 255); 
  delay(1000);
}

void setRGB(int r, int g, int b) {
  analogWrite(RED_PIN, r); analogWrite(GREEN_PIN, g); analogWrite(BLUE_PIN, b);
}

void updateOLED() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0); 
  display.print("W:OK"); 
  display.setCursor(45, 0); 
  display.print("C:"); display.print(ArduinoCloud.connected() ? "OK" : "NO");
  display.setCursor(90, 0); 
  display.print(WiFi.RSSI()); display.print("dB");
  display.setCursor(0, 10);
  display.print("IP: "); display.print(WiFi.localIP().toString());
  display.drawLine(0, 20, 128, 20, WHITE);
  display.setCursor(0, 24); 
  display.print("S:"); display.print(gameStatus);
  display.setCursor(80, 24); 
  display.print("R:"); display.print(roundCount);
  display.setCursor(115, 24); 
  display.print(autoMode ? "A" : "M");
  display.setTextSize(2);
  display.setCursor(20, 36); 
  display.print(reflexTime); display.print(" ms");
  display.drawLine(0, 54, 128, 54, WHITE);
  display.setTextSize(1);
  display.setCursor(0, 57); 
  display.print("B:"); display.print(bestTime);
  display.setCursor(68, 57); 
  display.print("A:"); display.print(avgTime);
  display.display();
}

void onTriggerRoundChange() {}
void onAutoModeChange() {}
void onResetStatsChange() { 
  if(resetStats) { 
    bestTime = 0; avgTime = 0; totalTime = 0; roundCount = 0; 
    cloudRoundCount = 0; reflexTime = 0; resetStats = false; 
  }
}