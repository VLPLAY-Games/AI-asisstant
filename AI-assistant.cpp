﻿// Copyright MIT License 2025 VL_PLAY Games


#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    system("chcp 65001");
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return app.exec();
}
