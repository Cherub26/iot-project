#include "thingProperties.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>

// Pin Definitions
#define SW420_PIN      12   // Vibration Sensor (Interrupt)
#define TCRT5000_PIN   34   // IR Ready Sensor
#define MOTOR_PIN      18   // Vibration Motor Actuator
#define RED_PIN         2   // RGB Red
#define GREEN_PIN       4   // RGB Green
#define BLUE_PIN        5   // RGB Blue

// OLED Setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Logic Variables
volatile unsigned long startTime = 0, endTime = 0;
volatile bool hitDetected = false;
bool isWaitingForHit = false;
bool isCountingDown = false;
unsigned long targetGoTime = 0;
unsigned long handStartTime = 0;
int roundCount = 0;
long totalTime = 0;

// Interrupt Service Routine
void IRAM_ATTR handleImpact() {
  if (isWaitingForHit && !hitDetected) {
    endTime = millis();
    hitDetected = true;
  }
}

void setup() {
  Serial.begin(115200);
  
  pinMode(SW420_PIN, INPUT);
  pinMode(TCRT5000_PIN, INPUT);
  pinMode(MOTOR_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT); 
  pinMode(GREEN_PIN, OUTPUT); 
  pinMode(BLUE_PIN, OUTPUT);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED failed"));
    for(;;);
  }
  
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(10, 25);
  display.print("Initializing IoT...");
  display.display();

  initProperties();
  ArduinoCloud.begin(Arduino_ConnectionHandler);
  
  // Attach Interrupt to SW-420
  attachInterrupt(digitalPinToInterrupt(SW420_PIN), handleImpact, RISING);
}

void loop() {
  ArduinoCloud.update();
  bool handPresent = (digitalRead(TCRT5000_PIN) == LOW);

  // --- STEP 1: VISUAL FEEDBACK & READY CHECK ---
  if (!isWaitingForHit) {
    if (handPresent) {
      setRGB(0, 0, 255); // Blue: Ready
      if (gameStatus != "GET SET...") gameStatus = "[READY]";
    } else {
      setRGB(0, 0, 0); 
      if (gameStatus != "GET SET...") gameStatus = "WAITING";
      isCountingDown = false;
      triggerRound = false; // Reset cloud trigger if hand leaves
    }
  }

  // --- STEP 2: MODE & TRIGGER LOGIC ---
  if (handPresent && !isWaitingForHit && !isCountingDown) {
    if (autoMode) {
      if (handStartTime == 0) handStartTime = millis();
      if (millis() - handStartTime > 1500) startCountdown();
    } 
    else if (triggerRound) {
      startCountdown();
    }
  } else if (!handPresent) {
    handStartTime = 0;
  }

  // --- STEP 3: NON-BLOCKING COUNTDOWN ---
  if (isCountingDown && millis() >= targetGoTime) {
    triggerGoSignal();
  }

  // --- STEP 4: IMPACT PROCESSING ---
  if (isWaitingForHit && hitDetected) {
    processHit();
  }
  
  updateOLED();
}

void startCountdown() {
  isCountingDown = true;
  triggerRound = false;
  gameStatus = "GET SET...";
  targetGoTime = millis() + random(500, 3000);
}

void triggerGoSignal() {
  isCountingDown = false;
  isWaitingForHit = true;
  hitDetected = false;
  digitalWrite(MOTOR_PIN, HIGH);
  setRGB(0, 255, 0); // Green: GO!
  startTime = millis();
  delay(150); 
  digitalWrite(MOTOR_PIN, LOW);
}

void processHit() {
  isWaitingForHit = false;
  reflexTime = endTime - startTime;
  
  if (bestTime == 0 || reflexTime < bestTime) bestTime = reflexTime;
  roundCount++;
  totalTime += reflexTime;
  avgTime = totalTime / roundCount;
  
  gameStatus = "HIT!";
  setRGB(255, 255, 255); // White: Success
  delay(1000);
}

void setRGB(int r, int g, int b) {
  analogWrite(RED_PIN, r); 
  analogWrite(GREEN_PIN, g); 
  analogWrite(BLUE_PIN, b);
}

void updateOLED() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  
  // Row 1: Connection Status
  display.setCursor(0, 0);
  display.print("WiFi: Connected"); 
  display.setCursor(95, 0);
  display.print("C:OK");
  
  // Row 2: Network Info
  display.setCursor(0, 10);
  display.print("IP: "); display.print(WiFi.localIP().toString());
  display.setCursor(95, 10);
  display.print(WiFi.RSSI()); display.print("dB");
  
  // First Separator
  display.drawLine(0, 20, 128, 20, WHITE);
  
  // Row 3: Game Logic State, Round, and Mode Indicator
  display.setCursor(0, 24);
  display.print("ST: "); display.print(gameStatus);
  display.setCursor(75, 24);
  display.print("R:"); display.print(roundCount);
  display.setCursor(105, 24);
  display.print(autoMode ? "A" : "M");
  
  // Row 4: Main Result
  display.setTextSize(2);
  display.setCursor(20, 36);
  display.print(reflexTime); display.print(" ms");
  
  // Second Separator
  display.drawLine(0, 54, 128, 54, WHITE);
  
  // Row 5: Processed Analytics
  display.setTextSize(1);
  display.setCursor(0, 57);
  display.print("BEST: "); display.print(bestTime); display.print("ms");
  display.setCursor(70, 57);
  display.print("AVG: "); display.print(avgTime); display.print("ms");
  
  display.display();
}

// Cloud Callbacks
void onTriggerRoundChange() {}
void onAutoModeChange() { Serial.println("Mode Changed"); }
void onResetStatsChange() { 
  if(resetStats) { 
    bestTime = 0; avgTime = 0; totalTime = 0; roundCount = 0; resetStats = false; 
  }
}
