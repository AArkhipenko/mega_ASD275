/*
 * scaled_parameter — хранилище параметра с линейным масштабом
 * «сырое значение -> физические единицы» (угол, расстояние, системные переменные).
 */

#ifndef SCALED_PARAMETER_H
#define SCALED_PARAMETER_H

#include "parameter_storage.h"

/// @brief Хранилище параметра с линейным масштабом в физические единицы.
class scaled_parameter : public parameter_storage {
public:
    /// @brief Конструктор.
    /// @param address Адрес параметра.
    /// @param name Имя параметра (для диагностики).
    /// @param unit_scale Множитель перевода сырого значения в физические единицы.
    /// @param system_value Является ли параметр системным (уставкой).
    scaled_parameter(uint16_t address, const char* name, float unit_scale, bool system_value);

    /// @brief Задать допустимый диапазон сырых значений.
    /// @param min_raw Минимальное значение.
    /// @param max_raw Максимальное значение.
    void set_raw_range(int32_t min_raw, int32_t max_raw);

    /// @brief Основной адрес параметра.
    uint16_t address() const override;

    /// @brief Является ли параметр системным (уставкой).
    bool is_system_value() const override;

    /// @brief Применить новое значение и при необходимости вызвать транслятор.
    bool apply_update(uint16_t address, int32_t raw_value) override;

    /// @brief Текущее значение (сырое).
    int32_t current_raw() const override;

    /// @brief Целевое значение (сырое).
    int32_t target_raw() const override;

    /// @brief Зафиксировать целевое значение как текущее.
    void commit_target() override;

    /// @brief Текущее значение в физических единицах.
    float current_units() const override;

    /// @brief Целевое значение в физических единицах.
    float target_units() const override;

    /// @brief Имя параметра (для диагностики).
    /// @return Имя параметра.
    const char* name() const { return _name; }

private:
    uint16_t _address;    ///< Адрес параметра.
    const char* _name;    ///< Имя параметра.
    float _unit_scale;    ///< Множитель в физические единицы.
    bool _system_value;   ///< Признак системного параметра.
    int32_t _current_raw; ///< Текущее сырое значение.
    int32_t _target_raw;  ///< Целевое сырое значение.
    int32_t _min_raw;     ///< Минимальное сырое значение.
    int32_t _max_raw;     ///< Максимальное сырое значение.
};

#endif
