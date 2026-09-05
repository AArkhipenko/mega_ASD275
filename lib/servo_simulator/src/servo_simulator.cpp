/*
 * servo_simulator — реализация имитации движения сервопривода по времени выполнения.
 */

#include "servo_simulator.h"

servo_simulator::servo_simulator()
    : _enabled(false),
      _revolutions(0),
      _rpm(0),
      _running(false),
      _start_ms(0),
      _duration_ms(0) {}

bool servo_simulator::enable() {
    _enabled = true;
    _running = false;
    _revolutions = 0;
    _rpm = 0;
    Serial.println(F("[SIM] Сервопривод включён"));
    return true;
}

bool servo_simulator::disable() {
    _enabled = false;
    _running = false;
    Serial.println(F("[SIM] Сервопривод выключен"));
    return true;
}

bool servo_simulator::start_move(int32_t revolutions, uint16_t rpm) {
    if (!_enabled || revolutions == 0 || rpm == 0 || _running) {
        return false;
    }

    _revolutions = revolutions;
    _rpm = rpm;
    _running = true;
    _start_ms = millis();

    unsigned long revs = (revolutions < 0) ? (unsigned long)(-revolutions) : (unsigned long)revolutions;
    _duration_ms = revs * 60000UL / rpm;

    Serial.print(F("[SIM] Движение: "));
    Serial.print(revs);
    Serial.print(F(" об., "));
    Serial.print(rpm);
    Serial.println(F(" об/мин"));
    return true;
}

void servo_simulator::stop() {
    _running = false;
    Serial.println(F("[SIM] Стоп"));
}

bool servo_simulator::is_moving() {
    if (!_running) {
        return false;
    }
    if (millis() - _start_ms >= _duration_ms) {
        _running = false;
        Serial.println(F("[SIM] Движение завершено"));
    }
    return _running;
}

bool servo_simulator::has_fault() {
    return false;
}
