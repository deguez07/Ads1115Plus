// This Example shows how to read a single channel of the ADS1115 using the same interface as the Adafruit_ADS1x15 library
// To test this code you can use a potentiometer connected to channel 0
// For the preferred single channel read checkout the ReadChannel example
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

    // Read the 16bit raw value from the channel (note this value will be between 0 and 2^15)
    // For more information on how to map this value check the ReadChannel example 
    uint16_t rawValue = ads.readADC_singleEnded(adsChannel);
    Serial.print("Ads Channel "); Serial.print(adsChannel); Serial.print(" value = "); Serial.println(rawValue);

    delay(1000);
}