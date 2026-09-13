# Документация проекта mega_ASD275

Документация описывает **прошивку** проекта mega_ASD275 — целевое устройство на Arduino Mega 2560 (управление сервоприводом ASD275 по уставкам с дисплея DWIN). Набор общий для проекта, не привязан ни к одной сессии. Основной источник — артефакты аналитика (проектирование целевой архитектуры); файлы кода используются как справочный контекст для сопоставления имён и сигнатур. Если этап разработки ещё не выполнялся, описания отражают спроектированную архитектуру и контракты (раздел 8 дизайна), а не текущее состояние кода.

## Файлы набора

| Файл | Содержимое |
|---|---|
| [overview.md](./overview.md) | Обзор системы: назначение, железо и подключения, глобальный поток данных, режимы отладки |
| [modules.md](./modules.md) | Карта модулей целевой архитектуры и связи между ними (схема mermaid) |
| [flow.md](./flow.md) | Полная цепочка данных DWIN → МК → ASD275, буферы A/B/C, неблокирующий главный цикл |
| [principles.md](./principles.md) | Принципы работы программы (из артефактов аналитика и решений владельца) |
| [features.md](./features.md) | Функциональность по разделам требований: категории, угол, скорость, RS485, приоритеты, симуляторы |
| [main.md](./main.md) | Разбор `src/main.cpp`: композиция модулей, `setup()`, `loop()`, флаги симуляторов |
| [module-data_source.md](./module-data_source.md) | `data_source` + `dwin_lcm` / `dwin_lcm_simulator` |
| [module-storage.md](./module-storage.md) | `parameter_storage`, `scaled_parameter`, `axis_settings` (многоадресные хранилища) |
| [module-translator.md](./module-translator.md) | `parameter_translator`, `axis_motion_translator`: дельта → команды, кэш настроек |
| [module-driver.md](./module-driver.md) | `driver_interface`, `servo_driver` (Modbus RTU), `servo_simulator` |
| [module-router-registry.md](./module-router-registry.md) | `parameter_router`, `parameter_registry`: маршрутизация по адресу |
| [module-addresses.md](./module-addresses.md) | `parameter_addresses.h`: таблица VP-адресов, масштабы, категории |

## Последняя генерация

- Дата и время: 2026-09-13.
- Генерируется командой `/docs` по правилам генератора документации (см. описание роли).

## Источники

### Артефакты аналитика (основной источник — целевая архитектура)

| Файл | Что даёт |
|---|---|
| [base-requirements.md](../sessions/base/artifacts/base-requirements.md) | Функциональные/нефункциональные требования FR-1..NFR-8, категории параметров, ограничения платформы, риски R-1..R-5 |
| [base-design.md](../sessions/base/artifacts/base-design.md) | Целевая архитектура, буферы A/B/C, главный цикл, приоритеты, решения владельца, файлы (раздел 7), сигнатуры API (раздел 8), принципы (раздел 10) |
| [base-tasks.md](../sessions/base/artifacts/base-tasks.md) | План задач T1..T10 этапа разработки, что отложено |
| [base-acceptance.md](../sessions/base/artifacts/base-acceptance.md) | Критерии приёмки AC-1..AC-10 и сценарии S1..S10 |
| [project.md](../sessions/base/project.md) | Компактная справка: цепочка вызовов, ключевые файлы, стиль |
| [README.md](../../README.md) | Целевая архитектура проекта, таблица подключения, таблица адресов, блок-схемы |

### Файлы кода (справочный контекст — только имена и сигнатуры)

Покрытые файлы (планируемые файлы целевой реализации по разделу 7 дизайна; текущее содержимое может не соответствовать целевой архитектуре):

- `include/data_source.h`, `include/parameter_storage.h`, `include/parameter_registry.h`, `include/parameter_router.h`, `include/parameter_translator.h`, `include/driver_interface.h`, `include/axis_motion_translator.h`, `include/axis_settings.h`, `include/scaled_parameter.h`, `include/parameter_addresses.h`
- `lib/dwin_lcm/*`, `lib/dwin_lcm_simulator/*`, `lib/servo_driver/*`, `lib/servo_simulator/*`
- `src/main.cpp` (+ `src/axis_motion_translator.cpp`, `src/axis_settings.cpp`, `src/parameter_registry.cpp`, `src/parameter_router.cpp`, `src/scaled_parameter.cpp`)
- `platformio.ini` (флаги симуляторов, порты, скорости)

## Как перегенерировать

1. Выполнить команду `/docs` из корня проекта.
2. Генератор прочитает код (`src/`, `include/`, `lib/`, `platformio.ini`) и артефакты аналитика из `docs/sessions/*/artifacts/` (или `docs/artifacts/`) для принципов работы, которые не видны в коде.
3. Весь набор файлов в `docs/generated/` перезаписывается целиком — устаревшее содержимое прошлых генераций не сохраняется.

## Пометки источников фактов

- **(из артефактов аналитика)** — факт из требований/дизайна/задач/приёмки, может не следовать из кода.
- **(из кода)** — факт, установленный по текущему коду, в артефактах не описан.
- **(справочно, из кода)** — имя/сигнатура существующего члена в текущем коде, приведённая для сопоставления с целевым контрактом.