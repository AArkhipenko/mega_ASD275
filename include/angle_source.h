#ifndef ANGLE_SOURCE_H
#define ANGLE_SOURCE_H

/**
 * @brief Интерфейс получения нового значения угла.
 * Абстрагирует источник целевого угла (например, дисплей DWIN),
 * позволяя управляющей логике не зависеть от конкретного источника.
 */
class angle_source {
public:
    /// @brief Виртуальный деструктор.
    virtual ~angle_source() = default;

    /**
     * @brief Получает новое значение угла в градусах.
     * @param out_angle Ссылка на переменную для сохранения угла в градусах.
     * @return true, если получено новое значение угла; false при отсутствии данных.
     */
    virtual bool try_read_angle(float& out_angle) = 0;
};

#endif // ANGLE_SOURCE_H
