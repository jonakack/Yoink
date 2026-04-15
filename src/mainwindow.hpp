////////////////////////////////////////////////////////////////////
//                                                                //
//    Copyright (c) 2026 Jonathan Käcker. All rights reserved.    //
//                                                                //
////////////////////////////////////////////////////////////////////

#pragma once

#include <QMainWindow>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QRadioButton>
#include <QComboBox>
#include <QPushButton>
#include <QProgressBar>
#include <QLabel>
#include <QFont>
#include <QGraphicsOpacityEffect>
#include <QtConcurrent>
#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>

#include "downloader.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onDownloadClicked();
    void onFormatChanged();
    void onBrowseClicked();
    
private:
    Downloader             downloader;
       
    QWidget                *central;
    QVBoxLayout            *mainLayout;
       
    QLineEdit              *urlBar;
       
    QHBoxLayout            *formatLayout;
    QLabel                 *formatLabel;
    QRadioButton           *formatMp3Button;
    QRadioButton           *formatMp4Button;
       
    QHBoxLayout            *qualityLayout;
    QLabel                 *qualityLabel;
    QComboBox              *qualitySelectionBox;
       
    QLineEdit              *fileNameBar;
       
    QHBoxLayout            *saveLayout;
    QLineEdit              *savePathBar;
    QPushButton            *savePathBrowseButton;
       
    QPushButton            *downloadButton;
    QProgressBar           *progressBar;
    QGraphicsOpacityEffect *opacityEffect;
};