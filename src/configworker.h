#if defined(_MSC_VER) && (_MSC_VER >= 1600)
    #pragma execution_character_set("utf-8")
#endif

#ifndef CONFIGWORKER_H
#define CONFIGWORKER_H

#include <QObject>
#include <QApplication>
#include <QFileInfo>
#include <QSettings>

#include "sutil.h"

class ConfigWorker : public QObject
{
    Q_OBJECT
public:
    explicit ConfigWorker(QObject *parent = 0);

    void setDefault();  //写入默认值
    void readConfig();  //将配置读入本对象的属性中，方便调用

    QSettings* getSetting();

    //下面是保存的配置项
    QString initUrl;
    QString hostAddr;
    QString loginUrl;
    QString loginDataUrl;
    QString account;
    QString pwd;
    QString logoffUrl;
    QString logoffDataUrl;
    QString last_arg1_value;
    QString last_arg2_value;
    QString blogUrl;
    QString updateUrl;
    QString chargeUrl;

    bool autoLogin;
    bool autoStart;
    bool autoReconnect;

private:
    QString filePath;
    QSettings* setting;

    bool isValidConfig();  //检查配置项是否正确，防止因配置文件错误导致程序出现异常

};

#endif // CONFIGWORKER_H
