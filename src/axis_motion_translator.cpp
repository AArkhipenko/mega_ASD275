/*
 * axis_motion_translator — реализация запуска движения:
 * расчёт оборотов/скорости из дельты, контроль завершения, подтверждения старта и таймаута.
 * Настройки оси берутся из локального кэша, обновляемого при изменении хранилища настроек.
 */

#include <Arduino.h>
#include "axis_motion_translator.h"
#include "parameter_storage.h"
#include "axis_settings.h"
#include "driver_interface.h"

static const unsigned long MOVE_TIMEOUT_MS = 60000UL;
static const unsigned long MOVE_GRACE_MS   = 20UL;

axis_motion_translator::axis_motion_translator(parameter_storage& storage,
                                               driver_interface& driver,
                                               axis_settings& settings)
    : _storage(storage),
      _driver(driver),
      _settings(settings),
      _rpm_cache(0.0f),
      _revs_per_unit_cache(0.0f),
      _busy(false),
      _motion_observed(false),
      _start_ms(0) {
    refresh_settings();
}

void axis_motion_translator::refresh_settings() {
    _rpm_cache = (float)_settings.rpm();
    _revs_per_unit_cache = _settings.revs_per_degree();
}

void axis_motion_translator::on_settings_changed(parameter_storage& storage) {
    (void)storage;
    refresh_settings();
}

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

    float rpm = _rpm_cache;
    if (rpm < 1.0f) {
        Serial.println(F("[MOVE] Скорость не задана"));
        return false;
    }

    float revs_f = delta * _revs_per_unit_cache;

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
