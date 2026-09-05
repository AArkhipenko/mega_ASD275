/*
 * servo_driver — реализация Modbus RTU обмена с ASD275:
 * enable/disable, start_move (обороты и скорость), stop, is_moving и контроль ошибок связи.
 */

#include "servo_driver.h"

#define MODBUS_MAX_RETRIES 3

servo_driver::servo_driver()
    : _comm_fault(false),
      _last_error_print(0) {}

void servo_driver::set_transmit(bool transmit) {
    digitalWrite(RS485_RSE_PIN, transmit ? HIGH : LOW);
}

uint16_t servo_driver::crc16(const uint8_t* data, size_t len) {
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

bool servo_driver::enable() {
    pinMode(RS485_RSE_PIN, OUTPUT);
    set_transmit(false);

    Serial3.begin(SERVO_BAUD, SERVO_SERIAL_CONFIG);
    Serial3.setTimeout(SERVO_RESPONSE_TIMEOUT);
    _comm_fault = false;

    bool ok = write_register(SERVO_ADDR_ENABLE, 0x0001);
    ok = write_register(SERVO_ADDR_RESET, 0x0000) && ok;

    if (!ok) {
        Serial.println(F("[DRV] Ошибка включения сервопривода"));
    } else {
        Serial.println(F("[DRV] Сервопривод включён"));
    }
    return ok;
}

bool servo_driver::disable() {
    return write_register(SERVO_ADDR_ENABLE, 0x0000);
}

bool servo_driver::write_register(uint16_t reg, uint16_t value) {
    uint8_t frame[8];
    frame[0] = SERVO_SLAVE_ADDR;
    frame[1] = MODBUS_WRITE_SINGLE;
    frame[2] = (uint8_t)(reg >> 8);
    frame[3] = (uint8_t)(reg & 0xFF);
    frame[4] = (uint8_t)(value >> 8);
    frame[5] = (uint8_t)(value & 0xFF);
    uint16_t crc = crc16(frame, 6);
    frame[6] = (uint8_t)(crc & 0xFF);
    frame[7] = (uint8_t)(crc >> 8);

    for (uint8_t attempt = 0; attempt < MODBUS_MAX_RETRIES; attempt++) {
        delay(SERVO_FRAME_GAP_MS);

        set_transmit(true);
        Serial3.write(frame, sizeof(frame));
        Serial3.flush();
        set_transmit(false);

        uint8_t response[8];
        size_t received = Serial3.readBytes(response, sizeof(response));

        if (received != sizeof(response) ||
            response[0] != SERVO_SLAVE_ADDR ||
            response[1] != MODBUS_WRITE_SINGLE) {
            continue;
        }

        uint16_t resp_crc = crc16(response, 6);
        if (response[6] != (uint8_t)(resp_crc & 0xFF) ||
            response[7] != (uint8_t)(resp_crc >> 8)) {
            continue;
        }

        _comm_fault = false;
        return true;
    }

    _comm_fault = true;
    return false;
}

bool servo_driver::read_register(uint16_t reg, uint16_t& out_value) {
    return read_register_func(reg, MODBUS_READ_HOLDING, out_value) ||
           read_register_func(reg, MODBUS_READ_INPUT, out_value);
}

bool servo_driver::read_register_func(uint16_t reg, uint8_t func, uint16_t& out_value) {
    uint8_t frame[8];
    frame[0] = SERVO_SLAVE_ADDR;
    frame[1] = func;
    frame[2] = (uint8_t)(reg >> 8);
    frame[3] = (uint8_t)(reg & 0xFF);
    frame[4] = 0x00;
    frame[5] = 0x01;
    uint16_t crc = crc16(frame, 6);
    frame[6] = (uint8_t)(crc & 0xFF);
    frame[7] = (uint8_t)(crc >> 8);

    for (uint8_t attempt = 0; attempt < MODBUS_MAX_RETRIES; attempt++) {
        delay(SERVO_FRAME_GAP_MS);

        set_transmit(true);
        Serial3.write(frame, sizeof(frame));
        Serial3.flush();
        set_transmit(false);

        uint8_t response[7];
        size_t received = Serial3.readBytes(response, sizeof(response));

        if (received != sizeof(response) ||
            response[0] != SERVO_SLAVE_ADDR ||
            response[1] != func ||
            response[2] != 0x02) {
            continue;
        }

        uint16_t resp_crc = crc16(response, 5);
        if (response[5] != (uint8_t)(resp_crc & 0xFF) ||
            response[6] != (uint8_t)(resp_crc >> 8)) {
            continue;
        }

        out_value = (uint16_t)((response[3] << 8) | response[4]);
        _comm_fault = false;
        return true;
    }

    _comm_fault = true;
    return false;
}

bool servo_driver::start_move(int32_t revolutions, uint16_t rpm) {
    if (revolutions == 0 || rpm == 0) {
        return false;
    }

    int32_t clamped = revolutions;
    if (clamped > 32767) clamped = 32767;
    if (clamped < -32767) clamped = -32767;

    if (!write_register(SERVO_ADDR_REVOLUTIONS, (uint16_t)(int16_t)clamped)) {
        return false;
    }
    if (!write_register(SERVO_ADDR_SPEED, rpm)) {
        return false;
    }
    if (!write_register(SERVO_ADDR_RESET, 0x0001)) {
        return false;
    }
    write_register(SERVO_ADDR_RESET, 0x0000);
    return true;
}

void servo_driver::stop() {
    write_register(SERVO_ADDR_RESET, 0x0000);
}

bool servo_driver::is_moving() {
    uint16_t status = 0;
    if (!read_register(SERVO_ADDR_MONITOR, status)) {
        if (millis() - _last_error_print >= 2000UL) {
            _last_error_print = millis();
            Serial.println(F("[DRV] Ошибка чтения статуса движения"));
        }
        return true;
    }
    return status != 0;
}

bool servo_driver::has_fault() {
    return _comm_fault;
}
