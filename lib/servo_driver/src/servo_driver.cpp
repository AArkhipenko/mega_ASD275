#include "servo_driver.h"

servo_driver::servo_driver()
    : _motor_running(false), _current_direction(true), _pulse_width(10) {}

void servo_driver::init() {
    // Настройка всех пинов как выходы
    pinMode(PULS_PLUS, OUTPUT);
    pinMode(PULS_MINUS, OUTPUT);
    pinMode(SIGN_PLUS, OUTPUT);
    pinMode(SIGN_MINUS, OUTPUT);

    // Начальное состояние
    set_pulse(false);
    set_signals(true);  // Направление "Вперед"
}

void servo_driver::set_pulse(bool state) {
    if (state) {
        digitalWrite(PULS_PLUS, HIGH);
        digitalWrite(PULS_MINUS, LOW);
    } else {
        digitalWrite(PULS_PLUS, LOW);
        digitalWrite(PULS_MINUS, HIGH);
    }
}

void servo_driver::set_signals(bool forward) {
    _current_direction = forward;
    if (forward) {
        digitalWrite(SIGN_PLUS, HIGH);
        digitalWrite(SIGN_MINUS, LOW);
    } else {
        digitalWrite(SIGN_PLUS, LOW);
        digitalWrite(SIGN_MINUS, HIGH);
    }
}

void servo_driver::move(unsigned long pulses) {
    if (_motor_running) {
        Serial.println(F("⚠ Двигатель уже работает! Дождитесь завершения."));
        return;
    }

    if (pulses == 0) {
        Serial.println(F("⚠ Количество импульсов должно быть больше 0!"));
        return;
    }

    _motor_running = true;

    unsigned long start_time = millis();

    // Локальные переменные для быстрого доступа к пинам
    uint8_t puls_plus_pin = PULS_PLUS;
    uint8_t puls_minus_pin = PULS_MINUS;

    if (_pulse_width < 2) {
        Serial.println(F("⚠ Внимание: слишком маленькая ширина импульса!"));
    }

    for (unsigned long i = 0; i < pulses; i++) {
        // PULS+ = HIGH, PULS- = LOW
        digitalWrite(puls_plus_pin, HIGH);
        digitalWrite(puls_minus_pin, LOW);
        delayMicroseconds(_pulse_width);

        // PULS+ = LOW, PULS- = HIGH
        digitalWrite(puls_plus_pin, LOW);
        digitalWrite(puls_minus_pin, HIGH);
        delayMicroseconds(_pulse_width);

        // Индикация прогресса (каждые 10000 импульсов)
        if (i % 10000 == 0 && i > 0) {
            Serial.print(F("."));
        }

        // Проверка на STOP команду
        if (Serial.available()) {
            char cmd = Serial.read();
            if (cmd == 'S' || cmd == 's') {
                Serial.println(F("\n⏹ Остановка по команде!"));
                _motor_running = false;
                set_pulse(false);
                return;
            }
        }
    }

    // Возврат в исходное состояние
    set_pulse(false);

    unsigned long elapsed_time = millis() - start_time;

    Serial.println();
    Serial.print(F("✅ Готово за "));
    Serial.print(elapsed_time);
    Serial.println(F(" мс"));

    _motor_running = false;
}

unsigned long servo_driver::degrees_to_pulses(float degrees) {
    if (degrees <= 0.0f) {
        return 0;
    }
    return (unsigned long)((degrees / 360.0f) * PULSES_PER_REVOLUTION);
}

void servo_driver::set_direction(bool forward) {
    if (_motor_running) {
        Serial.println(F("⚠ Нельзя менять направление во время движения!"));
        return;
    }

    set_signals(forward);
}

void servo_driver::stop() {
    set_pulse(false);
    _motor_running = false;
    Serial.println(F("⏹ СТОП"));
}

void servo_driver::set_pulse_width(unsigned int width) {
    if (width < 2) {
        Serial.println(F("⚠ Минимальная ширина импульса: 2 мкс"));
        _pulse_width = 2;
    } else if (width > 1000) {
        Serial.println(F("⚠ Максимальная ширина импульса: 1000 мкс"));
        _pulse_width = 1000;
    } else {
        _pulse_width = width;
    }
}

bool servo_driver::is_running() const {
    return _motor_running;
}

void servo_driver::print_status() const {
    Serial.println(F("=== СТАТУС СЕРВОДВИГАТЕЛЯ ==="));
    Serial.print(F("Состояние: "));
    Serial.println(_motor_running ? F("РАБОТАЕТ") : F("ОСТАНОВЛЕН"));
    Serial.print(F("Направление: "));
    Serial.println(_current_direction ? F("ВПЕРЕД (CCW)") : F("НАЗАД (CW)"));
    Serial.print(F("Ширина импульса: "));
    Serial.print(_pulse_width);
    Serial.println(F(" мкс"));
    Serial.println(F("=============================="));
}
