#include <QCoreApplication>
#include "gstLive.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    GstLive gstLive;

    return a.exec();
}
