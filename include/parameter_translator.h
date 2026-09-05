/*
 * parameter_translator — интерфейс преобразования «параметр -> команды управления драйвером».
 * Вызывается хранилищем при изменении, опрашивается главным циклом через tick().
 */

#ifndef PARAMETER_TRANSLATOR_H
#define PARAMETER_TRANSLATOR_H

class parameter_storage;

/// @brief Интерфейс преобразования «параметр -> команды управления драйвером».
class parameter_translator {
public:
    /// @brief Виртуальный деструктор.
    virtual ~parameter_translator() {}

    /// @brief Обработать изменение хранилища и сформировать команды драйверу.
    /// @param storage Изменившееся хранилище.
    /// @return true, если изменение принято (запущено движение).
    virtual bool on_storage_changed(parameter_storage& storage) = 0;

    /// @brief Уведомление об изменении настроек драйвера (системных переменных).
    /// @param storage Хранилище настроек, изменившее значение.
    virtual void on_settings_changed(parameter_storage& storage) { (void)storage; }

    /// @brief Периодический опрос состояния движения (вызывается главным циклом).
    virtual void tick() = 0;

    /// @brief Выполняется ли в данный момент движение.
    /// @return true, если транслятор занят движением.
    virtual bool is_busy() const = 0;
};

#endif
