/*
 * dwin_lcm — реализация протокола DWIN:
 * парсер кадров (0x82/0x83) и низкоуровневые команды записи/чтения RAM.
 */

#include "dwin_lcm.h"

dwin_lcm::dwin_lcm(Stream& serial_port)
    : _serial(&serial_port),
      _state(ST_IDLE),
      _idx(0),
      _need(0) {}

bool dwin_lcm::write_ram(uint16_t addr, const uint8_t* data, size_t data_len) {
    size_t payload_len = 1 + 2 + data_len;
    if (payload_len + 2 > 64) {
        return false;
    }

    uint8_t frame[64];
    frame[0] = 0x5A;
    frame[1] = 0xA5;
    frame[2] = (uint8_t)payload_len;
    frame[3] = 0x82;
    frame[4] = (uint8_t)(addr >> 8);
    frame[5] = (uint8_t)(addr & 0xFF);

    for (size_t i = 0; i < data_len; i++) {
        frame[6 + i] = data[i];
    }

    _serial->write(frame, 2 + payload_len);
    return true;
}

bool dwin_lcm::read_ram(uint16_t addr, uint8_t num_words) {
    uint8_t frame[7];
    frame[0] = 0x5A;
    frame[1] = 0xA5;
    frame[2] = 0x04;
    frame[3] = 0x83;
    frame[4] = (uint8_t)(addr >> 8);
    frame[5] = (uint8_t)(addr & 0xFF);
    frame[6] = num_words;

    _serial->write(frame, 7);
    return true;
}

bool dwin_lcm::decode_frame(parameter_update& out) {
    if (_need < 4) {
        return false;
    }

    uint8_t cmd = _buf[3];
    uint16_t addr = ((uint16_t)_buf[4] << 8) | _buf[5];

    if (cmd == 0x82) {
        if (_need < 8) {
            return false;
        }
        out.address = addr;
        out.raw_value = (int32_t)(((uint16_t)_buf[6] << 8) | _buf[7]);
        return true;
    }

    if (cmd == 0x83) {
        if (_need < 9 || _buf[6] < 1) {
            return false;
        }
        out.address = addr;
        out.raw_value = (int32_t)(((uint16_t)_buf[7] << 8) | _buf[8]);
        return true;
    }

    return false;
}

bool dwin_lcm::try_read_update(parameter_update& out) {
    while (_serial->available() > 0) {
        uint8_t b = _serial->read();

        switch (_state) {
        case ST_IDLE:
            if (b == 0x5A) {
                _state = ST_HEAD;
            }
            break;

        case ST_HEAD:
            if (b == 0xA5) {
                _state = ST_LEN;
            } else {
                _state = (b == 0x5A) ? ST_HEAD : ST_IDLE;
            }
            break;

        case ST_LEN:
            _buf[0] = 0x5A;
            _buf[1] = 0xA5;
            _buf[2] = b;
            if (b == 0) {
                _state = ST_IDLE;
                break;
            }
            if ((uint16_t)3 + b > sizeof(_buf)) {
                _state = ST_IDLE;
                break;
            }
            _need = (uint8_t)(3 + b);
            _idx = 3;
            _state = ST_DATA;
            if (_idx >= _need) {
                _state = ST_IDLE;
                if (decode_frame(out)) {
                    return true;
                }
            }
            break;

        case ST_DATA:
            if (_idx < sizeof(_buf)) {
                _buf[_idx++] = b;
            }
            if (_idx >= _need) {
                _state = ST_IDLE;
                if (decode_frame(out)) {
                    return true;
                }
            }
            break;
        }
    }

    return false;
}
