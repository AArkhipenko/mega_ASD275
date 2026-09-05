/*
 * parameter_registry — реестр параметров.
 * Сопоставляет адрес параметра с его хранилищем (parameter_storage).
 */

#ifndef PARAMETER_REGISTRY_H
#define PARAMETER_REGISTRY_H

#include <stdint.h>

class parameter_storage;

/// @brief Реестр хранилищ параметров: поиск по адресу и перечисление.
class parameter_registry {
public:
    /// @brief Конструктор.
    parameter_registry();

    /// @brief Зарегистрировать хранилище параметра.
    /// @param storage Ссылка на хранилище.
    void register_storage(parameter_storage& storage);

    /// @brief Найти хранилище, принимающее данный адрес.
    /// @param address Адрес параметра.
    /// @return Указатель на хранилище или null.
    parameter_storage* find(uint16_t address);

    /// @brief Количество зарегистрированных хранилищ.
    /// @return Количество хранилищ.
    uint8_t count() const;

    /// @brief Хранилище по индексу регистрации.
    /// @param index Индекс хранилища.
    /// @return Указатель на хранилище (null для недопустимого индекса).
    parameter_storage* storage_at(uint8_t index) const;

private:
    static const uint8_t CAPACITY = 16;
    parameter_storage* _items[CAPACITY]; ///< Массив зарегистрированных хранилищ.
    uint8_t _size;                       ///< Текущее количество хранилищ.
};

#endif
