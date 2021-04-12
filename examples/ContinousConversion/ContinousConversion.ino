// This Example shows how to read a single channel (display its raw value and the mapping in millivolts)
#include <Ads1115Plus.h>

/// The channel of the ADS used for reading
const int adsChannel = 0;

/// The ADS instance used to read
Ads1115Plus ads;

void setup() {
    Serial.begin(9600);
    ads.begin(); // Start I2C communication
    ads.startContinousConversionMode(adsChannel);
}

void loop() {

    // Read read the raw value and millivolts from the ADS
    double millivolts = ads.getLastConversionMillivolts();
    int16_t rawValue = ads.getLastConversionResults();
    Serial.print("Ads Channel "); Serial.print(adsChannel); Serial.print(" millivolts = "); Serial.println(millivolts);
    Serial.print("Ads Channel "); Serial.print(adsChannel); Serial.print(" raw value = "); Serial.println(rawValue);

    delay(1000);
}