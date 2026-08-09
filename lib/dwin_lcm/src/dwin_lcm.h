#ifndef DWIN_LCM_H
#define DWIN_LCM_H

#include <Arduino.h>
#include "data_source.h"

// Максимальный размер кадра (с запасом для стандартных запросов DWIN)
#define DWIN_MAX_FRAME_LEN 64

/**
 * @brief Драйвер дисплея DWIN, реализующий интерфейс data_source.
 * Обеспечивает низкоуровневый обмен кадрами с дисплеем по протоколу DWIN
 * и предоставляет значение заданной переменной (VP) как источник данных.
 */
class dwin_lcm : public data_source {
public:
    /**
     * @brief Конструктор класса.
     * @param serial_port Ссылка на объект Serial (HardwareSerial).
     * @param vp_addr Адрес переменной VP, значение которой является источником данных.
     */
    dwin_lcm(Stream& serial_port, uint16_t vp_addr = 0x5002);

    /**
     * @brief Команда записи в RAM (0x82).
     * @param addr Адрес переменной VP (например, 0x5000).
     * @param data Указатель на массив данных для записи.
     * @param data_len Количество байт данных.
     * @return true, если кадр успешно сформирован и отправлен.
     */
    bool write_ram(uint16_t addr, const uint8_t* data, size_t data_len);

    /**
     * @brief Команда чтения из RAM (0x83).
     * @param addr Адрес переменной VP (например, 0x5000).
     * @param num_words Количество считываемых слов (1 слово = 2 байта).
     * @return true, если кадр успешно сформирован и отправлен.
     */
    bool read_ram(uint16_t addr, uint8_t num_words);

    /**
     * @brief Неблокирующее чтение ответа от дисплея.
     * @param buffer Буфер для сохранения принятого кадра.
     * @param max_len Максимальный размер буфера.
     * @param timeout Таймаут ожидания в миллисекундах.
     * @return Количество принятых байт (0, если ничего не принято или таймаут).
     */
    size_t read_response(uint8_t* buffer, size_t max_len, uint32_t timeout = 100);

    /**
     * @brief Считывает новое значение заданной VP-переменной.
     * Реализация интерфейса data_source.
     * @param out_value Ссылка на переменную для сохранения сырого значения.
     * @return true, если получено новое значение; false при таймауте или ошибке кадра.
     */
    bool try_read_value(float& out_value) override;

private:
    Stream* _serial;   ///< Указатель на объект Serial.
    uint16_t _vp_addr;  ///< Адрес переменной VP для чтения значения.
};

#endif // DWIN_LCM_H
