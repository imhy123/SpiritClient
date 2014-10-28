#if defined(_MSC_VER) && (_MSC_VER >= 1600)
    #pragma execution_character_set("utf-8")
#endif

#ifndef SUTIL_H
#define SUTIL_H

#include <QFile>
#include <QTextStream>
#include <QApplication>
#include <QtScript>
#include <QMessageBox>
#include <QTextCodec>
#include <QDebug>

namespace SpiritUtil {
    bool readFile(const QString& path, QString& content);
    bool getValueById(QString& page, QString& value, QString id);
    bool getMid(QString& page, QString& value, QString start, QString tail);
    bool encodePwdByJs(QString pwd, QString& encodePwd);
    QString getXorEncryptDecrypt(const QString &source, const char &key);
}

#endif // SUTIL_H
