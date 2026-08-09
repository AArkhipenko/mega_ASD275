#ifndef SERVO_CONTROLLER_H
#define SERVO_CONTROLLER_H

#include "position_calculator.h"

class servo_interface;

/**
 * @brief Контроллер положения сервопривода.
 * Отслеживает текущее положение, вычисляет изменение положения относительно
 * нового целевого угла и управляет сервоприводом через интерфейс servo_interface.
 */
class servo_controller : public position_calculator {
public:
    /**
     * @brief Конструктор класса.
     * @param servo Ссылка на сервопривод (интерфейс servo_interface).
     * @param initial_position Начальное положение в градусах (по умолчанию 0).
     */
    explicit servo_controller(servo_interface& servo, float initial_position = 0.0f);

    /**
     * @brief Обрабатывает новое целевое значение угла.
     * Вычисляет дельту относительно текущего положения и при необходимости
     * запускает движение сервопривода.
     * @param target_angle Целевой угол в градусах.
     * @return true, если было запущено движение сервопривода.
     */
    bool try_apply_target(float target_angle) override;

    /**
     * @brief Возвращает текущее положение сервопривода в градусах.
     * @return Текущий угол в градусах.
     */
    float current_position() const override;

    /**
     * @brief Проверяет, выполняет ли сервопривод движение в данный момент.
     * @return true, если сервопривод в движении.
     */
    bool is_moving() const override;

private:
    servo_interface& _servo; ///< Сервопривод.
    float _current_angle;    ///< Текущее положение в градусах.

    /// @brief Порог чувствительности (минимальное изменение угла).
    static constexpr float ANGLE_EPSILON = 0.01f;
};

#endif // SERVO_CONTROLLER_H
