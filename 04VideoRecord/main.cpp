#include <QCoreApplication>
#include "videoRecord.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    gst_init(nullptr, nullptr);
    VideoRecord videoRecord;

    return a.exec();
}
