#if defined(_MSC_VER) && (_MSC_VER >= 1600)
    #pragma execution_character_set("utf-8")
#endif

#ifndef NETWORKER_H
#define NETWORKER_H

#include <QObject>
#include <QtNetwork>
#include <QDebug>

#include "configworker.h"
#include "sutil.h"

class NetWorker : public QObject
{
    Q_OBJECT
public slots:
    void replyFinished(QNetworkReply*);

signals:
    void postStatus(QString);  //发送状态字符
    void postStatusCode(int);  //发送状态码
    void postUserInfo(int,QString);  //发送费用信息
    void postUpdateInfo(QString); //发送更新信息

public:
    explicit NetWorker(ConfigWorker &configHolder, QObject *parent = 0);

    void doLogin(QString account, QString pwd);
    void doLogoff();

    void getUpdateInfo(); //获取软件更新

private:
    ConfigWorker &config;

    QNetworkAccessManager netManager;

    //状态码
    int nowRequestType;

    void getLoginProp(); //获得登录页面的信息
    void postLogin(); //发送登录信息
    void postLogoff();  //发送下线信息
    void getUserInfo(); //获取用户费用等信息
};

#endif // NETWORKER_H
