#include <QCoreApplication>
#include "audioRecord.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    gst_init(NULL, NULL);

    AudioRecord audioRecord;

    return a.exec();
}
