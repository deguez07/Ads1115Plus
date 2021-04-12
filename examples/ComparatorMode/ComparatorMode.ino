#include <Ads1115Plus.h>

/// The ads channel used for the readings
const int adsChannel = 0;

/// The pin used to detect the alrt interrupt
const int alrtPin = 2;

/// The threshold above which the comparator will assert (go LOW since comparatorPolarity = ActiveHigh)
const int highThresholdRaw = 10000; // = 1.875 V

/// The threshold below wich the comparator will de-assert (go HIGH since comparatorPolarity = ActiveHigh)
const int lowThresholdRaw = 9000; // = 1.68750 V

/// The reference to the ADS object
Ads1115Plus ads;


void setup() {
    Serial.begin(9600);

    // Initialize the interrupt
    pinMode(alrtPin, INPUT);

    // Initialize the ADS
    ads.begin();
    ads.startComparatorMode(0, highThresholdRaw, 9000); // Note these are the raw values

    // Print the voltages equivalent to the high and low thresholds
    Serial.print("High thresh = "); Serial.print(ads.millivoltsPerRawValue() * highThresholdRaw);
    Serial.print("mV ; low thresh = "); Serial.print(ads.millivoltsPerRawValue() * lowThresholdRaw); Serial.println("mV");
}

void loop() {

    // Retrieve the conversion value
    double conversionResult = ads.getLastConversionMillivolts();
    Serial.print("Channel "); Serial.print(adsChannel); Serial.print(": "); Serial.print(conversionResult); Serial.println("mV");

    // Read the state of the pin (should be LOW when above the [highThresholdRaw] and HIGH when above the [lowThresholdRaw])
    // Note the polarity of the comparator can be configured through [ads.setComparatorPolarity]
    int alrtPinState = digitalRead(alrtPin);
    Serial.print("ALRT/RDY: "); Serial.print(alrtPinState == HIGH ? "High" : "Low");

    delay(2000);

}