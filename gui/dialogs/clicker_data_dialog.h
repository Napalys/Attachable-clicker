#ifndef INJECTIONCLICKER_CLICKER_DATA_DIALOG_H
#define INJECTIONCLICKER_CLICKER_DATA_DIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include "clicker_data.h"

namespace GUI::Dialogs {

    class ClickerDataDialog : public QDialog {
    Q_OBJECT

    public:
        explicit ClickerDataDialog(QWidget *parent = nullptr) : QDialog(parent) {
            setupUi();
            connectSignals();
        }

        [[nodiscard]] inline ClickerData getClickerData() const {
            return {static_cast<uint8_t>(keyCodeEdit->text().toInt()),
                    static_cast<uint32_t>(delayEdit->value()),
                    static_cast<ClickerData::Event>(eventComboBox->currentIndex()),
                    keyNameEdit->text().toStdString()};
        }

    private:
        void setupUi();

        inline void connectSignals() const noexcept {
            connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
            connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
        }

    private:
        QLineEdit *keyNameEdit{};
        QSpinBox *keyCodeEdit{};
        QSpinBox *delayEdit{};
        QComboBox *eventComboBox{};
        QDialogButtonBox *buttonBox{};
    };

}

#endif //INJECTIONCLICKER_CLICKER_DATA_DIALOG_H
