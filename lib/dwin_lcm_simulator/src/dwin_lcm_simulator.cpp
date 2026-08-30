#include "dwin_lcm_simulator.h"
#include <stdlib.h>

dwin_lcm_simulator::dwin_lcm_simulator(Stream& serial_port, uint16_t vp_addr)
    : _serial(&serial_port), _vp_addr(vp_addr), _vp_value(0), _line_len(0) {
    _line[0] = '\0';
}

bool dwin_lcm_simulator::write_ram(uint16_t addr, const uint8_t* data, size_t data_len) {
    if (data == nullptr || data_len < 1) {
        return false;
    }

    uint16_t value = (data_len >= 2) ? ((uint16_t)data[0] << 8) | data[1]
                                     : data[0];

    if (addr == _vp_addr) {
        _vp_value = (int16_t)value;
    }

    Serial.print(F("[SIM] Запись VP 0x"));
    Serial.print(addr, HEX);
    Serial.print(F(" = 0x"));
    Serial.print(value, HEX);
    Serial.println();
    return true;
}

bool dwin_lcm_simulator::read_ram(uint16_t addr, uint8_t num_words) {
    Serial.print(F("[SIM] Запрос чтения VP 0x"));
    Serial.print(addr, HEX);
    Serial.print(F(", слов: "));
    Serial.println(num_words);
    return true;
}

// Пример формируемого кадра
// 5a a5 06 83 50 02 01 3e f6
size_t dwin_lcm_simulator::read_response(uint8_t* buffer, size_t max_len, uint32_t timeout) {
    (void)timeout;

    if (buffer == nullptr || max_len < 9) {
        return 0;
    }

    uint16_t word = (uint16_t)_vp_value;

    buffer[0] = 0x5A; // Frame Header H
    buffer[1] = 0xA5; // Frame Header L
    buffer[2] = 0x06; // Byte Count (BC) = CMD(1) + ADDR(2) + LEN(1) + DATA(2)
    buffer[3] = 0x83; // CMD: Ответ на чтение RAM
    buffer[4] = (_vp_addr >> 8) & 0xFF;   // Адрес H
    buffer[5] = _vp_addr & 0xFF;          // Адрес L
    buffer[6] = 0x01;                     // Количество слов
    buffer[7] = (word >> 8) & 0xFF;       // Данные H
    buffer[8] = word & 0xFF;              // Данные L

    return 9;
}

bool dwin_lcm_simulator::try_read_value(float& out_value) {
    while (_serial->available() > 0) {
        char c = (char)_serial->read();

        if (c == '\n' || c == '\r') {
            if (_line_len > 0) {
                _line[_line_len] = '\0';
                _line_len = 0;
                if (parse_line(out_value)) {
                    return true;
                }
            }
        } else if (_line_len < DWIN_SIM_LINE_LEN - 1) {
            _line[_line_len++] = c;
        } else {
            // Переполнение строки — сбрасываем накопленное
            _line_len = 0;
        }
    }

    return false;
}

bool dwin_lcm_simulator::parse_line(float& out_value) {
    char* end_ptr = nullptr;
    long value = strtol(_line, &end_ptr, 10);

    if (end_ptr == _line || *end_ptr != '\0') {
        Serial.println(F("[SIM] Ошибка: ожидалось целое число"));
        return false;
    }

    _vp_value = (int16_t)value;
    out_value = (float)_vp_value;

    Serial.print(F("▶ [SIM] VP 0x"));
    Serial.print(_vp_addr, HEX);
    Serial.print(F(" <- "));
    Serial.print(_vp_value);
    Serial.print(F(" (сырое значение, угол "));
    Serial.print(out_value * 0.01f, 2);
    Serial.println(F("°)"));
    return true;
}
