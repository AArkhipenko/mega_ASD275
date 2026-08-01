#ifndef SERVO_INTERFACE_H
#define SERVO_INTERFACE_H

/**
 * @brief Интерфейс для общения с сервоприводом.
 * Абстрагирует драйвер сервопривода, чтобы управляющая логика
 * не зависела от конкретной реализации железа.
 */
class servo_interface {
public:
    /// @brief Виртуальный деструктор.
    virtual ~servo_interface() = default;

    /**
     * @brief Инициализирует сервопривод (настройка пинов и т.п.).
     */
    virtual void init() = 0;

    /**
     * @brief Задаёт направление вращения.
     * @param forward true — вперёд, false — назад.
     */
    virtual void set_direction(bool forward) = 0;

    /**
     * @brief Выполняет вращение на заданное количество импульсов.
     * @param pulses Количество импульсов.
     */
    virtual void move(unsigned long pulses) = 0;

    /**
     * @brief Преобразует угол в градусах в количество импульсов.
     * @param degrees Угол в градусах.
     * @return Количество импульсов для поворота на заданный угол.
     */
    virtual unsigned long degrees_to_pulses(float degrees) = 0;

    /**
     * @brief Останавливает двигатель.
     */
    virtual void stop() = 0;

    /**
     * @brief Проверяет, выполняет ли двигатель движение в данный момент.
     * @return true, если двигатель в движении.
     */
    virtual bool is_running() const = 0;
};

#endif // SERVO_INTERFACE_H
