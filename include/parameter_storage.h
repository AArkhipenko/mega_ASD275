/*
 * parameter_storage — интерфейс хранилища одного параметра устройства.
 * Хранит текущее и целевое значения, применяет обновление и вызывает parameter_translator.
 */

#ifndef PARAMETER_STORAGE_H
#define PARAMETER_STORAGE_H

#include <stdint.h>

class parameter_translator;

/// @brief Интерфейс хранилища одного параметра устройства.
class parameter_storage {
public:
    /// @brief Конструктор по умолчанию.
    parameter_storage() : _translator(0) {}

    /// @brief Виртуальный деструктор.
    virtual ~parameter_storage() {}

    /// @brief Привязать транслятор, вызываемый при изменении параметра.
    /// @param translator Указатель на транслятор (может быть null).
    void attach_translator(parameter_translator* translator) { _translator = translator; }

    /// @brief Получить привязанный транслятор.
    /// @return Указатель на транслятор или null.
    parameter_translator* translator() { return _translator; }

    /// @brief Адрес параметра.
    /// @return Адрес параметра.
    virtual uint16_t address() const = 0;

    /// @brief Является ли параметр системным (уставкой, порождающей движение).
    /// @return true для системного параметра.
    virtual bool is_system_value() const = 0;

    /// @brief Применить новое значение параметра.
    /// @param raw_value Новое сырое значение.
    /// @return true, если значение принято (и/или запущено движение).
    virtual bool apply_update(int32_t raw_value) = 0;

    /// @brief Текущее значение параметра (сырое).
    /// @return Текущее сырое значение.
    virtual int32_t current_raw() const = 0;

    /// @brief Целевое значение параметра (сырое).
    /// @return Целевое сырое значение.
    virtual int32_t target_raw() const = 0;

    /// @brief Зафиксировать целевое значение как текущее (после завершения движения).
    virtual void commit_target() = 0;

    /// @brief Текущее значение в физических единицах.
    /// @return Текущее значение в физических единицах.
    virtual float current_units() const = 0;

    /// @brief Целевое значение в физических единицах.
    /// @return Целевое значение в физических единицах.
    virtual float target_units() const = 0;

protected:
    parameter_translator* _translator; ///< Привязанный транслятор.
};

#endif
