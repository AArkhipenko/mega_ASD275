#include "servo_simulator.h"

servo_simulator::servo_simulator()
    : _motor_running(false), _current_direction(true), _pulse_width(10) {}

void servo_simulator::init() {
    _motor_running = false;
    _current_direction = true;
    Serial.println(F("[SIM] Симулятор сервопривода инициализирован"));
}

void servo_simulator::set_direction(bool forward) {
    if (_motor_running) {
        Serial.println(F("⚠ [SIM] Нельзя менять направление во время движения!"));
        return;
    }
    _current_direction = forward;
}

void servo_simulator::move(unsigned long pulses) {
    if (_motor_running) {
        Serial.println(F("⚠ [SIM] Двигатель уже работает! Дождитесь завершения."));
        return;
    }

    if (pulses == 0) {
        Serial.println(F("⚠ [SIM] Количество импульсов должно быть больше 0!"));
        return;
    }

    _motor_running = true;

    // Расчётная длительность: 2 импульса-перехода (HIGH+LOW) на импульс
    unsigned long sim_duration_ms = (2UL * _pulse_width * pulses) / 1000UL;

    Serial.print(F("▶ [SIM] Движение: "));
    Serial.print(pulses);
    Serial.print(F(" имп. ("));
    Serial.print((float)pulses / PULSES_PER_REV, 2);
    Serial.print(F(" об.), "));
    Serial.print(_current_direction ? F("ВПЕРЕД") : F("НАЗАД"));
    Serial.print(F(", ~"));
    Serial.print(sim_duration_ms);
    Serial.println(F(" мс"));

    unsigned long start_time = millis();

    // Имитация движения с возможностью остановки командой 'S'
    while (sim_duration_ms > 0) {
        delay(10);
        sim_duration_ms = (sim_duration_ms > 10) ? sim_duration_ms - 10 : 0;

        if (Serial.available()) {
            char cmd = Serial.read();
            if (cmd == 'S' || cmd == 's') {
                Serial.println(F("\n⏹ [SIM] Остановка по команде!"));
                _motor_running = false;
                return;
            }
        }
    }

    unsigned long elapsed_time = millis() - start_time;

    Serial.print(F("✅ [SIM] Готово за "));
    Serial.print(elapsed_time);
    Serial.println(F(" мс"));

    _motor_running = false;
}

unsigned long servo_simulator::degrees_to_pulses(float degrees) {
    if (degrees <= 0.0f) {
        return 0;
    }
    return (unsigned long)((degrees / 360.0f) * PULSES_PER_REV);
}

void servo_simulator::stop() {
    _motor_running = false;
    Serial.println(F("⏹ [SIM] СТОП"));
}

bool servo_simulator::is_running() const {
    return _motor_running;
}
