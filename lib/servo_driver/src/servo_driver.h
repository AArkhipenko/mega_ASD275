#ifndef SERVO_DRIVER_H
#define SERVO_DRIVER_H

#include <Arduino.h>
#include "servo_interface.h"

// Определение пинов для Arduino Nano
// Используем цифровые пины D2-D13
#define PULS_PLUS  5   // D5 - PULS+ (прямой сигнал)
#define PULS_MINUS 3   // D3 - PULS- (инверсный сигнал)
#define SIGN_PLUS  4   // D4 - SIGN+ (прямой сигнал)
#define SIGN_MINUS 2   // D2 - SIGN- (инверсный сигнал)

// Константы для расчета оборотов
#define PULSES_PER_REVOLUTION 10000  // 10000 импульсов = 1 оборот

/**
 * @brief Драйвер сервопривода ASD, реализующий интерфейс servo_interface.
 * Генерирует импульсы управления (PULS) и сигналы направления (SIGN) через CN1.
 */
class servo_driver : public servo_interface {
public:
    /// @brief Конструктор класса.
    servo_driver();

    /**
     * @brief Инициализирует пины управления сервоприводом.
     */
    void init() override;

    /**
     * @brief Задаёт направление вращения.
     * @param forward true — вперёд (CCW), false — назад (CW).
     */
    void set_direction(bool forward) override;

    /**
     * @brief Выполняет вращение на заданное количество импульсов.
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
     * @brief Останавливает двигатель.
     */
    void stop() override;

    /**
     * @brief Проверяет, выполняет ли двигатель движение в данный момент.
     * @return true, если двигатель в движении.
     */
    bool is_running() const override;

    /**
     * @brief Устанавливает ширину импульса в микросекундах.
     * @param width Ширина импульса (2..1000 мкс).
     */
    void set_pulse_width(unsigned int width);

    /**
     * @brief Выводит текущий статус в Serial.
     */
    void print_status() const;

private:
    bool _motor_running;       ///< Флаг работы двигателя.
    bool _current_direction;   ///< Текущее направление (true = вперед).
    unsigned int _pulse_width; ///< Ширина импульса в микросекундах.

    /**
     * @brief Устанавливает состояние импульсного сигнала.
     * @param state true — активный импульс, false — пассивный.
     */
    void set_pulse(bool state);

    /**
     * @brief Устанавливает сигналы направления.
     * @param forward true — вперёд, false — назад.
     */
    void set_signals(bool forward);
};

#endif // SERVO_DRIVER_H
