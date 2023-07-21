// This file is part of the DMComm project by BladeSabre. License: MIT.

#include <Arduino.h>
#include "DMComm.h"

namespace DMComm {

void BaseProngOutput::setActiveLevel(uint8_t level) {
    active_level_ = level;
    idle_level_ = level == HIGH ? LOW : HIGH;
}

DComOutput::DComOutput(uint8_t pin_out, uint8_t pin_notOE) :
        pin_out_(pin_out), pin_notOE_(pin_notOE) {
    setActiveLevel(LOW);
}

DComOutput::~DComOutput() {
    end();
}

void DComOutput::begin() {
    pinModeMaybe(pin_out_, OUTPUT);
    pinModeMaybe(pin_notOE_, OUTPUT);
    release();
}

void DComOutput::end() {}

void DComOutput::driveActive() {
    digitalWriteMaybe(pin_out_, active_level_);
    digitalWriteMaybe(pin_notOE_, LOW);
}

void DComOutput::driveInactive() {
    digitalWriteMaybe(pin_out_, idle_level_);
    digitalWriteMaybe(pin_notOE_, LOW);
}

void DComOutput::release() {
    digitalWriteMaybe(pin_notOE_, HIGH);
    digitalWriteMaybe(pin_out_, idle_level_);  // "don't care" on D-Com, but matters for A-Com
}

void BaseProngInput::setActiveLevel(uint8_t level) {
    active_level_ = level;
    idle_level_ = level == HIGH ? LOW : HIGH;
}

AnalogProngInput::AnalogProngInput(uint8_t pin_in, uint16_t board_voltage_mV, uint8_t read_resolution) :
        pin_in_(pin_in), board_voltage_mV_(board_voltage_mV), read_resolution_(read_resolution) {
    setActiveLevel(LOW);
    setThreshold(board_voltage_mV / 2);
}

AnalogProngInput::~AnalogProngInput() {
    end();
}

void AnalogProngInput::begin() {
    pinMode(pin_in_, INPUT);
}

void AnalogProngInput::end() {}

void AnalogProngInput::setThreshold(uint16_t threshold_mV) {
    uint32_t max_input = (1 << read_resolution_) - 1;
    threshold_mV_ = threshold_mV;
    threshold_units_ = (uint16_t) (max_input * threshold_mV / board_voltage_mV_);
}

bool AnalogProngInput::isActive() {
    uint16_t read = analogRead(pin_in_);
    return (read >= threshold_units_) != (active_level_ == LOW);  // != as XOR
}

uint16_t AnalogProngInput::voltage() {
    uint32_t max_input = (1 << read_resolution_) - 1;
    uint32_t read = analogRead(pin_in_);
    return (uint16_t) (read * board_voltage_mV_ / max_input);
}

}  // namespace DMComm