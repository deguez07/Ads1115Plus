// This Example shows how to read a single channel (display its raw value and the mapping in millivolts)
#include <Ads1115Plus.h>

/// The channel of the ADS used for reading
const int adsChannel = 0;

/// The ADS instance used to read
Ads1115Plus ads;

void setup() {
    Serial.begin(9600);
    ads.begin(); // Start I2C communication
}

void loop() {

    // Read the value in millivolts from the ADS
    uint16_t millivolts = ads.readChannelMillivolts(adsChannel);
    Serial.print("Ads Channel "); Serial.print(adsChannel); Serial.print(" millivolts = "); Serial.println(millivolts);

    // Read the 16bit raw value from the channel (note this value will be between 0 and 2^15)
    uint16_t rawValue = ads.readChannelRaw(adsChannel);
    Serial.print("Ads Channel "); Serial.print(adsChannel); Serial.print(" raw value = "); Serial.println(rawValue);

    delay(1000);
}