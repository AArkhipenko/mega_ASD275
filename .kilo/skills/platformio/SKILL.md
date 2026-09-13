---
name: platformio
description: "PlatformIO и расширение: сборка, заливка, монитор, структура проекта, конфиг platformio.ini, симуляторы. Использовать при любой работе с проектом mega_ASD275."
---

# PlatformIO — сборка и работа с проектом

Проект `mega_ASD275` — прошивка Arduino, собранная через PlatformIO. Работай с ней только через PlatformIO, а не через `avr-gcc` напрямую.

## Основные команды

| Команда | Назначение |
|---|---|
| `pio run` | Сборка прошивки. **Обязательна после любых изменений кода.** |
| `pio run -t upload` | Сборка и заливка в МК (порт `upload_port = COM3`). |
| `pio device monitor` | Serial-монитор (9600 бод) — отладка и ввод команд симулятора DWIN. |

## Структура проекта

```
platformio.ini   — конфигурация сборки (там указаны платформа и плата)
src/main.cpp     — главный файл: setup() и loop(), композиция модулей (обязателен)
include/         — интерфейсы и хранилища проекта (подключаются флагом -Iinclude)
lib/<name>/      — библиотека проекта:
  library.json   — метаданные библиотеки
  src/*.h, src/*.cpp — исходники библиотеки
test/            — тесты (если есть)
```

- Файлы в `src/` и `lib/*/src/` компилируются автоматически; ничего не нужно добавлять в Makefile.
- `include/` подключается флагом `-Iinclude` (см. `build_flags`).

## Конфиг platformio.ini (кратко)

```ini
[env:megaatmega2560]
platform = atmelavr
board = megaatmega2560
board_build.mcu = atmega2560
board_build.f_cpu = 16000000L
framework = arduino
monitor_speed = 9600
upload_port = COM3
build_flags =
    -Wall
    -Wextra
    -Iinclude
    -DUSE_DWIN_LCM_SIMULATOR   ; симулятор DWIN (Serial Monitor)
    ;-DUSE_SERVO_SIMULATOR      ; симулятор сервопривода
```

## Симуляторы

- `USE_DWIN_LCM_SIMULATOR` — вместо реального дисплея параметры вводятся в Serial Monitor парой `адрес значение` (например `0x5002 4500`).
- `USE_SERVO_SIMULATOR` — имитация сервопривода вместо реального ASD275 (для отладки логики без железа).
- Сборка выбирается флагами из `build_flags`; не дублируй симуляторы в коде через `#ifdef` без необходимости.

## Правила

- Финальная проверка изменений — **успешный `pio run` без новых предупреждений** при `-Wall -Wextra`.
- Команды запускай из корня проекта (там лежит `platformio.ini`).
- Не меняй `platformio.ini` без задачи: порты, скорости и флаги симуляторов влияют на железо.