/*
 * servo_driver — реализация driver_interface для ASD275 по Modbus RTU (RS485, Serial3).
 * Параметры связи и адреса регистров привода настраиваются макросами ниже.
 */

#ifndef SERVO_DRIVER_H
#define SERVO_DRIVER_H

#include <Arduino.h>
#include "driver_interface.h"

// Пин управления направлением RS485 (RSE).
#ifndef RS485_RSE_PIN
#define RS485_RSE_PIN 2
#endif

// Параметры связи Modbus RTU (PA71–PA73 привода ASD275).
#ifndef SERVO_SLAVE_ADDR
#define SERVO_SLAVE_ADDR     0x01
#endif
#ifndef SERVO_BAUD
#define SERVO_BAUD           9600
#endif
#ifndef SERVO_SERIAL_CONFIG
#define SERVO_SERIAL_CONFIG  SERIAL_8N2
#endif

// Адреса регистров ASD275 (Modbus).
#define SERVO_ADDR_ENABLE    0xB5    ///< Включение привода.
#define SERVO_ADDR_RESET     0x11F   ///< Старт/сброс движения.
#define SERVO_ADDR_REVOLUTIONS 0x202 ///< Количество оборотов.
#define SERVO_ADDR_SPEED     0x204   ///< Скорость вращения.
#define SERVO_ADDR_MONITOR   0x1000  ///< Мониторинг движения.

// Коды функций Modbus RTU.
#define MODBUS_READ_HOLDING  0x03
#define MODBUS_READ_INPUT    0x04
#define MODBUS_WRITE_SINGLE  0x06

// Тайминги Modbus RTU.
#ifndef SERVO_RESPONSE_TIMEOUT
#define SERVO_RESPONSE_TIMEOUT 100
#endif
#ifndef SERVO_FRAME_GAP_MS
#define SERVO_FRAME_GAP_MS 5
#endif

/// @brief Драйвер сервопривода ASD275 по Modbus RTU (RS485).
class servo_driver : public driver_interface {
public:
    /// @brief Конструктор.
    servo_driver();

    /// @brief Настроить порт и включить привод.
    /// @return true при успехе.
    bool enable() override;

    /// @brief Выключить привод.
    /// @return true при успехе.
    bool disable() override;

    /// @brief Задать перемещение и запустить движение.
    /// @param revolutions Обороты (знак — направление, ограничено ±32767).
    /// @param rpm Скорость в об/мин.
    /// @return true, если движение запущено.
    bool start_move(int32_t revolutions, uint16_t rpm) override;

    /// @brief Остановить движение.
    void stop() override;

    /// @brief Проверка вращения привода (регистр 0x1000).
    /// @return true, если привод вращается.
    bool is_moving() override;

    /// @brief Наличие ошибки обмена по Modbus.
    /// @return true при ошибке связи.
    bool has_fault() override;

private:
    /// @brief Переключить направление RS485 (RSE).
    /// @param transmit true — передача, false — приём.
    void set_transmit(bool transmit);

    /// @brief Вычислить CRC-16 Modbus.
    /// @param data Указатель на данные.
    /// @param len Длина данных.
    /// @return Значение CRC.
    uint16_t crc16(const uint8_t* data, size_t len);

    /// @brief Записать один регистр (функция 0x06).
    /// @param reg Адрес регистра.
    /// @param value Значение.
    /// @return true при успешной записи.
    bool write_register(uint16_t reg, uint16_t value);

    /// @brief Прочитать регистр (пробует функции 0x03 и 0x04).
    /// @param reg Адрес регистра.
    /// @param out_value Прочитанное значение.
    /// @return true при успешном чтении.
    bool read_register(uint16_t reg, uint16_t& out_value);

    /// @brief Прочитать регистр заданной функцией.
    /// @param reg Адрес регистра.
    /// @param func Код функции чтения.
    /// @param out_value Прочитанное значение.
    /// @return true при успешном чтении.
    bool read_register_func(uint16_t reg, uint8_t func, uint16_t& out_value);

    bool _comm_fault;        ///< Флаг ошибки обмена Modbus.
    uint32_t _last_error_print; ///< Время последнего вывода ошибки.
};

#endif
