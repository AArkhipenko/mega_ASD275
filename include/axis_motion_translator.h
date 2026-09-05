/*
 * axis_motion_translator — преобразователь «дельта параметра -> движение привода».
 * Рассчитывает обороты и скорость, запускает driver_interface и контролирует завершение движения.
 */

#ifndef AXIS_MOTION_TRANSLATOR_H
#define AXIS_MOTION_TRANSLATOR_H

#include "parameter_translator.h"

class parameter_storage;
class driver_interface;

/// @brief Преобразователь дельты параметра в движение привода.
class axis_motion_translator : public parameter_translator {
public:
    /// @brief Конструктор.
    /// @param storage Хранилище управляемого параметра (оси).
    /// @param driver Драйвер привода.
    /// @param rpm_provider Хранилище скорости вращения (об/мин).
    /// @param revs_per_unit_provider Хранилище масштаба «оборотов на единицу параметра».
    axis_motion_translator(parameter_storage& storage,
                           driver_interface& driver,
                           const parameter_storage* rpm_provider,
                           const parameter_storage* revs_per_unit_provider);

    /// @brief Обработать изменение хранилища и запустить движение.
    bool on_storage_changed(parameter_storage& storage) override;

    /// @brief Опрос состояния движения, завершение и контроль таймаута.
    void tick() override;

    /// @brief Выполняется ли движение в данный момент.
    bool is_busy() const override;

private:
    parameter_storage& _storage;          ///< Хранилище параметра оси.
    driver_interface& _driver;            ///< Драйвер привода.
    const parameter_storage* _rpm_provider;          ///< Источник скорости.
    const parameter_storage* _revs_per_unit;         ///< Источник масштаба.
    bool _busy;                           ///< Флаг активного движения.
    bool _motion_observed;                ///< Флаг подтверждения фактического движения.
    unsigned long _start_ms;              ///< Время запуска движения.
};

#endif
