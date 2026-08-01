#include "dwin_lcm.h"

dwin_lcm::dwin_lcm(Stream& serial_port, uint32_t baudrate, uint16_t vp_addr)
    : _serial(&serial_port), _baudrate(baudrate), _vp_addr(vp_addr) {}

bool dwin_lcm::write_ram(uint16_t addr, const uint8_t* data, size_t data_len) {
    // Byte Count (BC) = CMD(1) + ADDR(2) + DATA(dataLen)
    // Заголовок (2 байта) и сам байт BC в это число НЕ входят.
    size_t payload_len = 1 + 2 + data_len;

    if (payload_len + 2 > DWIN_MAX_FRAME_LEN) {
        return false; // Защита от переполнения буфера
    }

    uint8_t frame[DWIN_MAX_FRAME_LEN];
    frame[0] = 0x5A; // Frame Header H
    frame[1] = 0xA5; // Frame Header L
    frame[2] = (uint8_t)payload_len; // Byte Count (BC)
    frame[3] = 0x82; // CMD: Запись в RAM
    frame[4] = (addr >> 8) & 0xFF;   // Адрес H
    frame[5] = addr & 0xFF;          // Адрес L

    // Копируем полезные данные
    for (size_t i = 0; i < data_len; i++) {
        frame[6 + i] = data[i];
    }

    // Отправляем кадр: 2 байта заголовка + payload_len байт
    _serial->write(frame, 2 + payload_len);
    return true;
}

bool dwin_lcm::read_ram(uint16_t addr, uint8_t num_words) {
    // Byte Count (BC) = CMD(1) + ADDR(2) + LEN(1) = 4 байта
    size_t payload_len = 4;

    if (payload_len + 2 > DWIN_MAX_FRAME_LEN) {
        return false;
    }

    uint8_t frame[DWIN_MAX_FRAME_LEN];
    frame[0] = 0x5A; // Frame Header H
    frame[1] = 0xA5; // Frame Header L
    frame[2] = (uint8_t)payload_len; // Byte Count (BC) = 4
    frame[3] = 0x83; // CMD: Чтение RAM
    frame[4] = (addr >> 8) & 0xFF;   // Адрес H
    frame[5] = addr & 0xFF;          // Адрес L
    frame[6] = num_words;            // Количество слов (LEN)

    // Отправляем кадр: 2 + 4 = 6 байт
    _serial->write(frame, 6);
    return true;
}

// Пример сообщения
// 5a a5 06 83 50 02 01 3e f6
size_t dwin_lcm::read_response(uint8_t* buffer, size_t max_len, uint32_t timeout) {
    uint32_t start_time = millis();
    size_t index = 0;

    // 1. Ищем и читаем заголовок 0x5A 0xA5
    while (millis() - start_time < timeout) {
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
    Serial.println("Somemessage");

    // 2. Читаем байт количества байт (BC)
    while (millis() - start_time < timeout) {
        if (_serial->available() > 0) {
            buffer[2] = _serial->read();
            index = 3;
            break;
        }
    }

    uint8_t bc = buffer[2];
    size_t total_frame_len = 3 + bc; // Общая длина = Header(2) + BC

    if (total_frame_len > max_len) {
        // Очищаем буфер приема, чтобы не ломать следующие кадры
        while (_serial->available()) _serial->read();
        return 0; // Кадр слишком большой для нашего буфера
    }

    // 3. Читаем оставшуюся часть кадра (BC байт)
    while (millis() - start_time < timeout) {
        while (_serial->available() > 0 && index < total_frame_len) {
            buffer[index++] = _serial->read();
        }
        if (index >= total_frame_len) {
            return total_frame_len; // Кадр получен полностью и успешно
        }
    }

    return 0; // Таймаут при ожидании данных
}

bool dwin_lcm::try_read_value(float& out_value) {
    uint8_t buffer[DWIN_MAX_FRAME_LEN];
    size_t len = read_response(buffer, sizeof(buffer), 1000);

    if (len < 9) {
        return false;
    }

    // Ответ на чтение (0x83) для заданной переменной VP, одно слово данных
    if (buffer[3] != 0x83) {
        return false;
    }
    if (buffer[4] != (uint8_t)(_vp_addr >> 8) || buffer[5] != (uint8_t)(_vp_addr & 0xFF)) {
        return false;
    }
    if (buffer[6] != 1) {
        return false;
    }

    out_value = (float)((buffer[7] << 8) | buffer[8]);
    return true;
}
