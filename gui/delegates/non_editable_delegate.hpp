//
// Created by Widok on 5/22/24.
//

#ifndef INJECTIONCLICKER_NON_EDITABLE_DELEGATE_HPP
#define INJECTIONCLICKER_NON_EDITABLE_DELEGATE_HPP

#include <QItemDelegate>

namespace GUI::Delegates {

    class NonEditableDelegate : public QItemDelegate {
    public:
        explicit NonEditableDelegate(QObject *parent = nullptr) : QItemDelegate(parent) {}

        inline QWidget *createEditor(QWidget *, const QStyleOptionViewItem &, const QModelIndex &) const override {
            return nullptr;
        }
    };

}

#endif //INJECTIONCLICKER_NON_EDITABLE_DELEGATE_HPP
