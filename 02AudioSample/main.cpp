#include <QCoreApplication>
#include "audioSample.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    AudioSample audioSample;

    return a.exec();
}
