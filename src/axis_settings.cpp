/*
 * axis_settings — реализация хранилища настроек оси:
 * маршрутизация обновлений по адресам VP и уведомление транслятора о изменении.
 */

#include "axis_settings.h"
#include "parameter_translator.h"

const float axis_settings::REVS_PER_DEG_SCALE = 1e-4f;

axis_settings::axis_settings(uint16_t speed_address, uint16_t revs_address)
    : _speed_address(speed_address),
      _revs_address(revs_address),
      _rpm_raw(0),
      _revs_raw(0) {}

uint16_t axis_settings::address() const {
    return _speed_address;
}

bool axis_settings::owns(uint16_t address) const {
    return address == _speed_address || address == _revs_address;
}

uint16_t axis_settings::address_at(uint8_t index) const {
    if (index == 1) {
        return _revs_address;
    }
    return _speed_address;
}

bool axis_settings::apply_update(uint16_t address, int32_t raw_value) {
    bool changed = false;

    if (address == _speed_address) {
        changed = (raw_value != _rpm_raw);
        _rpm_raw = raw_value;
    } else if (address == _revs_address) {
        changed = (raw_value != _revs_raw);
        _revs_raw = raw_value;
    } else {
        return false;
    }

    if (changed && _translator) {
        _translator->on_settings_changed(*this);
    }
    return true;
}
