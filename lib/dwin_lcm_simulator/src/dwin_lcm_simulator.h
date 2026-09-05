/*
 * dwin_lcm_simulator — источник данных для отладки без дисплея.
 * Параметры вводятся в Serial Monitor в формате: <адрес> <значение> (например, 0x5002 4500).
 */

#ifndef DWIN_LCM_SIMULATOR_H
#define DWIN_LCM_SIMULATOR_H

#include <Arduino.h>
#include "data_source.h"

/// @brief Симулятор дисплея DWIN: ввод параметров из Serial Monitor.
class dwin_lcm_simulator : public data_source {
public:
    /// @brief Конструктор.
    /// @param serial_port Последовательный порт для ввода команд (Serial).
    explicit dwin_lcm_simulator(Stream& serial_port);

    /// @brief Неблокирующее чтение введённой команды «адрес значение».
    /// @param out Ссылка для сохранения события.
    /// @return true, если введена корректная команда.
    bool try_read_update(parameter_update& out) override;

private:
    Stream* _serial;   ///< Последовательный порт ввода.
    char _line[40];    ///< Буфер накопления строки команды.
    uint8_t _line_len; ///< Текущая длина строки.

    /// @brief Разобрать строку команды «адрес значение».
    /// @param out Ссылка для сохранения события.
    /// @return true, если строка корректна.
    bool parse_line(parameter_update& out);
};

#endif
