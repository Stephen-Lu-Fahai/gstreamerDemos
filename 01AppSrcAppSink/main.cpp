#include <QCoreApplication>
#include "appSrc.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    gst_init(NULL, NULL);

    AppSrc appSrc;

    return a.exec();
}
