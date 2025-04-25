/*bardağın 3/4'ü dolu olacak şekilde plastik bardakta toprak, şişe su, 
Toprak nem ölçüm sensörü, seven segment display ve 4 adet transistörden oluşan 
Toprak nemi gösterme devresi. uygun olan devreyi esp 32 ile kurun. 
 Transistörsüz sürme devrelerini kullanmayın çünkü esp 32'nizi yakabilir
 Devrenizi denerken sensörün elektronik devrelerini suya değdirmemek şartıyla 
 su içine soktuğunuzda nem değeri yükselecek kuru ortama aldığınızda nem değeri sıfır olacak şekilde seven segment display'de gösterin*/
// Segment pinleri
const int segA = 13;
const int segB = 12;
const int segC = 14;
const int segD = 27;
const int segE = 26;
const int segF = 25;
const int segG = 33;
const int dpPin = 32;  // Decimal point

const int segmentPins[7] = {segA, segB, segC, segD, segE, segF, segG};
const int digitPins[4] = {15, 2, 4, 5}; // 4 hane

// Toprak nem sensörü pini
const int sensorPin = 34;

// Sayılar için segment kombinasyonları
const byte digits[10][7] = {
  {1,1,1,1,1,1,0}, // 0
  {0,1,1,0,0,0,0}, // 1
  {1,1,0,1,1,0,1}, // 2
  {1,1,1,1,0,0,1}, // 3
  {0,1,1,0,0,1,1}, // 4
  {1,0,1,1,0,1,1}, // 5
  {1,0,1,1,1,1,1}, // 6
  {1,1,1,0,0,0,0}, // 7
  {1,1,1,1,1,1,1}, // 8
  {1,1,1,1,0,1,1}  // 9
};

// Ortalama hesaplama için
const int numReadings = 5;
int readings[numReadings];
int readIndex = 0;
int total = 0;
int average = 0;

void setup() {
  Serial.begin(115200);

  for (int i = 0; i < 7; i++) {
    pinMode(segmentPins[i], OUTPUT);
    digitalWrite(segmentPins[i], LOW);
  }

  for (int i = 0; i < 4; i++) {
    pinMode(digitPins[i], OUTPUT);
    digitalWrite(digitPins[i], LOW);
  }

  pinMode(dpPin, OUTPUT);
  digitalWrite(dpPin, LOW);
  pinMode(sensorPin, INPUT);

  for (int i = 0; i < numReadings; i++) readings[i] = 0;
}

void loop() {
  // Ortalama al
  total -= readings[readIndex];
  readings[readIndex] = analogRead(sensorPin);
  total += readings[readIndex];
  readIndex = (readIndex + 1) % numReadings;
  average = total / numReadings;

  float percent = map(average, 4095, 1500, 0, 1000) / 10.0;
  if (percent < 0) percent = 0;
  if (percent > 100) percent = 100;
  if (percent < 70) percent = 0.0;

  Serial.print("Nem: ");
  Serial.println(percent, 2);

  for (int i = 0; i < 20; i++) {
    displayFormattedPercent(percent);
  }
}

void displayFormattedPercent(float value) {
  int intVal = (int)(value * 100); // 78.90 → 7890

  int digitsToDisplay[4];
  bool dotPositions[4] = {false, false, false, false};

  if (value < 100.0) {
    digitsToDisplay[0] = (intVal / 1000) % 10;
    digitsToDisplay[1] = (intVal / 100) % 10;
    digitsToDisplay[2] = (intVal / 10) % 10;
    digitsToDisplay[3] = intVal % 10;
    dotPositions[1] = true; // 00.00 gibi görünmesi için 2. haneden sonra nokta
  } else {
    digitsToDisplay[0] = 1;
    digitsToDisplay[1] = 0;
    digitsToDisplay[2] = 0;
    digitsToDisplay[3] = 0;
    dotPositions[2] = true; // 100.0 için 3. haneden sonra nokta
  }

  for (int i = 0; i < 4; i++) {
    setSegments(digitsToDisplay[i]);
    digitalWrite(digitPins[i], HIGH);
    digitalWrite(dpPin, dotPositions[i] ? HIGH : LOW);
    delay(5);
    digitalWrite(digitPins[i], LOW);
    digitalWrite(dpPin, LOW);
  }
}

void setSegments(int num) {
  for (int i = 0; i < 7; i++) {
    digitalWrite(segmentPins[i], digits[num][i]);
  }
}