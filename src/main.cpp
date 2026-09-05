/*
 * Управление сервоприводом ASD275 по параметрам от источника данных (DWIN).
 * Платформа: Arduino Mega 2560 ATmega2560
 *
 * Сборка модулей (dependency injection):
 *   data_source        -> DWIN (Serial1) или симулятор (Serial)
 *   parameter_registry -> реестр параметров по адресам
 *   parameter_router   -> диспетчер обновлений от data_source
 *   scaled_parameter   -> хранилища уставок (угол, расстояние)
 *   axis_settings      -> хранилище настроек оси (скорость, масштаб)
 *   axis_motion_translator -> преобразование «дельта параметра» в команды
 *   driver_interface   -> ASD275 (Modbus RTU, Serial3) или симулятор
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

#include "parameter_addresses.h"
#include "parameter_storage.h"
#include "scaled_parameter.h"
#include "axis_settings.h"
#include "axis_motion_translator.h"
#include "parameter_registry.h"
#include "parameter_router.h"

#ifdef USE_DWIN_LCM_SIMULATOR
dwin_lcm_simulator display(Serial);
#else
dwin_lcm display(Serial1);
#endif

#ifdef USE_SERVO_SIMULATOR
servo_simulator servo_impl;
#else
servo_driver servo_impl;
#endif

scaled_parameter table_angle(param_addr::TABLE_ANGLE, "Угол станины", 0.01f, true);
scaled_parameter distance(param_addr::DISTANCE_TO_STOP, "Расстояние до упора", 0.01f, true);
axis_settings settings(param_addr::SPEED_RPM, param_addr::REVS_PER_DEGREE);

axis_motion_translator angle_translator(table_angle, servo_impl, settings);

parameter_registry registry;
parameter_router router(display, registry);

/// @brief Инициализация системы: настройка источника данных, реестра и привода.
void setup() {
    Serial.begin(9600);
    while (!Serial) { }

#ifndef USE_DWIN_LCM_SIMULATOR
    Serial1.begin(115200);
#endif

    table_angle.attach_translator(&angle_translator);
    settings.attach_translator(&angle_translator);

    registry.register_storage(table_angle);
    registry.register_storage(distance);
    registry.register_storage(settings);

    settings.apply_update(param_addr::SPEED_RPM, 60);
    settings.apply_update(param_addr::REVS_PER_DEGREE, 10000);

    servo_impl.enable();

    Serial.println(F("Система инициализирована"));
#ifdef USE_DWIN_LCM_SIMULATOR
    Serial.println(F("[SIM] Формат команды: <адрес> <значение>"));
    Serial.println(F("[SIM] Примеры: 0x5002 4500 (угол), 0x6000 120 (скорость)"));
#endif
}

/// @brief Главный цикл: диспетчеризация обновлений и контроль движения привода.
void loop() {
    while (router.poll()) { }

#ifndef USE_DWIN_LCM_SIMULATOR
    static unsigned long last_poll = 0;
    static const unsigned long POLL_PERIOD_MS = 200UL;
    unsigned long now = millis();
    if (now - last_poll >= POLL_PERIOD_MS) {
        last_poll = now;
        for (uint8_t i = 0; i < registry.count(); i++) {
            parameter_storage* storage = registry.storage_at(i);
            for (uint8_t j = 0; j < storage->address_count(); j++) {
                display.read_ram(storage->address_at(j), 1);
            }
        }
    }
#endif

    angle_translator.tick();
    delay(1);
}
