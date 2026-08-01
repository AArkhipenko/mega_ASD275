#include <Arduino.h>
#include "servo_controller.h"
#include "servo_interface.h"

servo_controller::servo_controller(servo_interface& servo, float initial_position)
    : _servo(servo), _current_angle(initial_position) {}

bool servo_controller::try_apply_target(float target_angle) {
    if (_servo.is_running()) {
        return false;
    }

    float delta = target_angle - _current_angle;

    // Изменение слишком мало — движение не требуется
    if (delta > -ANGLE_EPSILON && delta < ANGLE_EPSILON) {
        return false;
    }

    bool forward = delta > 0.0f;
    float abs_delta = (delta > 0.0f) ? delta : -delta;
    unsigned long pulses = _servo.degrees_to_pulses(abs_delta);

    if (pulses == 0) {
        return false;
    }

    Serial.print(F("[MOTOR] Дельта: "));
    Serial.print(delta, 2);
    Serial.print(F(" -> "));
    Serial.print(pulses);
    Serial.print(F(" импульсов "));
    Serial.println(forward ? F("ВПЕРЁД") : F("НАЗАД"));

    _servo.set_direction(forward);
    _servo.move(pulses);
    _current_angle = target_angle;

    return true;
}

float servo_controller::current_position() const {
    return _current_angle;
}

bool servo_controller::is_moving() const {
    return _servo.is_running();
}
