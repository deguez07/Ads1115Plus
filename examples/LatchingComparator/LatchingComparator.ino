#include <Ads1115Plus.h>

/// The pin of the arduino we'll use to compare the ADS reading
const int alrtPin = 2;

/// The pin of the arduino we'll use to compare the ADS reading
const int analogPin = 0;

/// The pin of the read ADS button
const int buttonPin = 3;

/// The raw value of the high threshold
const int highThresholdRaw = 10000;

/// The raw value for the low threshold
const int lowThresholdRaw = 9000;

/// Create an instance to manipulate the ADS
Ads1115Plus ads; 

void setup() {
    Serial.begin(9600);

    // Set the pins for the alrt and clear latch
    pinMode(alrtPin, INPUT);
    pinMode(buttonPin, INPUT);

    // Initialize the ADS
    ads.begin();
    ads.startComparatorMode(0, 10000, 8000, ComparatorLatchingConfig::latching, ComparatorModeConfig::traditionalComparator, ComparatorPolarityConfig::activeLow);

    Serial.print("Starting continous conversion mode: high thresh = ");
    Serial.print(ads.millivoltsPerRawValue() * highThresholdRaw);
    Serial.print("; low thresh = ");
    Serial.println(ads.millivoltsPerRawValue() * lowThresholdRaw);
}

void loop() {

    // Read the ads while the button is pressed
    bool readAds = digitalRead(buttonPin) == HIGH;
    if (readAds) {
        int16_t adc0 = ads.getLastConversionResults();
        Serial.print("ADS Channel 0: "); Serial.print(ads.millivoltsPerRawValue() * adc0); Serial.println("mV");

    }

    // Read the analog value, so we can determine when the ADS has latched
    int analogValue = analogRead(analogPin);
    double analogMillivolts = (double)analogValue * 5000.0 / 1023.0;
    Serial.print("Analog voltage: "); Serial.print(analogMillivolts); Serial.println("mV");
    Serial.print("ALRT/RDY: "); Serial.println(digitalRead(alrtPin) == HIGH ? "High" : "Low");

    delay(1000); // 1 second delay
}