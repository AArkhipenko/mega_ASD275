/*
 * servo_simulator — имитация драйвера сервопривода (driver_interface) для отладки без железа.
 * Движение длится расчётное время в зависимости от оборотов и скорости.
 */

#ifndef SERVO_SIMULATOR_H
#define SERVO_SIMULATOR_H

#include <Arduino.h>
#include "driver_interface.h"

/// @brief Симулятор драйвера сервопривода для отладки без железа.
class servo_simulator : public driver_interface {
public:
    /// @brief Конструктор.
    servo_simulator();

    /// @brief Включить имитацию привода.
    /// @return true.
    bool enable() override;

    /// @brief Выключить имитацию привода.
    /// @return true.
    bool disable() override;

    /// @brief Запустить имитацию движения.
    /// @param revolutions Обороты (знак — направление).
    /// @param rpm Скорость в об/мин.
    /// @return true, если движение запущено.
    bool start_move(int32_t revolutions, uint16_t rpm) override;

    /// @brief Остановить имитацию движения.
    void stop() override;

    /// @brief Проверка имитации движения.
    /// @return true, если движение в процессе.
    bool is_moving() override;

    /// @brief Ошибки отсутствуют.
    /// @return false.
    bool has_fault() override;

private:
    bool _enabled;             ///< Флаг включения привода.
    int32_t _revolutions;      ///< Заданные обороты (со знаком).
    uint16_t _rpm;             ///< Заданная скорость.
    bool _running;             ///< Флаг имитации движения.
    unsigned long _start_ms;   ///< Время запуска движения.
    unsigned long _duration_ms; ///< Длительность движения.
};

#endif
