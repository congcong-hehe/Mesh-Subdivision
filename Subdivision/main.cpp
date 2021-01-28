#include "Subdivision.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Subdivision w;
    w.show();
    return a.exec();
}
