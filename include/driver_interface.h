/*
 * driver_interface — интерфейс управления драйвером сервопривода.
 * Логические команды движения, единые для разных транспортов (Modbus RTU, RS111).
 */

#ifndef DRIVER_INTERFACE_H
#define DRIVER_INTERFACE_H

#include <stdint.h>

/// @brief Интерфейс управления драйвером сервопривода.
class driver_interface {
public:
    /// @brief Виртуальный деструктор.
    virtual ~driver_interface() {}

    /// @brief Включить привод и сбросить движение (вызывается один раз при старте).
    /// @return true при успехе.
    virtual bool enable() = 0;

    /// @brief Выключить привод.
    /// @return true при успехе.
    virtual bool disable() = 0;

    /// @brief Задать относительное перемещение и запустить движение.
    /// @param revolutions Количество оборотов (знак задаёт направление).
    /// @param rpm Скорость в об/мин (больше нуля).
    /// @return true, если движение запущено.
    virtual bool start_move(int32_t revolutions, uint16_t rpm) = 0;

    /// @brief Остановить движение.
    virtual void stop() = 0;

    /// @brief Проверка, выполняет ли привод движение.
    /// @return true, если привод вращается.
    virtual bool is_moving() = 0;

    /// @brief Наличие ошибки связи или привода.
    /// @return true при ошибке.
    virtual bool has_fault() = 0;
};

#endif
