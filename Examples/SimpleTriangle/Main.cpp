#include <QApplication>
#include <QWindow>

#include <iostream>
#include <string>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QWindow w;
    w.show();

    return a.exec();
}