---
description: "Генерация/обновление документации по коду (docs/generated): модули, функциональность, поток данных, связи с исходниками"
agent: docs
---
Сгенерируй (или перегенерируй) документацию по коду проекта mega_ASD275.

$ARGUMENTS

- Просканируй `src/`, `include/`, `lib/`, `platformio.ini` и прочитай исходники.
- Полностью обнови набор файлов в `docs/generated/`: `README.md` (индекс + дата генерации), `overview.md`, `modules.md`, `flow.md`, `module-data_source.md`, `module-storage.md`, `module-translator.md`, `module-driver.md`, `module-router-registry.md`, `module-addresses.md`, `main.md`, `features.md`.
- Содержимое — на русском, для человека; каждый факт со ссылкой на исходник (относительная ссылка на файл и `file:line`).
- Публичные API извлекай из фактического чтения файлов с номерами строк.
- Код не менять.

Если передан список файлов/модулей ($ARGUMENTS) — сначала обнови весь набор целиком, затем особое внимание удели указанным.