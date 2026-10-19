// Вариант 2: холодильная камера. Arduino Uno / Wokwi.
// s1: NTC A0 (°C); s2: LDR A1 (условные %); s3: дверь D2 (1 = открыта).
// Зуммер D8; жёлтый LED D7 через 200 Ом. Общий GND.
#include <Arduino.h>
#include <math.h>

const uint8_t PIN_NTC = A0;
const uint8_t PIN_LDR = A1;
const uint8_t PIN_DOOR = 2;
const uint8_t PIN_BUZZER = 8;
const uint8_t PIN_LED = 7;

const unsigned long T_MS = 2000;
const float THRESHOLD = 8.0;
const float BETA = 3950.0;
// В методичке для NTC допущена линейная шкала -30...30 °C.
// false: реальная модель NTC Wokwi, показания совпадают с его °C.
// true: строго учебная линейная шкала; °C на самом датчике тогда НЕ совпадут.
const bool USE_METHODICAL_LINEAR_NTC = false;

unsigned long n = 0;
unsigned long lastSampleMs = 0;
bool alarm = false;

float readTemperature() {
  const int raw = analogRead(PIN_NTC);
  if (USE_METHODICAL_LINEAR_NTC) return -30.0 + 60.0 * raw / 1023.0;
  // Защита от log(0) и деления на ноль при неисправности входа.
  if (raw <= 0 || raw >= 1023) return NAN;
  return 1.0 / (log(1.0 / (1023.0 / raw - 1.0)) / BETA
                + 1.0 / 298.15) - 273.15;
}

float readLightPercent() {
  // У Wokwi LDR при увеличении света уменьшается код АЦП.
  // Это относительный показатель 0...100 %, не люксы.
  return (1023 - analogRead(PIN_LDR)) * 100.0 / 1023.0;
}

int readDoor() {
  // SW: средний вывод 2 -> D2, левый 1 -> GND, правый 3 свободен.
  // Влево = дверь закрыта (0), вправо = открыта (1).
  return digitalRead(PIN_DOOR) == HIGH ? 1 : 0;
}

void setAlarm(bool enabled) {
  digitalWrite(PIN_LED, enabled ? HIGH : LOW);
  if (enabled) tone(PIN_BUZZER, 1000);
  else noTone(PIN_BUZZER);
}

void setup() {
  Serial.begin(9600);
  pinMode(PIN_NTC, INPUT);
  pinMode(PIN_LDR, INPUT);
  pinMode(PIN_DOOR, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  setAlarm(false);
  lastSampleMs = millis();
}

void loop() {
  // Первый опрос через T: в нормальном режиме t_s = n * T.
  // millis вместо delay уменьшает накопление задержек между опросами.
  const unsigned long now = millis();
  if (now - lastSampleMs < T_MS) return;
  lastSampleMs += T_MS;
  ++n;

  const float s1 = readTemperature();
  const float s2 = readLightPercent();
  const int s3 = readDoor();

  Serial.print("DATA n="); Serial.print(n);
  Serial.print(" s1="); Serial.print(s1, 2);
  Serial.print(" s2="); Serial.print(s2, 2);
  Serial.print(" s3="); Serial.println(s3);

  // Недостоверное значение не создаёт ложное событие NORMAL.
  // FAULT по диапазонам формирует Node-RED при загрузке журнала.
  if (!isfinite(s1)) return;
  const bool nextAlarm = s1 > THRESHOLD;
  if (nextAlarm != alarm) {
    alarm = nextAlarm;
    setAlarm(alarm);
    Serial.print("EVENT n="); Serial.print(n);
    Serial.println(alarm ? " type=THRESHOLD" : " type=NORMAL");
  }
}
