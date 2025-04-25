/*ilk olarak sıcaklık gelecek canlı bir şekilde,
 butona basınca nem ekrana gelecek canlı bir şekilde,
  butona 1 kere daha basınca sıcaklık grafiği,
   bir kere daha basınca nem grafiği, 1 kere daha basınca ad soyad yazacak*/
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_SHT31.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define BUTTON_PIN 4

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_SHT31 sht31 = Adafruit_SHT31();

int screenState = 0;
unsigned long lastButtonPress = 0;
float tempHistory[64];
float humHistory[64];
int historyIndex = 0;
const int redPin=12;
const int greenPin=13;

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);


  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED başlatılamadı");
    while (1);
  }

  if (!sht31.begin(0x44)) {
    Serial.println("SHT31 bulunamadı!");
    while (1);
  }

  display.clearDisplay();
}

void loop() {
  float temp = sht31.readTemperature();
  float hum = sht31.readHumidity();

  if(temp > 25.0 || hum > 75.0){
    digitalWrite(redPin, HIGH);
    digitalWrite(greenPin, LOW);
  }
  else{
    digitalWrite(greenPin, HIGH);
    digitalWrite(redPin, LOW);
  }

  tempHistory[historyIndex] = temp;
  humHistory[historyIndex] = hum;
  historyIndex = (historyIndex + 1) % 64;

  
  if (digitalRead(BUTTON_PIN) == LOW && millis() - lastButtonPress > 300) {
    screenState = (screenState + 1) % 5;
    lastButtonPress = millis();
  }

  display.clearDisplay();

  switch (screenState) {
    case 0:
      showLiveTemperature(temp);
      break;
    case 1:
      showLiveHumidity(hum);
      break;
    case 2:
      drawLiveGraph(tempHistory, "SICAKLIK");
      break;
    case 3:
      drawLiveGraph(humHistory, "NEM");
      break;
    case 4:
      showName();
      break;
  }

  display.display();
  delay(500);
}


void showLiveTemperature(float t) {
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Sicaklik:");
  display.setTextSize(2);
  display.setCursor(0, 30);
  display.print(t);
  display.println(" C");
}

void showLiveHumidity(float h) {
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Nem:");
  display.setTextSize(2);
  display.setCursor(0, 30);
  display.print(h);
  display.println(" %");
}
void drawLiveGraph(float data[], const char* label) {
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print(label);
  display.print(" (Canli)");

  display.drawLine(20, 10, 20, SCREEN_HEIGHT - 10, SSD1306_WHITE);
  for (int i = 0; i <= 100; i += 20) {
    int y = map(i, 0, 100, SCREEN_HEIGHT - 10, 10);
    display.setCursor(0, y - 3);
    display.print(i);
    display.drawPixel(20, y, SSD1306_WHITE);
  }

  display.drawLine(20, SCREEN_HEIGHT - 10, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 10, SSD1306_WHITE);
  for (int i = 0; i <= 60; i += 20) {
    int x = map(i, 0, 63, 20, SCREEN_WIDTH - 1);
    display.setCursor(x - 5, SCREEN_HEIGHT - 8);
    display.print(i);
    display.drawPixel(x, SCREEN_HEIGHT - 10, SSD1306_WHITE); 
  }

  for (int i = 1; i < 64; i++) {
    int x1 = map(i - 1, 0, 63, 20, SCREEN_WIDTH - 1);
    int y1 = map(data[i - 1], 0, 100, SCREEN_HEIGHT - 10, 10);
    int x2 = map(i, 0, 63, 20, SCREEN_WIDTH - 1);
    int y2 = map(data[i], 0, 100, SCREEN_HEIGHT - 10, 10);

    y1 = constrain(y1, 10, SCREEN_HEIGHT - 10);
    y2 = constrain(y2, 10, SCREEN_HEIGHT - 10);

    display.drawLine(x1, y1, x2, y2, SSD1306_WHITE);
  }
}

   void showName() {
    display.setTextSize(2);
    display.setCursor(0, 20);
    display.println("Elif");
    display.println("Akin");
 
  } 
 
