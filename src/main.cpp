////////////////////////////////////////////////////////////////////
//                                                                //
//    Copyright (c) 2026 Jonathan Käcker. All rights reserved.    //
//                                                                //
////////////////////////////////////////////////////////////////////

#include <iostream>

#include <QApplication>

#include "mainwindow.hpp"
#include "extractor.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    if (!Extractor::extractTools())
    {
        std::cerr << "Failed to extract tools\n";
        return 1;
    }

    MainWindow window;
    window.show();

    int result = app.exec();
    Extractor::cleanup();
    return result;
}