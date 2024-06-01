//
// Created by Widok on 6/1/24.
//

#ifndef INJECTIONCLICKER_ANOMALY_MANAGER_H
#define INJECTIONCLICKER_ANOMALY_MANAGER_H

#include <QObject>
#include <QTableWidget>
#include <variant>
#include "clicker_data.h"

namespace GUI {
    class AnomalyManager : public QObject {
    Q_OBJECT
    public:
        explicit AnomalyManager(QTableWidget *table);

        void setupTable();

        void addRow(const std::string& image_path, const std::string& message, const int percentage);

        void deleteSelectedRow() noexcept;

    signals:
        void dataChanged();

    private:
        QTableWidget *table{};
    };

} // GUI

#endif //INJECTIONCLICKER_ANOMALY_MANAGER_H
