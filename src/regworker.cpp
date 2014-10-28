#include "regworker.h"

RegWorker::RegWorker(QObject *parent) :
    QObject(parent)
{
    process = new QProcess( this );
}

void RegWorker::writeAutoStart()
{
    QString filePath = "AdminLauncher.exe";

    QStringList arguments;
    arguments << "write";

    process->start(filePath, arguments);
}

void RegWorker::delAutoStart()
{
    QString filePath = "AdminLauncher.exe";

    QStringList arguments;
    arguments << "delete";

    process->start(filePath, arguments);
}
