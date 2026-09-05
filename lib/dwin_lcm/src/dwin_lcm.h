/*
 * dwin_lcm — источник данных: обмен кадрами с дисплеем DWIN (RS232).
 * Реализует data_source: разбирает кадры 0x82/0x83 в события parameter_update.
 */

#ifndef DWIN_LCM_H
#define DWIN_LCM_H

#include <Arduino.h>
#include "data_source.h"

/// @brief Источник данных на основе дисплея DWIN (протокол по RS232).
class dwin_lcm : public data_source {
public:
    /// @brief Конструктор.
    /// @param serial_port Последовательный порт дисплея (например, Serial1).
    explicit dwin_lcm(Stream& serial_port);

    /// @brief Отправить команду записи в RAM (0x82).
    /// @param addr Адрес переменной VP.
    /// @param data Указатель на данные.
    /// @param data_len Количество байт данных.
    /// @return true, если кадр сформирован и отправлен.
    bool write_ram(uint16_t addr, const uint8_t* data, size_t data_len);

    /// @brief Отправить запрос чтения из RAM (0x83).
    /// @param addr Адрес переменной VP.
    /// @param num_words Количество запрашиваемых слов.
    /// @return true, если запрос отправлен.
    bool read_ram(uint16_t addr, uint8_t num_words);

    /// @brief Неблокирующее чтение события обновления параметра.
    /// @param out Ссылка для сохранения события.
    /// @return true, если получен полный кадр 0x82/0x83.
    bool try_read_update(parameter_update& out) override;

private:
    /// @brief Состояния приёмника кадра.
    enum frame_state {
        ST_IDLE, ///< Ожидание заголовка 0x5A.
        ST_HEAD, ///< Ожидание второго байта заголовка 0xA5.
        ST_LEN,  ///< Приём длины кадра (BC).
        ST_DATA  ///< Приём тела кадра.
    };

    Stream* _serial;   ///< Последовательный порт дисплея.
    frame_state _state; ///< Состояние приёмника.
    uint8_t _buf[32];  ///< Буфер принимаемого кадра.
    uint8_t _idx;      ///< Индекс текущего байта кадра.
    uint8_t _need;     ///< Полная длина ожидаемого кадра.

    /// @brief Разобрать принятый кадр в событие обновления.
    /// @param out Ссылка для сохранения события.
    /// @return true, если кадр содержит событие параметра.
    bool decode_frame(parameter_update& out);
};

#endif
