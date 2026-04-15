////////////////////////////////////////////////////////////////////
//                                                                //
//    Copyright (c) 2026 Jonathan Käcker. All rights reserved.    //
//                                                                //
////////////////////////////////////////////////////////////////////

#include <string>

#include "mainwindow.hpp"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    // ------------------------------------------------------
    //  Window
    // ------------------------------------------------------
    setWindowTitle("Yoink");
    setFixedSize(500, 300);

    // ------------------------------------------------------
    //  Create widgets
    // ------------------------------------------------------
    // URL
    urlBar               = new QLineEdit;
        
    // Format       
    formatLayout         = new QHBoxLayout;
    formatLabel          = new QLabel("Format:");
    formatMp3Button      = new QRadioButton("MP3");
    formatMp4Button      = new QRadioButton("MP4");

    // File name
    fileNameBar          = new QLineEdit;

    // Quality selection
    qualityLayout        = new QHBoxLayout;
    qualityLabel         = new QLabel("Quality:");
    qualitySelectionBox  = new QComboBox;

    // Saving
    saveLayout           = new QHBoxLayout;
    savePathBar          = new QLineEdit;
    savePathBrowseButton = new QPushButton("Browse");

    // Download
    QFont downloadButtonFont;
    downloadButton       = new QPushButton("Download");
    progressBar          = new QProgressBar;
    opacityEffect        = new QGraphicsOpacityEffect;

    // ------------------------------------------------------
    //  URL bar
    // ------------------------------------------------------
    urlBar->setFixedHeight(35);
    urlBar->setPlaceholderText("Enter YouTube URL");

    // ------------------------------------------------------
    //  Format radio buttons
    // ------------------------------------------------------
    formatLayout->addWidget(formatLabel);
    formatLayout->addWidget(formatMp3Button);
    formatLayout->addWidget(formatMp4Button);
    formatLayout->addStretch();
    formatMp3Button->setChecked(true); // Check MP3 button by default

    // ------------------------------------------------------
    //  Quality selector
    // ------------------------------------------------------
    qualityLayout->addWidget(qualityLabel);
    qualityLayout->addWidget(qualitySelectionBox);
    qualitySelectionBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    qualitySelectionBox->setPlaceholderText("Select quality");
    qualitySelectionBox->setFixedHeight(35);
    qualitySelectionBox->addItem("Highest quality available");
    qualitySelectionBox->addItem("320kbp/s");
    qualitySelectionBox->addItem("256kbp/s");
    qualitySelectionBox->addItem("128kbp/s");
    qualitySelectionBox->addItem("96kbp/s");
    qualitySelectionBox->setCurrentIndex(0); // Set "Highest quality available" as default

    // ------------------------------------------------------
    //  Output folder selection
    // ------------------------------------------------------
    saveLayout->addWidget(savePathBar);
    saveLayout->addWidget(savePathBrowseButton);
    savePathBar->setPlaceholderText("Enter output folder");
    savePathBar->setFixedHeight(35);
    savePathBrowseButton->setFixedHeight(35);

    // ------------------------------------------------------
    //  Filename selection
    // ------------------------------------------------------
    fileNameBar->setPlaceholderText("Enter filename (optional)");
    fileNameBar->setFixedHeight(35);

    // ------------------------------------------------------
    //  Download button
    // ------------------------------------------------------
    downloadButtonFont.setPointSize(15);
    downloadButton->setFixedHeight(50);
    downloadButton->setFont(downloadButtonFont);

    // ------------------------------------------------------
    //  Progress bar
    // ------------------------------------------------------
    progressBar->setFont(QFont("Consolas", 9));
    opacityEffect->setOpacity(0.0);
    progressBar->setValue(0);
    progressBar->setGraphicsEffect(opacityEffect);

    // ------------------------------------------------------
    //  Central widget and main layout
    // ------------------------------------------------------
    central    = new QWidget(this);
    mainLayout = new QVBoxLayout(central);
    mainLayout->addWidget(urlBar);
    mainLayout->addLayout(formatLayout);
    mainLayout->addLayout(qualityLayout);
    mainLayout->addLayout(saveLayout);
    mainLayout->addWidget(fileNameBar);
    mainLayout->addWidget(downloadButton);
    mainLayout->addWidget(progressBar);

    setCentralWidget(central);

    // ------------------------------------------------------
    //  Load settings
    // ------------------------------------------------------
    QSettings settings("KCorp", "Yoink");
    savePathBar->setText(settings.value("lastSavePath", "").toString());

    // ------------------------------------------------------
    //  Adding functionality to the GUI
    // ------------------------------------------------------
    connect(formatMp3Button,      &QRadioButton::toggled, this, &MainWindow::onFormatChanged);
    connect(downloadButton,       &QPushButton::clicked,  this, &MainWindow::onDownloadClicked);
    connect(savePathBrowseButton, &QPushButton::clicked,  this, &MainWindow::onBrowseClicked);
}

// ----------------------------------------------------------------
//  This function fires when the download button is clicked. It
//  verifies that all required fields are filled in. If they aren't,
//  a warning pop-up shows and the function returns. If they are,
//  Downloader::run() is executed with the filled fields as input
//  on a new thread to prevent the application to hang.
// ----------------------------------------------------------------
void MainWindow::onDownloadClicked()
{
    std::string url = urlBar->text().toStdString();
    bool isMp3 = formatMp3Button->isChecked();
    bool isMp4 = formatMp4Button->isChecked();
    std::string quality = qualitySelectionBox->currentText().toStdString();
    std::string outputFolder = savePathBar->text().toStdString();
    std::string filename = fileNameBar->text().toStdString();

    // ------------------------------------------------------
    //  Error checking
    // ------------------------------------------------------
    if (url.empty() || outputFolder.empty())
    {
        QMessageBox::warning(this, "Missing fields", "Please fill in all required fields");
        return;
    }

    if (!isMp3 && !isMp4)
    {
        QMessageBox::warning(this, "Missing format", "Please choose MP3 or MP4");
        return;
    }

    if (!QDir(savePathBar->text()).exists())
    {
        QMessageBox::warning(this, "Invalid folder", "The output folder does not exist.");
        return;
    }

    // ------------------------------------------------------
    //  Debug lines
    // ------------------------------------------------------
    qDebug() << "URL:"           << url;
    qDebug() << "mp3:"           << isMp3;
    qDebug() << "mp4:"           << isMp4;
    qDebug() << "Quality:"       << quality;
    qDebug() << "Output folder:" << outputFolder;
    qDebug() << "Filename:"      << filename;

    // ------------------------------------------------------
    //  Save settings
    // ------------------------------------------------------
    QSettings settings("KCorp", "Yoink");
    settings.setValue("lastSavePath", savePathBar->text());

    // ------------------------------------------------------
    //  Execution
    // ------------------------------------------------------
    downloadButton->setEnabled(false);
    auto future = QtConcurrent::run([=]()
    { 
        bool success = downloader.run(url, isMp3, quality, outputFolder, filename, [this](float percent, float downloadedMiB, float totalMiB, const std::string &speed){
            QString text = QString("%1 / %2 MiB  |  %3")
                .arg(downloadedMiB, 7, 'f', 1)
                .arg(totalMiB, 0, 'f', 1)
                .arg(QString::fromStdString(speed), -12);
            QMetaObject::invokeMethod(this, [this, percent, text](){
                opacityEffect->setOpacity(1.0);
                progressBar->setValue(static_cast<int>(percent));
                progressBar->setFormat(text);
            }, Qt::QueuedConnection);
        });

        // Runs after download finishes
        QMetaObject::invokeMethod(this, [this, outputFolder, success](){
            downloadButton->setEnabled(true);
            progressBar->setValue(0);
            progressBar->setFormat("%p%");
            opacityEffect->setOpacity(0.0);
            if (success)
            {
                QDesktopServices::openUrl(QUrl::fromLocalFile(QString::fromStdString(outputFolder)));
                QMessageBox::information(this, "Done", "Download successful");
            }
            else
            {
                QMessageBox::critical(this, "Error", "Download failed. Check that the URL is valid");
            }
        }, Qt::QueuedConnection);
    });
}

// ----------------------------------------------------------------
//  Repopulates the quality dropdown when the format changes.
//  MP3 shows audio bitrates, MP4 shows video resolutions.
// ----------------------------------------------------------------
void MainWindow::onFormatChanged()
{
    qualitySelectionBox->clear();

    if (formatMp3Button->isChecked())
    {
        qualitySelectionBox->addItem("Highest quality available");
        qualitySelectionBox->addItem("320kbp/s");
        qualitySelectionBox->addItem("256kbp/s");
        qualitySelectionBox->addItem("128kbp/s");
        qualitySelectionBox->addItem("96kbp/s");
        qualitySelectionBox->setCurrentIndex(0);
    }
    else // Mp4Button->isChecked()
    {
        qualitySelectionBox->addItem("Highest quality available");
        qualitySelectionBox->addItem("4K");
        qualitySelectionBox->addItem("1440p");
        qualitySelectionBox->addItem("1080p");
        qualitySelectionBox->addItem("720p");
        qualitySelectionBox->addItem("480p");
        qualitySelectionBox->addItem("360p");
        qualitySelectionBox->addItem("240p");
        qualitySelectionBox->addItem("144p");
        qualitySelectionBox->setCurrentIndex(0);
    }
}

// ----------------------------------------------------------------
//  Opens a folder picker and stores the selected path.
// ----------------------------------------------------------------
void MainWindow::onBrowseClicked()
{
    QString outputFolder = QFileDialog::getExistingDirectory(this, "Select output folder");

    if (outputFolder.isEmpty()) return;

    if (!QDir(outputFolder).exists())
    {
        QMessageBox::warning(this, "Invalid folder", "The output folder does not exist.");
        return;
    }

    savePathBar->setText(outputFolder);
    QSettings settings("KCorp", "Yoink");
    settings.setValue("lastSavePath", outputFolder);
}