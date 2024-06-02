//
// Created by Widok on 6/1/24.
//

#include "anomaly_manager.h"
#include <QHeaderView>
#include <QScrollBar>

namespace GUI {
    AnomalyManager::AnomalyManager(QTableWidget *table) : table(table) {
    }


    void AnomalyManager::setupTable() {
        table->clear();
        table->setRowCount(0);
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

        QStringList headers = {"Image", "Message", "Percent"};
        for (int i = 0; i < headers.size(); ++i) {
            auto *headerItem = new QTableWidgetItem(headers[i]);
            headerItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            table->setHorizontalHeaderItem(i, headerItem);
        }
    }


    void AnomalyManager::addRow(const std::string &image_path, const std::string &message, const int percentage) {
        int row = table->currentRow() + 1;
        if (row == 0) row = table->rowCount();
        table->insertRow(row);
        auto *path = new QTableWidgetItem(QString::fromStdString(image_path));
        path->setFlags(path->flags() ^ Qt::ItemIsEditable);
        path->setToolTip(QString::fromStdString(image_path));

        auto *msg = new QTableWidgetItem(QString::fromStdString(message));
        msg->setFlags(msg->flags() ^ Qt::ItemIsEditable);
        path->setToolTip(QString::fromStdString(message));

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

    std::vector<Anomaly> AnomalyManager::extractAnomalies() {
        std::vector<Anomaly> data;
        for (int i = 0; i < table->rowCount(); ++i) {
            QTableWidgetItem* itemPath = table->item(i, 0);
            QTableWidgetItem* itemMessage = table->item(i, 1);
            QTableWidgetItem* itemPercentage = table->item(i, 2);

            if (itemPath && itemMessage && itemPercentage) {
                std::string image_path = itemPath->text().toStdString();
                std::string message = itemMessage->text().toStdString();
                data.emplace_back(Anomaly{image_path, message, itemPercentage->text().toInt()});
            }
        }
        return data;
    }

    void AnomalyManager::addRow(const Anomaly &anomaly) {
        addRow(anomaly.template_image, anomaly.message, anomaly.coefficient);
    }


} // GUI