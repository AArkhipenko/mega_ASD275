#include <Arduino.h>
#include "dwin_angle_source.h"

dwin_angle_source::dwin_angle_source(data_source& source)
    : _source(source) {}

bool dwin_angle_source::try_read_angle(float& out_angle) {
    float raw_value;
    if (!_source.try_read_value(raw_value)) {
        return false;
    }

    out_angle = raw_value * RAW_TO_DEGREES;

    Serial.print(F("[DWIN] Угол: "));
    Serial.println(out_angle, 2);

    return true;
}
