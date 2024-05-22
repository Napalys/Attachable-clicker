//
// Created by Widok on 5/22/24.
//

#ifndef INJECTIONCLICKER_ACTION_DELEGATE_HPP
#define INJECTIONCLICKER_ACTION_DELEGATE_HPP

namespace GUI::Delegates {

    class ActionDelegate : public QStyledItemDelegate {
    public:
        explicit ActionDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

        inline QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                              const QModelIndex &index) const override {
            Q_UNUSED(option);
            Q_UNUSED(index);
            auto *editor = new QComboBox(parent);
            editor->addItem("Pressed");
            editor->addItem("Released");
            return editor;
        }

        inline void setEditorData(QWidget *editor, const QModelIndex &index) const override {
            QString value = index.model()->data(index, Qt::EditRole).toString();
            auto *comboBox = dynamic_cast<QComboBox *>(editor);
            comboBox->setCurrentIndex(comboBox->findText(value));
        }

        inline void setModelData(QWidget *editor, QAbstractItemModel *model,
                          const QModelIndex &index) const override {
            auto *comboBox = dynamic_cast<QComboBox *>(editor);
            model->setData(index, comboBox->currentText(), Qt::EditRole);
        }

        inline void updateEditorGeometry(QWidget *editor,
                                  const QStyleOptionViewItem &option, const QModelIndex &index) const override {
            Q_UNUSED(index);
            editor->setGeometry(option.rect);
        }
    };
}

#endif //INJECTIONCLICKER_ACTION_DELEGATE_HPP
