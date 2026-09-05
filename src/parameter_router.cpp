/*
 * parameter_router — реализация диспетчера:
 * чтение обновлений data_source, поиск хранилища по адресу и вызов apply_update.
 */

#include <Arduino.h>
#include "parameter_router.h"
#include "data_source.h"
#include "parameter_registry.h"
#include "parameter_storage.h"

parameter_router::parameter_router(data_source& source, parameter_registry& registry)
    : _source(source),
      _registry(registry) {}

bool parameter_router::poll() {
    parameter_update update;
    if (!_source.try_read_update(update)) {
        return false;
    }

    Serial.print(F("[ROUTER] 0x"));
    Serial.print(update.address, HEX);
    Serial.print(F(" = "));
    Serial.println(update.raw_value);

    parameter_storage* storage = _registry.find(update.address);
    if (storage == 0) {
        Serial.println(F("[ROUTER] Неизвестный адрес"));
        return true;
    }

    storage->apply_update(update.address, update.raw_value);
    return true;
}
