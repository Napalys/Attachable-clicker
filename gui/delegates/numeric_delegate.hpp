//
// Created by Widok on 5/22/24.
//

#ifndef INJECTIONCLICKER_NUMERIC_DELEGATE_HPP
#define INJECTIONCLICKER_NUMERIC_DELEGATE_HPP


#include <QItemDelegate>

namespace GUI::Delegates {

    class NumericDelegate : public QItemDelegate {
    public:
        explicit NumericDelegate(QObject *parent = nullptr) : QItemDelegate(parent) {}

        inline QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &,
                              const QModelIndex &) const override {
            auto *editor = new QLineEdit(parent);
            editor->setValidator(new QIntValidator(0, 100000, editor));
            return editor;
        }
    };

}

#endif //INJECTIONCLICKER_NUMERIC_DELEGATE_HPP
