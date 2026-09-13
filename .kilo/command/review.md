---
description: "Этап 3 — ревью: проверка написанного кода"
agent: review
---
Выполни этап «Ревью» для проекта mega_ASD275.

$ARGUMENTS

## В рамках сессии (если передан id сессии)

Если первый аргумент — это id существующей сессии (папка `docs/sessions/<id>`), работай в рамках сессии:
- прочитай `docs/sessions/<id>/session.md`, `docs/sessions/<id>/review-brief.md` и артефакты из `docs/sessions/<id>/artifacts/`;
- отчёт сохрани в `docs/sessions/<id>/reviews/<id>-review.md`;
- по завершении обнови статус и вердикт в `docs/sessions/<id>/session.md`.

## Разово (без сессии)

Если id сессии не передан: проверь код по артефактам из `docs/artifacts/`, запусти `pio run`, отчёт в `docs/reviews/`.

В обоих случаях: код не исправлять — только вердикт и список замечаний.