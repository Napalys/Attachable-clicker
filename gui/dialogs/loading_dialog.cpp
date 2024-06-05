//
// Created by Widok on 5/31/24.
//

#include <QVBoxLayout>
#include <QProgressBar>
#include <QLabel>
#include "loading_dialog.h"


namespace GUI::Dialogs {
    LoadingDialog::LoadingDialog(QWidget *parent) : QDialog(parent) {
        setWindowTitle(tr("Loading..."));
        setModal(true);
        auto *layout = new QVBoxLayout(this);

        auto *progressBar = new QProgressBar;
        progressBar->setMinimum(0);
        progressBar->setMaximum(0);

        layout->addWidget(new QLabel(tr("Please wait...")));
        layout->addWidget(progressBar);
        resize(200, 100);
    }
} // Dialogs
// GUI