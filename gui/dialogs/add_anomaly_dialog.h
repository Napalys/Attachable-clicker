//
// Created by Widok on 6/1/24.
//

#ifndef INJECTIONCLICKER_ADD_ANOMALY_DIALOG_H
#define INJECTIONCLICKER_ADD_ANOMALY_DIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QFormLayout>
#include <QLabel>

namespace GUI::Dialogs {

    class AddAnomalyDialog : public QDialog {
    Q_OBJECT

    public:
        explicit AddAnomalyDialog(QWidget *parent = nullptr);

        [[nodiscard]] QString getImagePath() const;

        [[nodiscard]] QString getMessage() const;

        [[nodiscard]] int getPercentage() const;

        void openFileDialog();

    private:
        QLineEdit *imagePathEdit{};
        QLineEdit *messageEdit{};
        QSpinBox *percentageSpin{};

        void setupUI();
    };

} // Dialogs
// GUI

#endif //INJECTIONCLICKER_ADD_ANOMALY_DIALOG_H
