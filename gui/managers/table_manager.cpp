#include "table_manager.h"
#include "delegates/non_editable_delegate.hpp"
#include "delegates/numeric_delegate.hpp"
#include "delegates/action_delegate.hpp"
#include <QStyledItemDelegate>
#include <QHeaderView>

namespace GUI {

    TableManager::TableManager(QTableWidget *table) : table(table) {
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        qRegisterMetaType<std::shared_ptr<ClickerData>>("std::shared_ptr<ClickerData>");
        qRegisterMetaType<std::shared_ptr<Delay>>("std::shared_ptr<Delay>");
        qRegisterMetaType<QItemSelection>();
    }

    void TableManager::setupTable() {
        table->clear();
        table->setRowCount(0);
        table->setColumnCount(4);
        QStringList headers = {"Name", "Key", "Delay ms", "Action"};
        table->setHorizontalHeaderLabels(headers);
        table->setSelectionMode(QAbstractItemView::SingleSelection);
        table->setSelectionBehavior(QAbstractItemView::SelectRows);

        table->setItemDelegateForColumn(0, new GUI::Delegates::NonEditableDelegate(table));
        table->setItemDelegateForColumn(1, new GUI::Delegates::NonEditableDelegate(table));
        table->setItemDelegateForColumn(2, new GUI::Delegates::NumericDelegate(table));
        table->setItemDelegateForColumn(3, new GUI::Delegates::ActionDelegate(table));

        table->verticalHeader()->hide();
        QObject::connect(table, &QTableWidget::itemChanged, [this](QTableWidgetItem *item) {
            int row = item->row();
            int column = item->column();

            QTableWidgetItem *firstColumnItem = table->item(row, 0);
            if (!firstColumnItem) return;

            QVariant userData = firstColumnItem->data(Qt::UserRole);

            if (userData.canConvert<std::shared_ptr<ClickerData>>()) {
                auto clickerData = qvariant_cast<std::shared_ptr<ClickerData>>(userData);
                if (column == 3) {
                    clickerData->event = (clickerData->event == ClickerData::Event::Pressed) ?
                                         ClickerData::Event::Released : ClickerData::Event::Pressed;
                }
            } else if (userData.canConvert<std::shared_ptr<Delay>>()) {
                auto delayData = qvariant_cast<std::shared_ptr<Delay>>(userData);
                if (column == 2) {
                    delayData->delay = item->text().toInt();
                }
            }
        });
    }

    struct PtrCreatorVisitor {
        QVariant operator()(const ClickerData &data) const {
            return QVariant::fromValue(std::make_shared<ClickerData>(data));
        }

        QVariant operator()(const Delay &data) const {
            return QVariant::fromValue(std::make_shared<Delay>(data));
        }
    };

    void TableManager::addRow(const std::variant<ClickerData, Delay> &data) {
        int row = table->currentRow() + 1;
        if (row == 0) row = table->rowCount();
        table->insertRow(row);

        std::visit([&](auto &&arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, ClickerData>) {
                // Handling ClickerData
                auto *name = new QTableWidgetItem(QString::fromStdString(arg.key_name));
                auto *keyItem = new QTableWidgetItem(QString::number(arg.key_code));
                auto *action = new QTableWidgetItem(arg.event == ClickerData::Event::Pressed ? "Pressed" : "Released");
                auto *placeholder = new QTableWidgetItem();  // Placeholder for the fourth column

                // Set items to the table
                table->setItem(row, 0, name);
                table->setItem(row, 1, keyItem);
                table->setItem(row, 2, placeholder);
                table->setItem(row, 3, action);

                // Gray out the unused fourth column
                placeholder->setFlags(placeholder->flags() ^ Qt::ItemIsEditable);
                placeholder->setBackground(QBrush(Qt::darkGray));
            } else if constexpr (std::is_same_v<T, Delay>) {
                // Handling Delay
                auto *name = new QTableWidgetItem("Delay");
                auto *delayTime = new QTableWidgetItem(QString::number(arg.delay));
                auto *placeholder1 = new QTableWidgetItem();
                auto *placeholder2 = new QTableWidgetItem();

                table->setItem(row, 0, name);
                table->setItem(row, 1, placeholder1);
                table->setItem(row, 2, delayTime);
                table->setItem(row, 3, placeholder2);

                placeholder1->setFlags(placeholder1->flags() ^ Qt::ItemIsEditable);
                placeholder1->setBackground(QBrush(Qt::darkGray));
                placeholder2->setFlags(placeholder2->flags() ^ Qt::ItemIsEditable);
                placeholder2->setBackground(QBrush(Qt::darkGray));
            }
        }, data);

        table->item(row, 0)->setData(Qt::UserRole, std::visit(PtrCreatorVisitor{}, data));
        table->setCurrentCell(row, 0);
        table->scrollToItem(table->item(row, 0));
        emit dataChanged();
    }

    std::vector<std::variant<ClickerData, Delay>> TableManager::extractAllData() const {
        std::vector<std::variant<ClickerData, Delay>> allData;
        for (int i = 0; i < table->rowCount(); ++i) {
            QVariant userData = table->item(i, 0)->data(Qt::UserRole);
            if (userData.canConvert<std::shared_ptr<ClickerData>>()) {
                auto clickerData = qvariant_cast<std::shared_ptr<ClickerData>>(userData);
                allData.emplace_back(*clickerData);
            } else if (userData.canConvert<std::shared_ptr<Delay>>()) {
                auto delayData = qvariant_cast<std::shared_ptr<Delay>>(userData);
                allData.emplace_back(*delayData);
            }
        }
        return allData;
    }

    bool TableManager::isEmpty() const noexcept {
        return table->rowCount() == 0;
    }

    void TableManager::deleteSelectedRow() noexcept {
        QModelIndex currentIndex = table->selectionModel()->currentIndex();
        if (!currentIndex.isValid()) return;
        table->removeRow(currentIndex.row());
        emit dataChanged();
    }
}