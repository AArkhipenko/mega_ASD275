/*
 * parameter_registry — реализация реестра: регистрация хранилищ и поиск по адресу.
 */

#include "parameter_registry.h"
#include "parameter_storage.h"

parameter_registry::parameter_registry()
    : _size(0) {
    for (uint8_t i = 0; i < CAPACITY; i++) {
        _items[i] = 0;
    }
}

void parameter_registry::register_storage(parameter_storage& storage) {
    if (_size < CAPACITY) {
        _items[_size++] = &storage;
    }
}

parameter_storage* parameter_registry::find(uint16_t address) {
    for (uint8_t i = 0; i < _size; i++) {
        if (_items[i]->owns(address)) {
            return _items[i];
        }
    }
    return 0;
}

uint8_t parameter_registry::count() const {
    return _size;
}

parameter_storage* parameter_registry::storage_at(uint8_t index) const {
    if (index >= _size) {
        return 0;
    }
    return _items[index];
}
