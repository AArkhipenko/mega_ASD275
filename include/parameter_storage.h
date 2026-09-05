/*
 * parameter_storage — интерфейс хранилища параметра устройства.
 * Хранит текущее и целевое значения, применяет обновление и вызывает parameter_translator.
 */

#ifndef PARAMETER_STORAGE_H
#define PARAMETER_STORAGE_H

#include <stdint.h>

class parameter_translator;

/// @brief Интерфейс хранилища параметра (или группы параметров) устройства.
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

    /// @brief Основной адрес параметра (для диагностики и регистрации).
    /// @return Адрес параметра.
    virtual uint16_t address() const = 0;

    /// @brief Принимает ли хранилище обновление по данному адресу.
    /// @param address Адрес из источника данных.
    /// @return true, если адрес относится к этому хранилищу.
    virtual bool owns(uint16_t address) const { return this->address() == address; }

    /// @brief Количество адресов, которые обслуживает хранилище.
    /// @return Количество адресов.
    virtual uint8_t address_count() const { return 1; }

    /// @brief Адрес по индексу (для хранилищ с несколькими адресами).
    /// @param index Индекс адреса.
    /// @return Адрес параметра.
    virtual uint16_t address_at(uint8_t index) const { (void)index; return address(); }

    /// @brief Является ли параметр системным (уставкой, порождающей движение).
    /// @return true для системного параметра.
    virtual bool is_system_value() const = 0;

    /// @brief Применить новое значение параметра.
    /// @param address Адрес параметра (для хранилищ, объединяющих несколько адресов).
    /// @param raw_value Новое сырое значение.
    /// @return true, если значение принято (и/или запущено движение).
    virtual bool apply_update(uint16_t address, int32_t raw_value) = 0;

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
