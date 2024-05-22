#include "clicker_data_dialog.h"


namespace GUI::Dialogs {
    void ClickerDataDialog::setupUi() {
        keyCodeEdit = new QSpinBox(this);
        keyCodeEdit->setRange(0, 255);

        delayEdit = new QSpinBox(this);
        delayEdit->setRange(0, 100000);

        keyNameEdit = new QLineEdit(this);

        eventComboBox = new QComboBox(this);
        eventComboBox->addItem("Pressed", QVariant(static_cast<int>(ClickerData::Event::Pressed)));
        eventComboBox->addItem("Released", QVariant(static_cast<int>(ClickerData::Event::Released)));
        eventComboBox->addItem("Unknown", QVariant(static_cast<int>(ClickerData::Event::Unknown)));

        buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

        auto *layout = new QFormLayout(this);
        layout->addRow("Key Code:", keyCodeEdit);
        layout->addRow("Delay (ms):", delayEdit);
        layout->addRow("Event:", eventComboBox);
        layout->addRow("Key Name:", keyNameEdit);
        layout->addWidget(buttonBox);

        setLayout(layout);
        setWindowTitle("Add Clicker Data");
    }
}