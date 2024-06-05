#ifndef INJECTIONCLICKER_TABLE_MANAGER_H
#define INJECTIONCLICKER_TABLE_MANAGER_H

#include <QObject>
#include <QTableWidget>
#include <variant>
#include "clicker_data.h"

Q_DECLARE_METATYPE(std::shared_ptr<ClickerData>)

Q_DECLARE_METATYPE(std::shared_ptr<Delay>)

namespace GUI {
    class TableManager : public QObject {
    Q_OBJECT
    public:
        explicit TableManager(QTableWidget *table);

        void setupTable();

        void addRow(const std::variant<ClickerData, Delay> &data);

        [[nodiscard]] std::vector<std::variant<ClickerData, Delay>> extractAllData() const;

        [[nodiscard]] bool isEmpty() const noexcept;

        void deleteSelectedRow() noexcept;

    signals:
        void dataChanged();

    private:
        QTableWidget *table;
    };

} // GUI

#endif //INJECTIONCLICKER_TABLE_MANAGER_H
