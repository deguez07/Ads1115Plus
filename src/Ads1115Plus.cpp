// Copyright(C) 2021 by Diego Eguez

// Permission is hereby granted, free of charge, to any person obtaining a copy of this softwareand associated documentation files(the "Software"), 
// to deal in the Software without restriction, including without l > imitation the rights to use, copy, modify, merge, publish, distribute, 
// sublicense, and /or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions :

// The above copyright noticeand this permission notice shall be included in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 


#include "Ads1115Plus.h"


byte Ads1115Plus::i2cReadByte() {
#if ARDUINO >= 100
    return Wire.read();
#else
    return Wire.receive();
#endif
}


void Ads1115Plus::i2cWriteByte(byte x) {
#if ARDUINO >= 100
    Wire.write((byte)x);
#else
    Wire.send(x);
#endif
}


void Ads1115Plus::writeToAds(byte i2cAddress, byte reg, uint16_t value) {
    Wire.beginTransmission(i2cAddress);
    i2cWriteByte((byte)reg);
    i2cWriteByte((byte)(value >> 8));
    i2cWriteByte((byte)(value & 0xFF));
    Wire.endTransmission();
}


uint16_t Ads1115Plus::readFromAds(byte i2cAddress, byte reg) {
    Wire.beginTransmission(i2cAddress);
    i2cWriteByte(reg);
    Wire.endTransmission();
    Wire.requestFrom(i2cAddress, (byte)2);
    return ((i2cReadByte() << 8) | i2cReadByte());
}


Ads1115Plus::Ads1115Plus(AdsAddress address, AdsGain gain, AdsSampleSpeed dataRate) {
    this->address = (byte)address;
    this->gain = (uint16_t)gain;
    this->sampleSpeed = (uint16_t)dataRate;

    // Set up the default config register values
    comparatorAssertConfig = (uint16_t) ComparatorAssertConfig::disableAndSetHighImpedance;
    comparatorLatching = (uint16_t) ComparatorLatchingConfig::nonLatching;
    comparatorPolarity = (uint16_t) ComparatorPolarityConfig::activeLow;
    comparatorMode = (uint16_t) ComparatorModeConfig::traditionalComparator;
    adsMode = (uint16_t)AdsModeConfig::singleShotConversion;
    muxConfig = (uint16_t) MuxConfig::channel0;
    osConfig = (uint16_t)OsConfig::startSingleConversion;

}

void Ads1115Plus::begin() {
    Wire.begin();
}

// MARK: Config getter and setters

AdsGain Ads1115Plus::getGain() {
    return (AdsGain)gain;
}

void Ads1115Plus::setGain(AdsGain gain, bool updateConfig) {
    this->gain = (uint16_t)gain;
    if (updateConfig && (AdsModeConfig)adsMode == AdsModeConfig::continuousConversion) {
        writeCurrentConfig();
    }
}

AdsSampleSpeed Ads1115Plus::getSampleSpeed() {
    return (AdsSampleSpeed)sampleSpeed;
}

void Ads1115Plus::setSampleSpeed(AdsSampleSpeed speed, bool updateConfig) {
    sampleSpeed = (uint16_t)speed;
    if (updateConfig && (AdsModeConfig)adsMode == AdsModeConfig::continuousConversion) {
        writeCurrentConfig();
    }
}

void Ads1115Plus::setComparatorLatching(ComparatorLatchingConfig comparatorLatching, bool updateConfig) {
    this->comparatorLatching = (uint16_t)comparatorLatching;
    if (updateConfig && (AdsModeConfig)adsMode == AdsModeConfig::continuousConversion) {
        writeCurrentConfig();
    }
}

ComparatorLatchingConfig Ads1115Plus::getComparatorLatching() {
    return (ComparatorLatchingConfig)comparatorLatching;
}

void Ads1115Plus::setComparatorMode(ComparatorModeConfig comparatorMode, bool updateConfig) {
    this->comparatorMode = (uint16_t)comparatorMode;
    if (updateConfig && (AdsModeConfig)adsMode == AdsModeConfig::continuousConversion) {
        writeCurrentConfig();
    }
}

ComparatorModeConfig Ads1115Plus::getComparatorMode() {
    return (ComparatorModeConfig)comparatorMode;
}

void Ads1115Plus::setComparatorPolarity(ComparatorPolarityConfig comparatorPolarity, bool updateConfig) {
    this->comparatorPolarity = (uint16_t)comparatorPolarity;
    if (updateConfig && (AdsModeConfig)adsMode == AdsModeConfig::continuousConversion) {
        writeCurrentConfig();
    }
}

ComparatorPolarityConfig Ads1115Plus::getComparatorPolarity() {
    return (ComparatorPolarityConfig)comparatorPolarity;
    
}

void Ads1115Plus::setComparatorAssert(ComparatorAssertConfig comparatorQueue, bool updateConfig) {
    this->comparatorAssertConfig = (uint16_t)comparatorQueue;
    if (updateConfig && (AdsModeConfig)adsMode == AdsModeConfig::continuousConversion) {
        writeCurrentConfig();
    }
}

ComparatorAssertConfig Ads1115Plus::getComparatorAssert() {
    return (ComparatorAssertConfig) comparatorAssertConfig;
}

// MARK: Continous conversion mode

void Ads1115Plus::startComparator_SingleEnded(byte channel, uint16_t highThreshold, ComparatorLatchingConfig comparatorLatching, ComparatorModeConfig comparatorMode, ComparatorPolarityConfig comparatorPolarity, ComparatorAssertConfig comparatorQueue) {
    uint16_t lowThreshold = highThreshold > DEFAULT_LOW_THRESHOLD_DIFF ? highThreshold - DEFAULT_LOW_THRESHOLD_DIFF : 0;
    startComparatorMode(channel, highThreshold, lowThreshold, comparatorLatching, comparatorMode, comparatorPolarity, comparatorQueue);
}

void Ads1115Plus::startComparatorMode(byte channel, uint16_t highThreshold, uint16_t lowThreshold, ComparatorLatchingConfig comparatorLatching, ComparatorModeConfig comparatorMode, ComparatorPolarityConfig comparatorPolarity, ComparatorAssertConfig comparatorQueue) {
    uint16_t mux = muxConfigOfSingleChannel(channel);
    startComparatorModeOnMux((MuxConfig)mux, highThreshold, lowThreshold, comparatorLatching, comparatorMode, comparatorPolarity, comparatorQueue);
}

void Ads1115Plus::startComparatorMode01(uint16_t highThreshold, uint16_t lowThreshold, ComparatorLatchingConfig comparatorLatching = ComparatorLatchingConfig::nonLatching, ComparatorModeConfig comparatorMode = ComparatorModeConfig::traditionalComparator, ComparatorPolarityConfig comparatorPolarity = ComparatorPolarityConfig::activeLow, ComparatorAssertConfig comparatorQueue = ComparatorAssertConfig::assertAfterOne) {
    startComparatorModeOnMux(MuxConfig::differential01, highThreshold, lowThreshold, comparatorLatching, comparatorMode, comparatorPolarity, comparatorQueue);
}

void Ads1115Plus::startComparatorMode03(uint16_t highThreshold, uint16_t lowThreshold, ComparatorLatchingConfig comparatorLatching = ComparatorLatchingConfig::nonLatching, ComparatorModeConfig comparatorMode = ComparatorModeConfig::traditionalComparator, ComparatorPolarityConfig comparatorPolarity = ComparatorPolarityConfig::activeLow, ComparatorAssertConfig comparatorQueue = ComparatorAssertConfig::assertAfterOne) {
    startComparatorModeOnMux(MuxConfig::differential03, highThreshold, lowThreshold, comparatorLatching, comparatorMode, comparatorPolarity, comparatorQueue);
}

void Ads1115Plus::startComparatorMode13(uint16_t highThreshold, uint16_t lowThreshold, ComparatorLatchingConfig comparatorLatching = ComparatorLatchingConfig::nonLatching, ComparatorModeConfig comparatorMode = ComparatorModeConfig::traditionalComparator, ComparatorPolarityConfig comparatorPolarity = ComparatorPolarityConfig::activeLow, ComparatorAssertConfig comparatorQueue = ComparatorAssertConfig::assertAfterOne) {
    startComparatorModeOnMux(MuxConfig::differential13, highThreshold, lowThreshold, comparatorLatching, comparatorMode, comparatorPolarity, comparatorQueue);
}

void Ads1115Plus::startComparatorMode23(uint16_t highThreshold, uint16_t lowThreshold, ComparatorLatchingConfig comparatorLatching = ComparatorLatchingConfig::nonLatching, ComparatorModeConfig comparatorMode = ComparatorModeConfig::traditionalComparator, ComparatorPolarityConfig comparatorPolarity = ComparatorPolarityConfig::activeLow, ComparatorAssertConfig comparatorQueue = ComparatorAssertConfig::assertAfterOne) {
    startComparatorModeOnMux(MuxConfig::differential23, highThreshold, lowThreshold, comparatorLatching, comparatorMode, comparatorPolarity, comparatorQueue);
}

void Ads1115Plus::startComparatorModeOnMux(MuxConfig mux, uint16_t highThreshold, uint16_t lowThreshold, ComparatorLatchingConfig comparatorLatching, ComparatorModeConfig comparatorMode, ComparatorPolarityConfig comparatorPolarity, ComparatorAssertConfig comparatorQueue) {
    muxConfig = (uint16_t)mux;
    comparatorAssertConfig = (uint16_t)comparatorQueue;
    this->comparatorLatching = (uint16_t)comparatorLatching;
    this->comparatorMode = (uint16_t)comparatorMode;
    this->comparatorPolarity = (uint16_t)comparatorPolarity;
    adsMode = (uint16_t)AdsModeConfig::continuousConversion;

    // Write the high and low thresholds to the ADS
    writeToAds(address, (byte)AddressPointerReg::highThresholdRegister, highThreshold);
    writeToAds(address, (byte)AddressPointerReg::lowThresholdRegister, lowThreshold);

    // Write the new configuration
    writeCurrentConfig();
}

void Ads1115Plus::startContinousConversionMode(byte channel) {
    MuxConfig mux = (MuxConfig)muxConfigOfSingleChannel(channel);
    startContinousConversionModeOnMux(mux);
}

void Ads1115Plus::startContinousConversionMode01() {
    startContinousConversionModeOnMux(MuxConfig::differential01);
}

void Ads1115Plus::startContinousConversionMode03() {
    startContinousConversionModeOnMux(MuxConfig::differential03);
}

void Ads1115Plus::startContinousConversionMode13() {
    startContinousConversionModeOnMux(MuxConfig::differential13);
}

void Ads1115Plus::startContinousConversionMode23() {
    startContinousConversionModeOnMux(MuxConfig::differential23);
}

void Ads1115Plus::startContinousConversionModeOnMux(MuxConfig mux) {

    // set the mux and disable the config
    muxConfig = (uint16_t)mux;
    comparatorAssertConfig = (uint16_t)ComparatorAssertConfig::disableAndSetHighImpedance;


    // write the new config
    writeCurrentConfig();
}

int16_t Ads1115Plus::getLastConversionResults() {
    return (int16_t)readFromAds(address, (byte) AddressPointerReg::conversionRegister);
}

double Ads1115Plus::getLastConversionMillivolts() {
    return getLastConversionResults() * millivoltsPerRawValue();
}

// MARK: Read channels

uint16_t Ads1115Plus::readChannelRaw(byte channel) {
    return readADC_singleEnded(channel);
}

uint16_t Ads1115Plus::readADC_singleEnded(byte channel) {
    if (channel > 3) {
        // TODO: Try look at exceptions config or find a different value
        return 0;
    }

    // Modify the mux config and return the reading
    muxConfig = muxConfigOfSingleChannel(channel);
    return currentConfigSingleShotRead();
}

int16_t Ads1115Plus::readDifferentialRaw01() {
    muxConfig = (uint16_t) MuxConfig::differential01;
    return currentConfigSingleShotRead();
}

int16_t Ads1115Plus::readDifferentialRaw03() {
    muxConfig = (uint16_t) MuxConfig::differential03;
    return currentConfigSingleShotRead();
}

int16_t Ads1115Plus::readDifferentialRaw13() {
    muxConfig = (uint16_t) MuxConfig::differential13;
    return currentConfigSingleShotRead();
}

int16_t Ads1115Plus::readDifferentialRaw23() {
    muxConfig = (uint16_t) MuxConfig::differential23;
    return currentConfigSingleShotRead();
}

int16_t Ads1115Plus::readADC_Differential_2_3() {
    return readDifferentialRaw23();
}

int16_t Ads1115Plus::readADC_Differential_0_1() {
    return readDifferentialRaw01();
}

double Ads1115Plus::readDifferentialMillivolts01() {
    return millivoltsPerRawValue() * readDifferentialRaw01();
}

double Ads1115Plus::readDifferentialMillivolts03() {
    return millivoltsPerRawValue() * readDifferentialRaw03();
}

double Ads1115Plus::readDifferentialMillivolts13() {
    return millivoltsPerRawValue() * readDifferentialRaw13();
}

double Ads1115Plus::readDifferentialMillivolts23() {
    return millivoltsPerRawValue() * readDifferentialRaw23();
}

double Ads1115Plus::readChannelMillivolts(byte channel) {
    uint16_t channelValue = readChannelRaw(channel);
    double millivoltsPerBit = millivoltsPerRawValue();

    return channelValue * millivoltsPerBit;
}

int16_t Ads1115Plus::readRawOnMux(MuxConfig mux) {
    muxConfig = (uint16_t)mux;
    return currentConfigSingleShotRead();
}

double Ads1115Plus::readMillivoltsOnMux(MuxConfig mux) {
    return readRawOnMux(mux) * millivoltsPerRawValue();
}

void Ads1115Plus::clearComparatorLatch() {
    getLastConversionResults();
}

// MARK: Public - Utility methods

void Ads1115Plus::writeCurrentConfig() {
    uint16_t configRegister = buildConfigRegister();
    writeToAds(address, (byte)AddressPointerReg::configRegister, configRegister);
}

double Ads1115Plus::millivoltsPerRawValue() {
    return millivoltsPerRawValue((AdsGain)gain);
}

double Ads1115Plus::millivoltsPerRawValue(AdsGain gain) {
    
    switch ((AdsGain)gain) {

    case AdsGain::twoThirds:
        return 0.1875;
    
    case AdsGain::one:
        return 0.125;
    
    case AdsGain::two:
        return 0.0625;
    
    case AdsGain::four:
        return 0.03215;
    
    case AdsGain::eight:
        return 0.015625;

    case AdsGain::sixteen:
        return 0.0078125;
    
    default:
        return 0;
    }
}


// MARK: Private methods

uint16_t Ads1115Plus::muxConfigOfSingleChannel(byte channel) {
    // Change the mux config
    switch (channel) {
    case 0:
        return (uint16_t)MuxConfig::channel0;
    case 1:
        return (uint16_t)MuxConfig::channel1;
    case 2:
        return (uint16_t)MuxConfig::channel2;
    case 3:
        return (uint16_t)MuxConfig::channel3;
    }

    // Return channel 0 if none is found
    return (uint16_t)MuxConfig::channel0;
}

int16_t Ads1115Plus::currentConfigSingleShotRead() {
    writeCurrentConfig();
    unsigned long readingDelay = delayForChannelReading();
    delay(readingDelay);

    return readFromAds(address, (byte)AddressPointerReg::conversionRegister);
}

unsigned long Ads1115Plus::delayForChannelReading() {
    switch ((AdsSampleSpeed)sampleSpeed) {

    case AdsSampleSpeed::sps8:
        return 126;
    
    case AdsSampleSpeed::sps16:
        return 64;
    
    case AdsSampleSpeed::sps32:
        return 33;

    case AdsSampleSpeed::sps64:
        return 17;
    
    case AdsSampleSpeed::sps128:
        return 9;
    
    case AdsSampleSpeed::sps250:
        return 5;

    case AdsSampleSpeed::sps475:
        return 4;

    case AdsSampleSpeed::sps860:
        return 3;
    
    default:
        return 126; // return the max delay in case the rate is not recognized
    }
}

uint16_t Ads1115Plus::buildConfigRegister() {
    return
        comparatorAssertConfig | // bits 0:1
        comparatorLatching | // bit 2
        comparatorPolarity | // bit 3
        comparatorMode | // bit 4
        sampleSpeed | // bits 5:7
        adsMode | // bit 8
        gain | // bits 9:11
        muxConfig | // bits 12:14
        osConfig; // bit 15
}

double Ads1115Plus::rawValueToMillivolts(int16_t rawValue) {
    return rawValueToMillivolts(rawValue, (AdsGain) gain);
}

double Ads1115Plus::rawValueToMillivolts(int16_t rawValue, AdsGain gain) {
    return rawValue * millivoltsPerRawValue(gain);
}

double Ads1115Plus::millivoltsToRawValue(double millivolts) {
    return millivoltsToRawValue(millivolts, (AdsGain)gain);
}

double Ads1115Plus::millivoltsToRawValue(double millivolts, AdsGain gain) {
    return millivolts / millivoltsPerRawValue(gain);
}