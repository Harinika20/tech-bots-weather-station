
#include <Wire.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>

#define DHTPIN D4
#define DHTTYPE DHT11
#define RAINPIN A0

LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHTPIN, DHTTYPE);

const unsigned long READ_INTERVAL = 30000UL;
const unsigned long DISPLAY_INTERVAL = 3000UL;

unsigned long previousRead = 0;
unsigned long previousDisplay = 0;

float temperature = 0;
float humidity = 0;
int rainValue = 0;

bool dhtOK = false;
bool showRainScreen = false;

const char* getRainStatus(int value) {
  if (value >= 799) {
    return "NO RAIN";
  } else if (value >= 599) {
    return "LOW RAIN";
  } else {
    return "HEAVY RAIN";
  }
}

void readSensors() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  if (!isnan(t) && !isnan(h)) {
    temperature = t;
    humidity = h;
    dhtOK = true;
  } else {
    dhtOK = false;
  }

  rainValue = analogRead(RAINPIN);

  Serial.println();
  Serial.println("===== TECH BOTS WEATHER =====");

  if (dhtOK) {
    Serial.print("Temperature: ");
    Serial.print(temperature, 1);
    Serial.println(" C");

    Serial.print("Humidity: ");
    Serial.print(humidity, 0);
    Serial.println(" %");
  } else {
    Serial.println("DHT11 reading failed!");
  }

  Serial.print("Rain Sensor Value: ");
  Serial.println(rainValue);

  Serial.print("Rain Status: ");
  Serial.println(getRainStatus(rainValue));

  Serial.println("=============================");
}

void displayLCD() {
  lcd.clear();

  if (!showRainScreen) {
    lcd.setCursor(0, 0);

    if (dhtOK) {
      lcd.print("Temp:");
      lcd.print(temperature, 1);
      lcd.print((char)223);
      lcd.print("C");
    } else {
      lcd.print("DHT11 Error");
    }

    lcd.setCursor(0, 1);

    if (dhtOK) {
      lcd.print("Humidity:");
      lcd.print(humidity, 0);
      lcd.print("%");
    } else {
      lcd.print("Check Sensor");
    }
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Rain:");
    lcd.print(rainValue);

    lcd.setCursor(0, 1);
    lcd.print(getRainStatus(rainValue));
  }
}

void setup() {
  Serial.begin(9600);
  delay(500);

  Wire.begin(D2, D1);

  lcd.init();
  lcd.backlight();
  lcd.clear();

  // Startup display
  lcd.setCursor(0, 0);
  lcd.print("TECH BOTS");
  lcd.setCursor(0, 1);
  lcd.print("Weather Station");

  dht.begin();

  Serial.println();
  Serial.println("TECH BOTS WEATHER MONITOR");
  Serial.println("NodeMCU ESP8266 Started");

  delay(1500);

  readSensors();
  displayLCD();

  previousRead = millis();
  previousDisplay = millis();
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousRead >= READ_INTERVAL) {
    previousRead = currentMillis;
    readSensors();
    displayLCD();
  }

  if (currentMillis - previousDisplay >= DISPLAY_INTERVAL) {
    previousDisplay = currentMillis;
    showRainScreen = !showRainScreen;
    displayLCD();
  }
}
