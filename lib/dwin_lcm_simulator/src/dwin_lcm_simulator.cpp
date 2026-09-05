/*
 * dwin_lcm_simulator — реализация ввода параметров из Serial Monitor:
 * построчный разбор команд «адрес значение» в события parameter_update.
 */

#include "dwin_lcm_simulator.h"
#include <stdlib.h>
#include <string.h>

dwin_lcm_simulator::dwin_lcm_simulator(Stream& serial_port)
    : _serial(&serial_port),
      _line_len(0) {
    _line[0] = '\0';
}

bool dwin_lcm_simulator::try_read_update(parameter_update& out) {
    while (_serial->available() > 0) {
        char c = (char)_serial->read();

        if (c == '\n' || c == '\r') {
            if (_line_len > 0) {
                _line[_line_len] = '\0';
                _line_len = 0;
                if (parse_line(out)) {
                    return true;
                }
            }
        } else if (_line_len < (uint8_t)(sizeof(_line) - 1)) {
            _line[_line_len++] = c;
        } else {
            _line_len = 0;
        }
    }

    return false;
}

bool dwin_lcm_simulator::parse_line(parameter_update& out) {
    char* save = 0;
    char* addr_tok = strtok_r(_line, " \t", &save);
    char* val_tok = strtok_r(0, " \t", &save);

    if (addr_tok == 0 || val_tok == 0) {
        Serial.println(F("[SIM] Ожидается: <адрес> <значение>"));
        return false;
    }

    char* end = 0;
    unsigned long addr;
    if ((addr_tok[0] == '0') && (addr_tok[1] == 'x' || addr_tok[1] == 'X')) {
        addr = strtoul(addr_tok, &end, 16);
    } else {
        addr = strtoul(addr_tok, &end, 10);
    }

    if (end == addr_tok || *end != '\0' || addr > 0xFFFFUL) {
        Serial.println(F("[SIM] Неверный адрес"));
        return false;
    }

    long value = strtol(val_tok, &end, 10);
    if (end == val_tok || *end != '\0') {
        Serial.println(F("[SIM] Неверное значение"));
        return false;
    }

    out.address = (uint16_t)addr;
    out.raw_value = (int32_t)value;

    Serial.print(F("[SIM] VP 0x"));
    Serial.print(out.address, HEX);
    Serial.print(F(" = "));
    Serial.println(out.raw_value);

    return true;
}
