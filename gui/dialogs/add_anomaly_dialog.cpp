//
// Created by Widok on 6/1/24.
//

#include <QDialogButtonBox>
#include <QFileDialog>
#include "add_anomaly_dialog.h"


namespace GUI::Dialogs {
    AddAnomalyDialog::AddAnomalyDialog(QWidget *parent) : QDialog(parent) {
        setupUI();
    }

    void AddAnomalyDialog::setupUI() {
        imagePathEdit = new QLineEdit(this);
        auto *browseButton = new QPushButton("Browse...", this);
        messageEdit = new QLineEdit(this);
        percentageSpin = new QSpinBox(this);
        percentageSpin->setRange(0, 100);

        auto *layout = new QFormLayout(this);
        auto *pathLayout = new QHBoxLayout();
        pathLayout->addWidget(imagePathEdit);
        pathLayout->addWidget(browseButton);
        layout->addRow(new QLabel("Image Path:"), pathLayout);
        layout->addRow(new QLabel("Message:"), messageEdit);
        layout->addRow(new QLabel("Percentage:"), percentageSpin);

        auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
        layout->addWidget(buttons);

        connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
        connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
        connect(browseButton, &QPushButton::clicked, this,
                &AddAnomalyDialog::openFileDialog);  // Connect the Browse button to slot
    }

    void AddAnomalyDialog::openFileDialog() {
        QString file = QFileDialog::getOpenFileName(this, tr("Select Image"), "",
                                                    tr("Image Files (*.png *.jpg *.jpeg *.bmp)"));
        if (!file.isEmpty()) {
            QString localDirPath = "anomalies";
            QDir localDir(localDirPath);

            if (!localDir.exists()) {
                localDir.mkpath(".");
            }

            QFileInfo fileInfo(file);
            QString filename = fileInfo.fileName();
            QString newFilePath = localDirPath + "/" + filename;

            QFile::copy(file, newFilePath);

            imagePathEdit->setText("anomalies/" + filename);
        }
    }


    QString AddAnomalyDialog::getImagePath() const {
        return imagePathEdit->text();
    }

    QString AddAnomalyDialog::getMessage() const {
        return messageEdit->text();
    }

    int AddAnomalyDialog::getPercentage() const {
        return percentageSpin->value();
    }
} // Dialogs
// GUI