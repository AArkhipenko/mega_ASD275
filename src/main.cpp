/*
 * Управление сервоприводом ASD через CN1 на основе данных с дисплея DWIN
 * Платформа: Arduino Nano ATmega328
 * 
 * Подключение:
 * D5 (PULS+)  → CN1 пин 5
 * D3 (PULS-)  → CN1 пин 21
 * D4 (SIGN+)  → CN1 пин 6
 * D2 (SIGN-)  → CN1 пин 22
 * 
 * D10 (RX)    → TX дисплея DWIN
 * D11 (TX)    → RX дисплея DWIN
 * (Аппаратный Serial (пины 0, 1) используется только для отладки)
 */

#include <Arduino.h>
#include <SoftwareSerial.h>
#include "servo_driver.h"
#include "DWIN_LCM.h"

SoftwareSerial dwinSerial(10, 11);
DWIN_LCM dwin(dwinSerial, 115200);

ServoDriver servo;
float currentAngle = 0.0;

void setup() {
    Serial.begin(9600);
    while (!Serial) { ; }

    servo.init();

    // ВАЖНО: вызываем begin() для SoftwareSerial ПЕРЕД использованием
    dwinSerial.begin(115200);

    Serial.println(F("====================================="));
    Serial.println(F(" Система инициализирована!"));
    Serial.println(F(" Платформа: Arduino Nano ATmega328"));
    Serial.println(F(" Ожидание команд от DWIN (VP 0x5002)..."));
    Serial.println(F("====================================="));
    
    delay(500);
}

void loop() {
    if (servo.isRunning()) {
        while (dwinSerial.available()) {
            dwinSerial.read();
        }
        delay(1);
        return;
    }

    uint8_t buffer[DWIN_MAX_FRAME_LEN];
    size_t len = dwin.readResponse(buffer, sizeof(buffer), 100);

    if (len >= 9 && buffer[3] == 0x83 && buffer[4] == 0x50 && buffer[5] == 0x02 && buffer[6] == 0x01) {
        uint16_t rawValue = (buffer[7] << 8) | buffer[8];
        float newAngle = rawValue / 100.0;

        Serial.print(F("[DWIN] Новый угол: "));
        Serial.print(newAngle, 2);
        Serial.print(F(" (сырое: "));
        Serial.print(rawValue);
        Serial.println(F(")"));

        float delta = newAngle - currentAngle;

        if (delta != 0.0) {
            bool forward = (delta > 0);
            float absDelta = (delta > 0) ? delta : -delta;
            unsigned long pulsesToMake = (unsigned long)((absDelta / 360.0) * PULSES_PER_REVOLUTION);

            if (pulsesToMake > 0) {
                Serial.print(F("[MOTOR] Дельта: "));
                Serial.print(delta, 2);
                Serial.print(F(" -> "));
                Serial.print(pulsesToMake);
                Serial.print(F(" импульсов "));
                Serial.println(forward ? F("ВПЕРЁД") : F("НАЗАД"));

                servo.setDirection(forward);
                servo.generatePulses(pulsesToMake);
                currentAngle = newAngle;

                Serial.println(F("[MOTOR] Движение завершено."));
            } else {
                Serial.println(F("[MOTOR] Дельта слишком мала."));
            }
        } else {
            Serial.println(F("[DWIN] Угол не изменился."));
        }
    }

    delay(10);
}