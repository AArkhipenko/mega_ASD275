/*
 * Управление сервоприводом ASD через CN1 с учетом данных с дисплея DWIN
 * Платформа: Arduino Mega 2560 ATmega2560
 *
 * Подключение:
 * D5 (PULS+)  → CN1 пин 5
 * D3 (PULS-)  → CN1 пин 21
 * D4 (SIGN+)  → CN1 пин 6
 * D2 (SIGN-)  → CN1 пин 22
 *
 * Serial1 (аппаратный UART):
 * D19 (RX1)   → TX дисплея DWIN
 * D18 (TX1)   → RX дисплея DWIN
 * (Serial (пины 0, 1) используется только для отладки)
 */

#include <Arduino.h>
#ifdef USE_DWIN_LCM_SIMULATOR
#include "dwin_lcm_simulator.h"
#else
#include "dwin_lcm.h"
#endif
#ifdef USE_SERVO_SIMULATOR
#include "servo_simulator.h"
#else
#include "servo_driver.h"
#endif
#include "data_source.h"
#include "angle_source.h"
#include "servo_interface.h"
#include "position_calculator.h"
#include "dwin_angle_source.h"
#include "servo_controller.h"

#ifdef USE_DWIN_LCM_SIMULATOR
// Симулятор: значение VP вводится с монитора Serial (пины 0, 1)
dwin_lcm_simulator display(Serial);
#else
// Дисплей DWIN подключен к аппаратному UART Serial1 (D18/TX1, D19/RX1)
dwin_lcm display(Serial1);
#endif
#ifdef USE_SERVO_SIMULATOR
servo_simulator servo_impl;
#else
servo_driver servo_impl;
#endif

dwin_angle_source angle_reader_impl(display);
servo_controller positioner_impl(servo_impl);

// Далее логика оперирует только абстракциями
servo_interface& servo = servo_impl;
angle_source& angle_reader = angle_reader_impl;
position_calculator& positioner = positioner_impl;

void setup() {
    Serial.begin(9600);
    while (!Serial) { ; }

    servo.init();

#ifndef USE_DWIN_LCM_SIMULATOR
    // ВАЖНО: вызываем begin() для аппаратного Serial1 ПЕРЕД использованием
    Serial1.begin(115200);
#endif

    Serial.println(F("Система инициализирована. Ожидание команд от DWIN (VP 0x5002)..."));

#ifdef USE_DWIN_LCM_SIMULATOR
    Serial.println(F("[SIM] Введите сырое значение VP (например, 4500 = 45.00°) в монитор порта"));
#endif

    delay(500);
}

void loop() {
    if (positioner.is_moving()) {
        while (Serial1.available()) {
            Serial1.read();
        }
        delay(1);
        return;
    }

    float new_angle = 0.0f;
    if (angle_reader.try_read_angle(new_angle)) {
        positioner.try_apply_target(new_angle);
    }

    delay(10);
}
