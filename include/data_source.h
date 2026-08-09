#ifndef DATA_SOURCE_H
#define DATA_SOURCE_H

/**
 * @brief Интерфейс источника данных.
 * Абстрагирует получение нового значения из внешнего источника данных
 * (например, дисплея DWIN), скрывая детали протокола обмена.
 */
class data_source {
public:
    /// @brief Виртуальный деструктор.
    virtual ~data_source() = default;

    /**
     * @brief Считывает новое значение из источника данных.
     * @param out_value Ссылка на переменную для сохранения результата.
     * @return true, если получено новое значение; false при таймауте или отсутствии данных.
     */
    virtual bool try_read_value(float& out_value) = 0;
};

#endif // DATA_SOURCE_H
