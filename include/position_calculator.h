#ifndef POSITION_CALCULATOR_H
#define POSITION_CALCULATOR_H

/**
 * @brief Интерфейс вычисления изменения положения сервопривода.
 * Вычисляет требуемое изменение положения относительно текущего состояния
 * и управляет сервоприводом через интерфейс servo_interface.
 */
class position_calculator {
public:
    /// @brief Виртуальный деструктор.
    virtual ~position_calculator() = default;

    /**
     * @brief Обрабатывает новое целевое значение положения (угол в градусах).
     * @param target_angle Целевой угол в градусах.
     * @return true, если было запущено движение сервопривода.
     */
    virtual bool try_apply_target(float target_angle) = 0;

    /**
     * @brief Возвращает текущее положение сервопривода в градусах.
     * @return Текущий угол в градусах.
     */
    virtual float current_position() const = 0;

    /**
     * @brief Проверяет, выполняет ли сервопривод движение в данный момент.
     * @return true, если сервопривод в движении.
     */
    virtual bool is_moving() const = 0;
};

#endif // POSITION_CALCULATOR_H
