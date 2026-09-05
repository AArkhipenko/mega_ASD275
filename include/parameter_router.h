/*
 * parameter_router — диспетчер обновлений.
 * Читает события data_source и доставляет их в хранилища параметров по адресу.
 */

#ifndef PARAMETER_ROUTER_H
#define PARAMETER_ROUTER_H

class data_source;
class parameter_registry;

/// @brief Диспетчер обновлений от источника данных к хранилищам параметров.
class parameter_router {
public:
    /// @brief Конструктор.
    /// @param source Источник данных.
    /// @param registry Реестр хранилищ параметров.
    parameter_router(data_source& source, parameter_registry& registry);

    /// @brief Прочитать одно обновление из источника и доставить его в хранилище.
    /// @return true, если обновление обработано.
    bool poll();

private:
    data_source& _source;         ///< Источник данных.
    parameter_registry& _registry; ///< Реестр хранилищ параметров.
};

#endif
