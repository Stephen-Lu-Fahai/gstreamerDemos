#include <QCoreApplication>
#include "gstRecorder.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    GstRecorder gstRecorder;
    gstRecorder.start();

    return a.exec();
}
