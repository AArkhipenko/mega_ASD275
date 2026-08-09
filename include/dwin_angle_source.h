#ifndef DWIN_ANGLE_SOURCE_H
#define DWIN_ANGLE_SOURCE_H

#include "angle_source.h"
#include "data_source.h"

/**
 * @brief Источник угла на основе дисплея DWIN.
 * Получает сырое значение из источника данных (интерфейс data_source)
 * и преобразует его в угол в градусах.
 */
class dwin_angle_source : public angle_source {
public:
    /**
     * @brief Конструктор класса.
     * @param source Источник данных, предоставляющий сырые значения угла.
     */
    explicit dwin_angle_source(data_source& source);

    /**
     * @brief Получает новое значение угла в градусах.
     * @param out_angle Ссылка на переменную для сохранения угла в градусах.
     * @return true, если получено новое значение угла.
     */
    bool try_read_angle(float& out_angle) override;

private:
    data_source& _source; ///< Источник данных (дисплей DWIN).

    /// @brief Множитель перевода сырого значения (1/100 градуса) в градусы.
    static constexpr float RAW_TO_DEGREES = 0.01f;
};

#endif // DWIN_ANGLE_SOURCE_H
