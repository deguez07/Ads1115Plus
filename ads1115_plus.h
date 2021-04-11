#ifndef __ADS1115_PLUS_H__
#define __ADS1115_PLUS_H__

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <Wire.h>


/// The default raw difference for the low threshold, when not specified in continous conversion mode (startComparatorMode_SingleEnded)
#define DEFAULT_LOW_THRESHOLD_DIFF 5

/** Enumerates the addresses available for the ADS */
enum class AdsAddress: byte {

    /// Default address (use when ADR pin is connected to GND... or not connected at all)
    gnd = 0x48,

    /// Use when ADR pin is connected to VCC pin
    vcc = 0x49,

    /// Use when ADR address is connected to SDA pin
    sda = 0x4A,

    /// Use when ADR pin is connected to SCL pin
    scl = 0x4B
};

/** 
 * Available gains for the PGA (programmable gain amplifier) of the Ads1115 
 * The possible configurations for the PGA (bits 11:9) of the config register
 */
enum class AdsGain: uint16_t {

    /// Gain = 2/3, FSR = +/- 6.144V, Resolution / LSB size = 187.5uV 
    twoThirds = 0x0, 

    /// Gain = 1, FSR = +/- 4.096V, Resolution / LSB size = 125uV
    one = (uint16_t)0x1 << 9,

    /// Gain = 2, FSR = +/- 2.048, Resolution / LSB size = 62.5uV
    two = (uint16_t)0x2 << 9, 

    /// Gain = 4, FSR = +/- 1.024, Resolution / LSB size = 31.25uV
    four = (uint16_t)0x3 << 9, 

    /// Gain = 8, FSR = +/- 0.512, Resolution / LSB size = 15.625uV
    eight = (uint16_t)0x4 << 9, 

    /// Gain = 16, FSR = +/- 0.256, Resolution / LSB size = 7.8125uV
    sixteen = (uint16_t)0x5 << 9
};

/**
 * The configuration for the data rate samples per second (bits 7:5) of the config register
 * For more details on the delays used for reading at each sample rate see: delayForChannelReading()
 */
enum class AdsSampleSpeed: uint16_t {
    sps8 = 0x0,
    sps16 = 0x1 << 5,
    sps32 = 0x2 << 5,
    sps64 = 0x3 << 5,
    sps128 = 0x4 << 5,
    sps250 = 0x5 << 5,
    sps475 = 0x6 << 5,
    sps860 = 0x7 << 5
};

/** 
 * The possible configurations for the PGA (bits 11:9) of the config register ]
 * Added for compatibility with ADS1x15 Adafruit library
 */
typedef enum {

    /// Gain = 2/3, FSR = +/- 6.144V, Resolution / LSB size = 187.5uV 
    GAIN_TWOTHIRDS = 0x0,

    /// Gain = 1, FSR = +/- 4.096V, Resolution / LSB size = 125uV
    GAIN_ONE = 0x0200,

    /// Gain = 2, FSR = +/- 2.048, Resolution / LSB size = 62.5uV
    GAIN_TWO = 0x0400,

    /// Gain = 4, FSR = +/- 1.024, Resolution / LSB size = 31.25uV
    GAIN_FOUR = 0x0600,

    /// Gain = 8, FSR = +/- 0.512, Resolution / LSB size = 15.625uV
    GAIN_EIGHT = 0x0800,

    /// Gain = 16, FSR = +/- 0.256, Resolution / LSB size = 7.8125uV
    GAIN_SIXTEEN = 0x0A00
} adsGain_t;

/**
 * The available configurations for the comparator mode (bit 4)
 * See Figure 28 of the datasheet for more information
 */ 
enum class ComparatorModeConfig : uint16_t {

    /// Asserts the ALRT pin when value goes above the high threshold or below the low threshold
    traditionalComparator = 0x0,

    /// Asserts the ALRT each time the value crosses a threshold low or high
    windowComparator = 0x1 << 4
};

/** 
 * The configuration for the comparator polarity (bit 3) 
 * Remember to use a pull-up resistor for this to work correctly
 */
enum class ComparatorPolarityConfig : uint16_t {

    /// The ALRT pin will be grounded when asserted
    activeLow = 0x0,

    /// The ALRT pint will be left floating when asserted (it'll be high assuming there is a pull-up resistor connected)
    activeHigh = 0x1 << 3
};

/**
 * The configuration for wheater the ALERT/RDY pin should latch after being asserted (bit 2)
 * See Figure 28 of the datasheet for more information
 */
enum class ComparatorLatchingConfig : uint16_t {

    /// ALRT pin will be asserted while above the low threshold (assuming it has first gone above the high threshold)
    nonLatching = 0x0,

    /**
     * ALRT pin will be asserted when it passes the high threshold
     * It'll be kept asserted even after it goes below the low threshold
     * Assertion will be cleared when the value is read (or a SMBus alert is sent)
     * If the assertion is cleared and the value is above the high threshold, the pin will be re-asserted 
     */ 
    latching = 0x1 << 2
};

/**
 * These bits perform two functions (bits 1:0):
 * - When set to [disableAndSetHighImpedance] the comparator function is disabled
 * - For other values the comparator will assert (the ALERT/RDY pin) after the given number of values
 */
enum class ComparatorAssertConfig : uint16_t {

    /// Asserts the ALRT pin after one conversion
    assertAfterOne = 0x0,

    /// Asserts the ALRT pin after two conversions
    assertAfterTwo = 0x1,

    /// Asserts the ALRT pin after four conversions
    assertAfterFour = 0x2,

    /// Disables the ALRT pin asserting functionality
    disableAndSetHighImpedance = 0x3
};

/** 
 * The posible configurations for the mux config (bits 14:12) 
 * The mux controls the channel(s) that are read from the ADS
 * Note the ADS can read a single channel or the difference between the chanels specified
 */
enum class MuxConfig : uint16_t {

    /// Reading value = channel A0 - channel A1
    differential01 = 0x0,

    /// Reading value = channel A0 - channel A3
    differential03 = (uint16_t)0x1 << 12,

    /// Reading value = channel A1 - channel A3
    differential13 = (uint16_t)0x2 << 12,

    /// Reading value = channel A2 - channel A3
    differential23 = (uint16_t)0x3 << 12,

    /// Read channel 0
    channel0 = (uint16_t)0x4 << 12,

    /// Read channel 1
    channel1 = (uint16_t)0x5 << 12,

    /// Read channel 2
    channel2 = (uint16_t)0x6 << 12,

    /// Read channel 3
    channel3 = (uint16_t)0x7 << 12
};


/**
 * Class used to interface with the ADS1115
 * Create an instance of this class for each ADS1115 breakout / chiplet you'll read.
 * You can switch the address but it might become problematic in continous conversion mode
 * Remember you can use up to 4 ADS1115 for each i2c interface
 * 
 * It is fully compatible with Adafruit_ADS1015 library (it incorporates the same interface and allows extended functionality)
 * Hence it is possible to just change the library and keep having the same code!
 * The only drawback is that this library doesn't work with the ADS1015
 * 
 * This library incorporates a set opf improvements over the ADS1015
 * - Provides the addresses needed to use other boards on the same i2c interface
 * - Read from all differential channels (0-1, 0-3, 1-3 and 2-3)
 * - Get the readings in millivolts (for easier use and testing)
 * - Allows for faster reading times on single shot readings and provides info about the reading delays used
 * - Allows the configuration of the samples per second (great for time critial applications)
 * - Expands the continous conversion mode. You can now use differential channels
 * - Improves the comparator mode. You can now set Low Threshold, Assert Polarity, Comparator Window Mode, Assertion Latching and Comparator Queue
 */
class Ads1115Plus {

private:

    /// The current address for the ADS1115, defaults to GND - 0x48 
    byte address;

    /// The gain to be used for the readings in the adc
    uint16_t gain;

    /// The bits for the config of the data rate
    uint16_t sampleSpeed;

    /// The os config bit
    uint16_t osConfig;

    /// The mux config bits
    uint16_t muxConfig;

    /// The mode config bit
    uint16_t adsMode;

    /// The comparator mode bit
    uint16_t comparatorMode;

    /// The comparator polarity bit
    uint16_t comparatorPolarity;

    /// The comparator latching bit
    uint16_t comparatorLatching;

    /// The queue and disable bits
    uint16_t comparatorAssertConfig;

    /** The posible configurations for the OS config bit (bit 15) */
    enum class OsConfig: uint16_t {
        noEffect = 0x0, // write
        performingConversion = 0x0, // read
        startSingleConversion = (uint16_t)0x1 << 15, // write
        notPerformingConversion = (uint16_t)0x1 << 15// read
    };

    /** The configuration for the reading mode config (bit 8) */
    enum class AdsModeConfig: uint16_t {

        /// Allows the ADS to work in continous conversion 
        continuousConversion = 0x0,

        /// Use when performing single reads from the ADS
        singleShotConversion = 0x1 << 8
    };


    /**
     * Available address pointer values
     * The address pointer determines which registers will be read / written
     */
    enum class AddressPointerReg: byte {

        /// Contains the results of the last conversion
        conversionRegister = 0x0,

        /// Used to change the configuration of the ADS
        configRegister = 0x1,

        /// 16-bit register used as the low threshold in comparator mode
        lowThresholdRegister = 0x2,

        /// 16-bit register used as the high threshold in comparator mode
        highThresholdRegister = 0x3
    };

    /// Returns the config register based on the current configuration / state
    uint16_t buildConfigRegister();

    /** 
     * Performs a single shot reading to the ADS1115 using the current config
     * Make sure the [modeConfig] has been set to [singleShot]
     */
    int16_t currentConfigSingleShotRead();

    /// Returns the mux config of the given [channel]
    uint16_t muxConfigOfSingleChannel(byte channel);

    /** Writes the [currentConfigRegister] to the ads */ 
    void writeCurrentConfig();

    /// Reads a byte using a legacy supported implementation of i2c
    static byte i2cReadByte();

    /// Writes the given [value] through i2c (its legacy supported)
    static void i2cWriteByte(byte value);

    /**
     * Writes the given [value] to the Ads
     * @param i2cAddress The address of the ADS for which the value is written
     * @param reg The [AddressPointer] register to which the value will be writen
     * @param value the data to be written
     */
    static void writeToAds(byte i2cAddress, byte reg, uint16_t value);

    /**
     * Reads two bytes from the Ads (a uint16), with the given [i2cAddress]
     * @param i2cAddress The address of the Ads from which the data will be read
     * @param reg The [AddressPointer] register from which data will be read
     * @return The two bytes read from the Ads as a uint16
     */
    static uint16_t readFromAds(byte i2cAddress, byte reg);

public:

    /**
     *  Creates a new Ads1115Plus instance, with the following defaults
     * - address = AdsAddres::gnd
     * - gain = AdsGain::twoThirds +/- 6.144v
     * - dataRate = AdsDataRate::sps64 (64 samples per second, which is sufficiently fast and stable)
     */
    Ads1115Plus(AdsAddress address = AdsAddress::gnd, AdsGain gain = AdsGain::twoThirds, AdsSampleSpeed dataRate = AdsSampleSpeed::sps64);

    /** 
     * Starts i2c communication, equivalent to calling Wire.begin() 
     * If using multiple i2c devices, opt for Wire.begin() instead
     */
    void begin();

    // MARK: Channel reading

    /** 
     * Same as calling readChannelBits(channel)
     * @deprecated use readChannelBits(channel) instead
     */
    uint16_t readADC_singleEnded(byte channel);

    /**
     * Reads the given [channel] from the ADS1115 with the current gain
     * @param channel The channel to be read from the ads (0 to 3)
     * @return The value of the given [channel] raw (note max value is 2^15)
     */
    uint16_t readChannelRaw(byte channel);

    /**
     * Reads the given [channel] from the ADS1115 with the current gain
     * @param channel The channel to be read from the ads (0 to 3)
     * @return the value of the given [channel] in millivolts
     */
    double readChannelMillivolts(byte channel);

    /** 
     * Reads the ADS1115 in differetial mode between channels 0 and 1 
     * @deprecated use readDifferentialRaws01 instead
     * @return (ADS channel 0 - ADS channel 1) * gain
     */
    int16_t readADC_Differential_0_1();

    /**
     * Reads the ADS1115 in differetial mode between channels 2 and 3
     * @deprecated use readDifferentialRaw23 instead
     * @return (ADS channel 2 - ADS channel 3) * gain
     */
    int16_t readADC_Differential_2_3();

    /**
     * Reads the ADS1115 in differential mode between channels 0 and 1
     * @return (ADS channel 0 - ADS channel 1) * gain (raw value)
     */
    int16_t readDifferentialRaw01();

    /**
     * Reads the ADS1115 in differential mode between channels 0 and 3
     * @return (ADS channel 0 - ADS channel 3) * gain (raw value)
     */
    int16_t readDifferentialRaw03();
    
    /**
     * Reads the ADS1115 in differential mode between channels 1 and 3
     * @return (ADS channel 01- ADS channel 3) * gain (raw value)
     */
    int16_t readDifferentialRaw13();

    /**
     * Reads the ADS1115 in differential mode between channels 2 and 3
     * @return (ADS channel 2 - ADS channel 3) * gain (raw value)
     */
    int16_t readDifferentialRaw23();

    /**
     * Reads the ADS1115 in differential mode between channels 0 and 1
     * @return (ADS channel 0 - ADS channel 1) * gain (in millivolts)
     */
    double readDifferentialMillivolts01();

    /**
     * Reads the ADS1115 in differential mode between channels 0 and 3
     * @return (ADS channel 0 - ADS channel 3) * gain (in millivolts)
     */
    double readDifferentialMillivolts03();

    /**
     * Reads the ADS1115 in differential mode between channels 1 and 3
     * @return (ADS channel 1 - ADS channel 3) * gain (in millivolts)
     */
    double readDifferentialMillivolts13();

    /**
     * Reads the ADS1115 in differential mode between channels 2 and 3
     * @return (ADS channel 2 - ADS channel 3) * gain (in millivolts)
     */
    double readDifferentialMillivolts23();

    /**
     * Performs a single shot reading on the given [mux] channel 
     * Note the value can only be negative when reading a differential channel
     * @param mux The channel (single or differential) to be read from the ADS
     * @return The raw value read from the ADS 
     */
    int16_t readRawOnMux(MuxConfig mux);

    /**
     * Performs a single shot reading on the given [mux] channel
     * Note the value can only be negative when reading a differential channel
     * The current [gain] is used to tranform the raw value into millivolts
     * @param mux The channel (single or differential) to be read from the ADS
     * @return The value read from the ADS in millivolts
     */
    double readMillivoltsOnMux(MuxConfig mux);

    // MARK: Comparator mode

    /**
     * Starts the comparator mode using the given [channel], with the given raw [highThreshold] and raw [lowThreshold]; using the following defaults
     * - Low threshold: highThreshold - [DEFAULT_LOW_THRESHOLD_DIFF]
     * - Comparator latching: false
     * - Comparator mode: traditional comparator
     * - Comparator polarity: active low
     * - Comparator queue: assert after 1 conversion
     * @deprecated use startContinousConversionMode instead
     */
    void startComparator_SingleEnded(byte channel, uint16_t highThreshold, ComparatorLatchingConfig comparatorLatching = ComparatorLatchingConfig::nonLatching, ComparatorModeConfig comparatorMode = ComparatorModeConfig::traditionalComparator, ComparatorPolarityConfig comparatorPolarity = ComparatorPolarityConfig::activeLow, ComparatorAssertConfig comparatorQueue = ComparatorAssertConfig::assertAfterOne);

    /**
     * Starts the comparator mode using the given [mux], with the given raw [highThreshold] and raw [lowThreshold]; using the following defaults
     * - Comparator latching: false
     * - Comparator mode: traditional comparator
     * - Comparator polarity: active low
     * - Comparator queue: assert after 1 conversion
     */
    void startComparatorModeOnMux(MuxConfig mux, uint16_t highThreshold, uint16_t lowThreshold, ComparatorLatchingConfig comparatorLatching = ComparatorLatchingConfig::nonLatching, ComparatorModeConfig comparatorMode = ComparatorModeConfig::traditionalComparator, ComparatorPolarityConfig comparatorPolarity = ComparatorPolarityConfig::activeLow, ComparatorAssertConfig comparatorQueue = ComparatorAssertConfig::assertAfterOne);

    /** 
     * Starts the comparator mode using the given [channel], with the given raw [highThreshold] and raw [lowThreshold]; using the following defaults
     * - Comparator latching: false
     * - Comparator mode: traditional comparator
     * - Comparator polarity: active low
     * - Comparator queue: assert after 1 conversion
     */
    void startComparatorMode(byte channel, uint16_t highThreshold, uint16_t lowThreshold, ComparatorLatchingConfig comparatorLatching = ComparatorLatchingConfig::nonLatching, ComparatorModeConfig comparatorMode = ComparatorModeConfig::traditionalComparator, ComparatorPolarityConfig comparatorPolarity = ComparatorPolarityConfig::activeLow, ComparatorAssertConfig comparatorQueue = ComparatorAssertConfig::assertAfterOne);

    /**
     * Starts the comparator mode for differential channel 0-1, with the given raw [highThreshold] and raw [lowThreshold]; using the following defaults
     * - Comparator latching: false
     * - Comparator mode: traditional comparator
     * - Comparator polarity: active low
     * - Comparator queue: assert after 1 conversion
     */
    void startComparatorMode01(uint16_t highThreshold, uint16_t lowThreshold, ComparatorLatchingConfig comparatorLatching = ComparatorLatchingConfig::nonLatching, ComparatorModeConfig comparatorMode = ComparatorModeConfig::traditionalComparator, ComparatorPolarityConfig comparatorPolarity = ComparatorPolarityConfig::activeLow, ComparatorAssertConfig comparatorQueue = ComparatorAssertConfig::assertAfterOne);

    /**
     * Starts the comparator mode for differential channel 0-3, with the given raw [highThreshold] and raw [lowThreshold]; using the following defaults
     * - Comparator latching: false
     * - Comparator mode: traditional comparator
     * - Comparator polarity: active low
     * - Comparator queue: assert after 1 conversion
     */
    void startComparatorMode03(uint16_t highThreshold, uint16_t lowThreshold, ComparatorLatchingConfig comparatorLatching = ComparatorLatchingConfig::nonLatching, ComparatorModeConfig comparatorMode = ComparatorModeConfig::traditionalComparator, ComparatorPolarityConfig comparatorPolarity = ComparatorPolarityConfig::activeLow, ComparatorAssertConfig comparatorQueue = ComparatorAssertConfig::assertAfterOne);

    /**
     * Starts the comparator mode for differential channel 1-3, with the given raw [highThreshold] and raw [lowThreshold]; using the following defaults
     * - Comparator latching: false
     * - Comparator mode: traditional comparator
     * - Comparator polarity: active low
     * - Comparator queue: assert after 1 conversion
     */
    void startComparatorMode13(uint16_t highThreshold, uint16_t lowThreshold, ComparatorLatchingConfig comparatorLatching = ComparatorLatchingConfig::nonLatching, ComparatorModeConfig comparatorMode = ComparatorModeConfig::traditionalComparator, ComparatorPolarityConfig comparatorPolarity = ComparatorPolarityConfig::activeLow, ComparatorAssertConfig comparatorQueue = ComparatorAssertConfig::assertAfterOne);

    /**
     * Starts the comparator mode for differential channel 2-3, with the given raw [highThreshold] and raw [lowThreshold]; using the following defaults
     * - Comparator latching: false
     * - Comparator mode: traditional comparator
     * - Comparator polarity: active low
     * - Comparator queue: assert after 1 conversion
     */
    void startComparatorMode23(uint16_t highThreshold, uint16_t lowThreshold, ComparatorLatchingConfig comparatorLatching = ComparatorLatchingConfig::nonLatching, ComparatorModeConfig comparatorMode = ComparatorModeConfig::traditionalComparator, ComparatorPolarityConfig comparatorPolarity = ComparatorPolarityConfig::activeLow, ComparatorAssertConfig comparatorQueue = ComparatorAssertConfig::assertAfterOne);

    /**
     * Starts the continous conversion mode on the given channel
     * This method disables the comparator assert
     * @param channel The ADS channel (0 to 3) for which the conversion will be read
     */
    void startContinousConversionMode(byte channel);


    /**
     * Starts the continous conversion mode on differential channel 0 - 1
     * This method disables the comparator assert
     */
    void startContinousConversionMode01();

    /**
     * Starts the continous conversion mode on differential channel 0 - 3
     * This method disables the comparator assert
     */
    void startContinousConversionMode03();

    /**
     * Starts the continous conversion mode on differential channel 1 - 3
     * This method disables the comparator assert
     */
    void startContinousConversionMode13();

    /**
     * Starts the continous conversion mode on differential channel 2 - 3
     * This method disables the comparator assert
     */
    void startContinousConversionMode23();

    /**
     * Starts the continous conversion mode on the given mux channel
     * This method disables the comparator assert
     * @param mux the mux channel for which the continuous conversion mode should begin (0, 1, 2, 3, 01, 03, 13 or 23)
     */
    void startContinousConversionModeOnMux(MuxConfig mux);
    

    /** 
     * Returns the raw value of the last conversion (use this when using continuous conversion mode) 
     * Note that the value may be negative if the continous conversion has been set in a differential channel
     */
    int16_t getLastConversionResults();

    /** 
     * Returns the result of the last conversion in millivolts (using the last configured gain) 
     * Note that the value may be negative if the continous conversion has been set in a differential channel
     */
    double getLastConversionMillivolts();

    // MARK: Comparator getter and setters

    /**
     * Sets the comparator latching mode configuration... Whether the ALRT pin should stay latched after an assertion
     * @param comparatorLatching The latching config for the Ads
     * @param updateConfig if true updates the ADS configuration when running in continous conversion mode (true by default)
     */
    void setComparatorLatching(ComparatorLatchingConfig comparatorLatching, bool updateConfig = true);

    /// Returns the comparator latching mode configuration
    ComparatorLatchingConfig getComparatorLatching();

    /**
     * Sets the comparator mode... The mechanism of how is the ALRT pin asserted
     * @param comparatorMode The mode of the comparator window or traditional
     * @param updateConfig if true updates the ADS configuration when running in continous conversion mode (true by default)
     */
    void setComparatorMode(ComparatorModeConfig comparatorMode, bool updateConfig = true);

    /// Returns the comparator mode configuration
    ComparatorModeConfig getComparatorMode(); 

    /**
     * Sets the polarity of the ALRT pin when asserted
     * @param comparatorPolarity Active low or active high (remeber a pull up is required)
     * @param updateConfig if true updates the ADS configuration when running in continous conversion mode (true by default)
     */
    void setComparatorPolarity(ComparatorPolarityConfig comparatorPolarity, bool updateConfig = true);

    /// Returns the polarity of the comparator
    ComparatorPolarityConfig getComparatorPolarity();

    /**
     * Sets the comparator queue configuration. When the ALRT pin should assert (if it should assert)
     * @param comparatorQueue Assert the alert pin after 1, 2 or 4 readings (can be also set so it doesn't assert at all)
     * @param updateConfig if true updates the ADS configuration when running in continous conversion mode (true by default)
     */
    void setComparatorAssert(ComparatorAssertConfig comparatorQueue, bool updateConfig = true);

    /// Returns the current configuration for the ALRT pin assertion
    ComparatorAssertConfig getComparatorAssert();

    /// Clears the comparator latch, if it has been set to latch
    void clearComparatorLatch();

    // MARK: Config setters and getters

    /**
     * Sets the gain to be used for readings
     * @param gain The PGA gain 2/3, 1, 2, 4 or 8
     * @param updateConfig if true updates the ADS configuration when running in continous conversion mode (true by default)
     */
    void setGain(AdsGain gain, bool updateConfig = true);

    /// Returns the gain currently used for readings
    AdsGain getGain();

    /**
     * Sets the amount of samples per second the ads should perform
     * Note more samples per second results in lower single shot readings delays, but it might add noise
     * See datasheet table 1 for more information of readings noise
     * Retrieve the delays used for readings from the delayForChannelReading() method
     * @param gain The PGA gain 2/3, 1, 2, 4 or 8
     * @param updateConfig if true updates the ADS configuration when running in continous conversion mode (true by default)
     */
    void setSampleSpeed(AdsSampleSpeed speed, bool updateConfig = true);

    /// Returns the sample speed currently used for readings
    AdsSampleSpeed getSampleSpeed();

    // MARK: Utility methods

    /**
     * The millivolts / bit for the current gain config (used to determine the voltage)
     * Note this is also known as the device resolution for the current gain config
     * @returns mv / rawValue factor
     */
    double millivoltsPerRawValue();

    /** 
     * The millivolts / bit for the given [gain]   
     * Note this is also known as the device resolution for the given [gain]
     * @param gain The gain for which the mv/rawValue factor is retrievd
     * @returns mv / rawValue factor
     */
    double millivoltsPerRawValue(AdsGain gain);

    /** Returns the delay in ms, for the current single shot channel reading */
    unsigned long delayForChannelReading();

    /// Transforms the given [rawValue] into millivolts using the current gain config
    double rawValueToMillivolts(int16_t rawValue);

    // Transforms the given [rawValue] into millivolts using the given [gain]
    double rawValueToMillivolts(int16_t rawValue, AdsGain gain);

    /// Transforms the given millivolts into a raw ADS value, using the current gain config (note the result is rounded to the nearest int)
    double millivoltsToRawValue(double millivolts);

    /// Transforms the given millivolts into a raw ADS value, using the given [gain] (note the result is rounded to the nearest int)
    double millivoltsToRawValue(double millivolts, AdsGain gain);
};


#endif