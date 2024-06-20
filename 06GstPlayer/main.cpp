#include <QCoreApplication>
#include "display.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    GstDisplay gstDisplay;

    return a.exec();
}
