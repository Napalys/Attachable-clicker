//
// Created by Widok on 6/1/24.
//

#include "anomaly_manager.h"
#include <QHeaderView>

namespace GUI {
    AnomalyManager::AnomalyManager(QTableWidget *table) : table(table) {
    }


    void AnomalyManager::setupTable() {
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        table->setSelectionMode(QAbstractItemView::SingleSelection);
        table->setSelectionBehavior(QAbstractItemView::SelectRows);
        table->verticalHeader()->hide();
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);

        const int totalWidth = table->width();
        table->setColumnWidth(0, static_cast<int>(totalWidth * 0.5));
        table->setColumnWidth(1, static_cast<int>(totalWidth * 0.4));
        table->setColumnWidth(2, static_cast<int>(totalWidth * 0.1));
        table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    }


    void AnomalyManager::addRow(const std::string &image_path, const std::string &message, const int percentage) {
        int row = table->currentRow() + 1;
        if (row == 0) row = table->rowCount();
        table->insertRow(row);
        auto *path = new QTableWidgetItem(QString::fromStdString(image_path));
        path->setFlags(path->flags() ^ Qt::ItemIsEditable);

        auto *msg = new QTableWidgetItem(QString::fromStdString(message));
        msg->setFlags(msg->flags() ^ Qt::ItemIsEditable);

        auto *percent = new QTableWidgetItem(QString::number(percentage));
        percent->setFlags(percent->flags() ^ Qt::ItemIsEditable);
        percent->setTextAlignment(Qt::AlignCenter);

        table->setItem(row, 0, path);
        table->setItem(row, 1, msg);
        table->setItem(row, 2, percent);
    }

    void AnomalyManager::deleteSelectedRow() noexcept {
        QModelIndex currentIndex = table->selectionModel()->currentIndex();
        if (!currentIndex.isValid()) return;
        table->removeRow(currentIndex.row());
        emit dataChanged();
    }
} // GUI