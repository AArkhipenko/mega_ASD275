/*
 * axis_motion_translator — преобразователь «дельта параметра -> движение привода».
 * Рассчитывает обороты и скорость, запускает driver_interface и контролирует завершение движения.
 * Настройки оси (скорость и масштаб) кэшируются и обновляются только при их изменении.
 */

#ifndef AXIS_MOTION_TRANSLATOR_H
#define AXIS_MOTION_TRANSLATOR_H

#include "parameter_translator.h"

class parameter_storage;
class driver_interface;
class axis_settings;

/// @brief Преобразователь дельты параметра в движение привода.
class axis_motion_translator : public parameter_translator {
public:
    /// @brief Конструктор.
    /// @param storage Хранилище управляемого параметра (оси).
    /// @param driver Драйвер привода.
    /// @param settings Хранилище настроек оси (скорость и масштаб).
    axis_motion_translator(parameter_storage& storage,
                           driver_interface& driver,
                           axis_settings& settings);

    /// @brief Обработать изменение хранилища и запустить движение.
    bool on_storage_changed(parameter_storage& storage) override;

    /// @brief Обновить кэш настроек при их изменении.
    void on_settings_changed(parameter_storage& storage) override;

    /// @brief Опрос состояния движения, завершение и контроль таймаута.
    void tick() override;

    /// @brief Выполняется ли движение в данный момент.
    bool is_busy() const override;

private:
    /// @brief Прочитать настройки из хранилища в локальный кэш.
    void refresh_settings();

    parameter_storage& _storage;          ///< Хранилище параметра оси.
    driver_interface& _driver;            ///< Драйвер привода.
    axis_settings& _settings;             ///< Настройки оси.
    float _rpm_cache;                     ///< Кэш скорости, об/мин.
    float _revs_per_unit_cache;           ///< Кэш масштаба «оборотов на единицу».
    bool _busy;                           ///< Флаг активного движения.
    bool _motion_observed;                ///< Флаг подтверждения фактического движения.
    unsigned long _start_ms;              ///< Время запуска движения.
};

#endif
