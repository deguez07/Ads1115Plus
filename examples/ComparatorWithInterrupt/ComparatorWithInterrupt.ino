/// This example shows how to use the comparator mode with an interrupt
/// When the pin is asserted the [hasAsserted] flag changes value (calls the interrupt routine)
/// If you need any other code to run in the routine, simply add the code within the interrupt routine (keep in mind interrupt routines need to be executed)
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

/// Flag used to determine if the comparator has asserted
/// Needs to be volatile since it is used in an interrupt
volatile bool hasAsserted = false;

/// Interrupt routing called each time the alrt pin asserts
void alertPinChanged() {
    hasAsserted = true;
}

void setup() {
    Serial.begin(9600);

    // Initialize the interrupt
    pinMode(alrtPin, INPUT);
    attachInterrupt(digitalPinToInterrupt(alrtPin), alertPinChanged, CHANGE);

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

    // Create a cummulative 2 second delay, that immediately reacts to the [hasAsserted flag]
    for (int i = 0; i < 2000; i++) {
        if (hasAsserted) {
            hasAsserted = false;
            Serial.print("ALRT/RDY: "); Serial.print(digitalRead(alrtPin) == HIGH ? "High" : "Low");
            double conversionResult = ads.getLastConversionMillivolts();
            Serial.print(" – "); Serial.print(conversionResult); Serial.println("mV");
        }
        delay(1);
    }

}