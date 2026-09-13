---
description: "Этап 1 — аналитика: подготовка артефактов для разработки"
agent: analytics
---
Выполни этап «Аналитика» для проекта mega_ASD275.

$ARGUMENTS

## В рамках сессии (если передан id сессии)

Если первый аргумент — это id существующей сессии (папка `docs/sessions/<id>`), работай в рамках сессии:
- прочитай `docs/sessions/<id>/session.md` и `docs/sessions/<id>/analytics-brief.md`;
- артефакты создавай в `docs/sessions/<id>/artifacts/` (файлы `<id>-requirements.md`, `<id>-design.md`, `<id>-tasks.md`, `<id>-acceptance.md`);
- по завершении обнови статус в `docs/sessions/<id>/session.md`.

## Разово (без сессии)

Если id сессии не передан: подготовь артефакты в `docs/artifacts/` (requirements, design, tasks, acceptance) по регламенту аналитика.

В обоих случаях: код не пиши (`src/`, `include/`, `lib/` не трогать).