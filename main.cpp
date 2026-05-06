#include <QApplication>
#include <QFont>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Ігрове опитування");
    app.setOrganizationName("Lab3");

    QFont defaultFont("Segoe UI", 10);
    app.setFont(defaultFont);

    MainWindow window;
    window.show();

    return app.exec();
}
