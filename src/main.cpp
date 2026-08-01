/*
 * Управление сервоприводом ASD через CN1 на основе данных с дисплея DWIN
 * Платформа: Arduino Nano ATmega328
 *
 * Подключение:
 * D5 (PULS+)  → CN1 пин 5
 * D3 (PULS-)  → CN1 пин 21
 * D4 (SIGN+)  → CN1 пин 6
 * D2 (SIGN-)  → CN1 пин 22
 *
 * D10 (RX)    → TX дисплея DWIN
 * D11 (TX)    → RX дисплея DWIN
 * (Аппаратный Serial (пины 0, 1) используется только для отладки)
 */

#define USE_SERVO_SIMULATOR

#include <Arduino.h>
#include <SoftwareSerial.h>
#include "dwin_lcm.h"
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

SoftwareSerial dwin_serial(10, 11);

// Конкретные реализации создаются один раз (composition root)
dwin_lcm display(dwin_serial, 115200);
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

    // ВАЖНО: вызываем begin() для SoftwareSerial ПЕРЕД использованием
    dwin_serial.begin(115200);

    Serial.println(F("Система инициализирована. Ожидание команд от DWIN (VP 0x5002)..."));

    delay(500);
}

void loop() {
    if (positioner.is_moving()) {
        while (dwin_serial.available()) {
            dwin_serial.read();
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
