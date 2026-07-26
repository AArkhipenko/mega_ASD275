#ifndef DWIN_LCM_H
#define DWIN_LCM_H

#include <Arduino.h>

// Максимальный размер кадра (с запасом для стандартных запросов DWIN)
#define DWIN_MAX_FRAME_LEN 64

class DWIN_LCM {
private:
    Stream* _serial;  // Изменено с HardwareSerial* на Stream*
    uint32_t _baudrate;

public:
    /**
     * @brief Конструктор класса
     * @param serialPort Ссылка на объект Serial (HardwareSerial или SoftwareSerial)
     * @param baudrate Скорость передачи данных (по умолчанию 115200)
     */
    DWIN_LCM(Stream& serialPort, uint32_t baudrate = 115200);

    /**
     * @brief Команда записи в RAM (0x82)
     * @param addr Адрес переменной VP (например, 0x5000)
     * @param data Указатель на массив данных для записи
     * @param dataLen Количество байт данных
     * @return true если кадр успешно сформирован и отправлен
     */
    bool writeRAM(uint16_t addr, const uint8_t* data, size_t dataLen);

    /**
     * @brief Команда чтения из RAM (0x83)
     * @param addr Адрес переменной VP (например, 0x5000)
     * @param numWords Количество считываемых слов (1 слово = 2 байта)
     * @return true если кадр успешно сформирован и отправлен
     */
    bool readRAM(uint16_t addr, uint8_t numWords);

    /**
     * @brief Неблокирующее чтение ответа от дисплея
     * @param buffer Буфер для сохранения принятого кадра
     * @param maxLen Максимальный размер буфера
     * @param timeout Таймаут ожидания в миллисекундах
     * @return Количество принятых байт (0 если ничего не принято или таймаут)
     */
    size_t readResponse(uint8_t* buffer, size_t maxLen, uint32_t timeout = 100);
};

#endif // DWIN_LCM_H