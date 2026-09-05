/*
 * data_source — интерфейс источника данных.
 * Отдаёт события обновления параметра: адрес и сырое значение (parameter_update).
 */

#ifndef DATA_SOURCE_H
#define DATA_SOURCE_H

#include <stdint.h>

/// @brief Событие обновления параметра от источника данных.
struct parameter_update {
    uint16_t address;   ///< Адрес параметра.
    int32_t raw_value;  ///< Сырое значение параметра.
};

/// @brief Интерфейс источника данных.
class data_source {
public:
    /// @brief Виртуальный деструктор.
    virtual ~data_source() {}

    /// @brief Неблокирующее чтение очередного обновления параметра.
    /// @param out Ссылка для сохранения события.
    /// @return true, если получено новое обновление.
    virtual bool try_read_update(parameter_update& out) = 0;
};

#endif
