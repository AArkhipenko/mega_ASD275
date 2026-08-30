#ifndef DWIN_LCM_SIMULATOR_H
#define DWIN_LCM_SIMULATOR_H

#include <Arduino.h>
#include "data_source.h"

// Максимальная длина строки ввода значения с Serial (без учета '\0')
#define DWIN_SIM_LINE_LEN 32

/**
 * @brief Симулятор дисплея DWIN, реализующий интерфейс data_source.
 * Не обменивается кадрами с реальным дисплеем, а имитирует его работу:
 * источником данных служит Serial — значение VP-переменной вводится
 * с монитора порта в виде числа. Предназначен для отладки прошивки
 * без наличия железа.
 */
class dwin_lcm_simulator : public data_source {
public:
    /**
     * @brief Конструктор класса.
     * @param serial_port Ссылка на объект Serial (источник данных).
     * @param vp_addr Адрес переменной VP, значение которой является источником данных.
     */
    dwin_lcm_simulator(Stream& serial_port, uint16_t vp_addr = 0x5002);

    /**
     * @brief Команда записи в RAM (0x82). Имитирует приём данных дисплеем.
     * @param addr Адрес переменной VP.
     * @param data Указатель на массив данных.
     * @param data_len Количество байт данных.
     * @return true, если данные приняты.
     */
    bool write_ram(uint16_t addr, const uint8_t* data, size_t data_len);

    /**
     * @brief Команда чтения из RAM (0x83). Имитирует запрос к дисплею.
     * @param addr Адрес переменной VP.
     * @param num_words Количество считываемых слов (1 слово = 2 байта).
     * @return true, если запрос принят.
     */
    bool read_ram(uint16_t addr, uint8_t num_words);

    /**
     * @brief Формирует ответ дисплея на запрос чтения.
     * @param buffer Буфер для сохранения сформированного кадра.
     * @param max_len Максимальный размер буфера.
     * @param timeout Таймаут в миллисекундах (в симуляции не используется).
     * @return Количество байт сформированного кадра (0 при ошибке).
     */
    size_t read_response(uint8_t* buffer, size_t max_len, uint32_t timeout = 100);

    /**
     * @brief Считывает новое значение заданной VP-переменной из Serial.
     * Реализация интерфейса data_source.
     * @param out_value Ссылка на переменную для сохранения сырого значения.
     * @return true, если получено новое значение; false при отсутствии данных.
     */
    bool try_read_value(float& out_value) override;

private:
    Stream* _serial;    ///< Указатель на объект Serial (источник данных).
    uint16_t _vp_addr;  ///< Адрес переменной VP для чтения значения.
    int16_t _vp_value;  ///< Текущее значение переменной VP (знаковое 16-битное слово).
    char _line[DWIN_SIM_LINE_LEN]; ///< Буфер накопления строки ввода.
    uint8_t _line_len;  ///< Текущая длина буфера строки.

    /**
     * @brief Разбирает накопленную строку как целое число и обновляет значение VP.
     * @param out_value Ссылка на переменную для сохранения результата.
     * @return true, если строка содержала корректное число.
     */
    bool parse_line(float& out_value);
};

#endif // DWIN_LCM_SIMULATOR_H
