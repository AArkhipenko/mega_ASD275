#include "DWIN_LCM.h"

DWIN_LCM::DWIN_LCM(Stream& serialPort, uint32_t baudrate) 
    : _serial(&serialPort), _baudrate(baudrate) {}

bool DWIN_LCM::writeRAM(uint16_t addr, const uint8_t* data, size_t dataLen) {
    // Byte Count (BC) = CMD(1) + ADDR(2) + DATA(dataLen)
    // Заголовок (2 байта) и сам байт BC в это число НЕ входят.
    size_t payloadLen = 1 + 2 + dataLen;
    
    if (payloadLen + 2 > DWIN_MAX_FRAME_LEN) {
        return false; // Защита от переполнения буфера
    }

    uint8_t frame[DWIN_MAX_FRAME_LEN];
    frame[0] = 0x5A; // Frame Header H
    frame[1] = 0xA5; // Frame Header L
    frame[2] = (uint8_t)payloadLen; // Byte Count (BC)
    frame[3] = 0x82; // CMD: Запись в RAM
    frame[4] = (addr >> 8) & 0xFF;   // Адрес H
    frame[5] = addr & 0xFF;          // Адрес L

    // Копируем полезные данные
    for (size_t i = 0; i < dataLen; i++) {
        frame[6 + i] = data[i];
    }

    // Отправляем кадр: 2 байта заголовка + payloadLen байт
    _serial->write(frame, 2 + payloadLen);
    return true;
}

bool DWIN_LCM::readRAM(uint16_t addr, uint8_t numWords) {
    // Byte Count (BC) = CMD(1) + ADDR(2) + LEN(1) = 4 байта
    size_t payloadLen = 4; 
    
    if (payloadLen + 2 > DWIN_MAX_FRAME_LEN) {
        return false;
    }

    uint8_t frame[DWIN_MAX_FRAME_LEN];
    frame[0] = 0x5A; // Frame Header H
    frame[1] = 0xA5; // Frame Header L
    frame[2] = (uint8_t)payloadLen; // Byte Count (BC) = 4
    frame[3] = 0x83; // CMD: Чтение RAM
    frame[4] = (addr >> 8) & 0xFF;   // Адрес H
    frame[5] = addr & 0xFF;          // Адрес L
    frame[6] = numWords;             // Количество слов (LEN)

    // Отправляем кадр: 2 + 4 = 6 байт
    _serial->write(frame, 6);
    return true;
}

size_t DWIN_LCM::readResponse(uint8_t* buffer, size_t maxLen, uint32_t timeout) {
    uint32_t startTime = millis();
    size_t index = 0;

    // 1. Ищем и читаем заголовок 0x5A 0xA5
    while (millis() - startTime < timeout) {
        if (_serial->available() >= 2) {
            if (_serial->peek() == 0x5A) {
                _serial->read(); // Читаем 0x5A
                if (_serial->peek() == 0xA5) {
                    _serial->read(); // Читаем 0xA5
                    buffer[0] = 0x5A;
                    buffer[1] = 0xA5;
                    index = 2;
                    break;
                }
            } else {
                _serial->read(); // Пропускаем мусорный байт
            }
        }
    }

    if (index < 2) return 0; // Заголовок не найден за время timeout

    // 2. Читаем байт количества байт (BC)
    while (millis() - startTime < timeout) {
        if (_serial->available() > 0) {
            buffer[2] = _serial->read();
            index = 3;
            break;
        }
    }

    uint8_t bc = buffer[2];
    size_t totalFrameLen = 2 + bc; // Общая длина = Header(2) + BC

    if (totalFrameLen > maxLen) {
        // Очищаем буфер приема, чтобы не ломать следующие кадры
        while(_serial->available()) _serial->read();
        return 0; // Кадр слишком большой для нашего буфера
    }

    // 3. Читаем оставшуюся часть кадра (BC байт)
    while (millis() - startTime < timeout) {
        while (_serial->available() > 0 && index < totalFrameLen) {
            buffer[index++] = _serial->read();
        }
        if (index >= totalFrameLen) {
            return totalFrameLen; // Кадр получен полностью и успешно
        }
    }

    return 0; // Таймаут при ожидании данных
}