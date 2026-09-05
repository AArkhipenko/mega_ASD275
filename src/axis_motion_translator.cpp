/*
 * axis_motion_translator — реализация запуска движения:
 * расчёт оборотов/скорости из дельты, контроль завершения, подтверждения старта и таймаута.
 */

#include <Arduino.h>
#include "axis_motion_translator.h"
#include "parameter_storage.h"
#include "driver_interface.h"

static const unsigned long MOVE_TIMEOUT_MS = 60000UL;
static const unsigned long MOVE_GRACE_MS   = 20UL;

axis_motion_translator::axis_motion_translator(parameter_storage& storage,
                                               driver_interface& driver,
                                               const parameter_storage* rpm_provider,
                                               const parameter_storage* revs_per_unit_provider)
    : _storage(storage),
      _driver(driver),
      _rpm_provider(rpm_provider),
      _revs_per_unit(revs_per_unit_provider),
      _busy(false),
      _motion_observed(false),
      _start_ms(0) {}

bool axis_motion_translator::on_storage_changed(parameter_storage& storage) {
    if (_busy) {
        Serial.println(F("[MOVE] Привод занят, параметр не принят"));
        return false;
    }

    float delta = storage.target_units() - storage.current_units();
    float abs_delta = delta < 0.0f ? -delta : delta;
    if (abs_delta < 0.001f) {
        return false;
    }

    float rpm = 0.0f;
    if (_rpm_provider != 0) {
        rpm = _rpm_provider->current_units();
    }
    if (rpm < 1.0f) {
        Serial.println(F("[MOVE] Скорость не задана"));
        return false;
    }

    float revs_f = delta;
    if (_revs_per_unit != 0) {
        revs_f *= _revs_per_unit->current_units();
    }

    int32_t revs = (int32_t)(revs_f >= 0.0f ? revs_f + 0.5f : revs_f - 0.5f);
    if (revs > 32767) revs = 32767;
    if (revs < -32767) revs = -32767;
    if (revs == 0) {
        Serial.println(F("[MOVE] Перемещение меньше одного оборота"));
        return false;
    }

    Serial.print(F("[MOVE] Дельта "));
    Serial.print(delta, 3);
    Serial.print(F(" -> "));
    Serial.print(revs);
    Serial.print(F(" об., "));
    Serial.print((uint16_t)rpm);
    Serial.println(F(" об/мин"));

    if (!_driver.start_move(revs, (uint16_t)rpm)) {
        Serial.println(F("[MOVE] Не удалось запустить движение"));
        return false;
    }

    _busy = true;
    _start_ms = millis();
    _motion_observed = false;
    return true;
}

void axis_motion_translator::tick() {
    if (!_busy) {
        return;
    }

    if (_driver.is_moving()) {
        _motion_observed = true;
        if (millis() - _start_ms >= MOVE_TIMEOUT_MS) {
            _driver.stop();
            _busy = false;
            Serial.println(F("[MOVE] Таймаут движения"));
        }
        return;
    }

    if (millis() - _start_ms < MOVE_GRACE_MS) {
        return;
    }

    if (!_motion_observed) {
        _busy = false;
        Serial.println(F("[MOVE] Нет подтверждения движения"));
        return;
    }

    _busy = false;
    _storage.commit_target();
    Serial.println(F("[MOVE] Позиция достигнута"));
}

bool axis_motion_translator::is_busy() const {
    return _busy;
}
