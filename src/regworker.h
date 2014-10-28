#ifndef REGWORKER_H
#define REGWORKER_H

#include <QObject>
#include <QApplication>
#include <QProcess>
#include <QDebug>

class RegWorker : public QObject
{
    Q_OBJECT

public:
    explicit RegWorker(QObject *parent = 0);

    void writeAutoStart();
    void delAutoStart();

private:
    QProcess *process;
};

#endif // REGWORKER_H
