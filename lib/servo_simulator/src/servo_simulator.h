#ifndef SERVO_SIMULATOR_H
#define SERVO_SIMULATOR_H

#include <Arduino.h>
#include "servo_interface.h"

// Константа для расчёта оборотов (как у реального привода)
#define PULSES_PER_REV 10000  // 10000 импульсов = 1 оборот

/**
 * @brief Симулятор сервопривода, реализующий интерфейс servo_interface.
 * Не управляет реальными пинами, а имитирует поведение привода:
 * блокирующее движение с расчётной длительностью и логирование в Serial.
 * Предназначен для отладки прошивки без наличия железа.
 */
class servo_simulator : public servo_interface {
public:
    /// @brief Конструктор класса.
    servo_simulator();

    /**
     * @brief Инициализирует симулятор.
     */
    void init() override;

    /**
     * @brief Задаёт направление вращения.
     * @param forward true — вперёд, false — назад.
     */
    void set_direction(bool forward) override;

    /**
     * @brief Имитирует движение на заданное количество импульсов.
     * Длительность движения рассчитывается из ширины импульса,
     * во время движения возможна остановка командой 'S'.
     * @param pulses Количество импульсов (10000 = 1 оборот).
     */
    void move(unsigned long pulses) override;

    /**
     * @brief Преобразует угол в градусах в количество импульсов.
     * @param degrees Угол в градусах.
     * @return Количество импульсов для поворота на заданный угол.
     */
    unsigned long degrees_to_pulses(float degrees) override;

    /**
     * @brief Останавливает имитацию движения.
     */
    void stop() override;

    /**
     * @brief Проверяет, выполняется ли имитация движения.
     * @return true, если движение в процессе.
     */
    bool is_running() const override;

private:
    bool _motor_running;     ///< Флаг имитации движения.
    bool _current_direction; ///< Текущее направление (true = вперед).
    unsigned int _pulse_width; ///< Ширина импульса в микросекундах.
};

#endif // SERVO_SIMULATOR_H
