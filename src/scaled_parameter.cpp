/*
 * scaled_parameter — реализация хранилища параметра:
 * диапазон значений, дельта и вызов привязанного транслятора.
 */

#include <Arduino.h>
#include <limits.h>
#include "scaled_parameter.h"
#include "parameter_translator.h"

scaled_parameter::scaled_parameter(uint16_t address, const char* name, float unit_scale, bool system_value)
    : _address(address),
      _name(name),
      _unit_scale(unit_scale),
      _system_value(system_value),
      _current_raw(0),
      _target_raw(0),
      _min_raw(INT32_MIN),
      _max_raw(INT32_MAX) {}

void scaled_parameter::set_raw_range(int32_t min_raw, int32_t max_raw) {
    _min_raw = min_raw;
    _max_raw = max_raw;
}

uint16_t scaled_parameter::address() const {
    return _address;
}

bool scaled_parameter::is_system_value() const {
    return _system_value;
}

int32_t scaled_parameter::current_raw() const {
    return _current_raw;
}

int32_t scaled_parameter::target_raw() const {
    return _target_raw;
}

void scaled_parameter::commit_target() {
    _current_raw = _target_raw;
}

float scaled_parameter::current_units() const {
    return (float)_current_raw * _unit_scale;
}

float scaled_parameter::target_units() const {
    return (float)_target_raw * _unit_scale;
}

bool scaled_parameter::apply_update(uint16_t address, int32_t raw_value) {
    if (address != _address) {
        return false;
    }

    if (raw_value < _min_raw || raw_value > _max_raw) {
        Serial.print(F("[PARAM] "));
        Serial.print(_name);
        Serial.println(F(": значение вне диапазона"));
        return false;
    }

    _target_raw = raw_value;

    if (raw_value == _current_raw) {
        return false;
    }

    if (_translator) {
        if (!_translator->on_storage_changed(*this)) {
            _target_raw = _current_raw;
            return false;
        }
        return true;
    }

    _current_raw = _target_raw;
    return true;
}
