/*
 * axis_settings — хранилище системных переменных оси (настройки драйвера).
 * Объединяет скорость вращения и масштаб «оборотов на градус» в одну сущность:
 * обновления приходят с разных адресов VP, но хранятся и применяются совместно.
 */

#ifndef AXIS_SETTINGS_H
#define AXIS_SETTINGS_H

#include <stdint.h>
#include "parameter_storage.h"

/// @brief Хранилище настроек оси (системных переменных драйвера).
class axis_settings : public parameter_storage {
public:
    /// @brief Конструктор.
    /// @param speed_address Адрес VP скорости вращения (об/мин).
    /// @param revs_address Адрес VP масштаба «оборотов на градус».
    axis_settings(uint16_t speed_address, uint16_t revs_address);

    /// @brief Основной адрес (адрес скорости).
    uint16_t address() const override;

    /// @brief Принимает адреса скорости и масштаба.
    bool owns(uint16_t address) const override;

    /// @brief Количество обслуживаемых адресов (два).
    uint8_t address_count() const override { return 2; }

    /// @brief Адрес по индексу: 0 — скорость, 1 — масштаб.
    uint16_t address_at(uint8_t index) const override;

    /// @brief Настройки не являются уставкой движения.
    bool is_system_value() const override { return false; }

    /// @brief Применить значение одной из настроек.
    bool apply_update(uint16_t address, int32_t raw_value) override;

    int32_t current_raw() const override { return _rpm_raw; }
    int32_t target_raw() const override { return _rpm_raw; }
    void commit_target() override {}
    float current_units() const override { return (float)_rpm_raw; }
    float target_units() const override { return (float)_rpm_raw; }

    /// @brief Скорость вращения оси, об/мин.
    /// @return Скорость в об/мин.
    uint16_t rpm() const { return (uint16_t)_rpm_raw; }

    /// @brief Масштаб «оборотов на градус».
    /// @return Количество оборотов привода на один градус.
    float revs_per_degree() const { return (float)_revs_raw * REVS_PER_DEG_SCALE; }

private:
    static const float REVS_PER_DEG_SCALE; ///< Множитель сырого масштаба в «оборотов/градус».

    uint16_t _speed_address; ///< Адрес VP скорости.
    uint16_t _revs_address;  ///< Адрес VP масштаба.
    int32_t _rpm_raw;        ///< Сырое значение скорости.
    int32_t _revs_raw;       ///< Сырое значение масштаба.
};

#endif
