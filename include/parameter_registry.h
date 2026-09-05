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

    /// @brief Найти хранилище по адресу параметра.
    /// @param address Адрес параметра.
    /// @return Указатель на хранилище или null.
    parameter_storage* find(uint16_t address);

    /// @brief Количество зарегистрированных хранилищ.
    /// @return Количество хранилищ.
    uint8_t count() const;

    /// @brief Адрес параметра по индексу регистрации.
    /// @param index Индекс хранилища.
    /// @return Адрес параметра (0 для недопустимого индекса).
    uint16_t address_at(uint8_t index) const;

private:
    static const uint8_t CAPACITY = 16;
    parameter_storage* _items[CAPACITY]; ///< Массив зарегистрированных хранилищ.
    uint8_t _size;                       ///< Текущее количество хранилищ.
};

#endif
