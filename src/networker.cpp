#if defined(_MSC_VER) && (_MSC_VER >= 1600)
    #pragma execution_character_set("utf-8")
#endif

#include "networker.h"

NetWorker::NetWorker(ConfigWorker &configHolder, QObject *parent) :
    QObject(parent),
    config(configHolder)
{
    connect( &netManager, SIGNAL( finished(QNetworkReply*) ), this, SLOT( replyFinished(QNetworkReply*) ) );
}

void NetWorker::replyFinished(QNetworkReply *reply)
{
    QString replyContent = reply->readAll();

    if ( 1 == nowRequestType ) //已得到prop信息
    {
        QString value;
        bool getAllProp = true;
        if ( SpiritUtil::getMid(replyContent, value, "ces/bas.", "?wlanuserip") )
        {
            config.getSetting()->setValue( "last_arg1_value", value );
        }
        else
        {
            getAllProp = false;
        }

        if ( SpiritUtil::getMid(replyContent, value, "userip=", "&wlanacname") )
        {
            config.getSetting()->setValue( "last_arg2_value", value );
        }
        else
        {
            getAllProp = false;
        }

        if ( getAllProp )
        {
            config.readConfig();
            postLogin();
        }
        else
        {
            emit postStatus("未能读取特征码，可能已登录");
            emit postStatusCode(32);
        }
    }
    else if ( 2 == nowRequestType ) //已得到登录信息
    {
        QString resultCode;

        if ( ! SpiritUtil::getMid(replyContent, resultCode, "resultCode\":\"", "\",\"") )
        {
            emit postStatus("异常：未能获取resultCode！");
            emit postStatusCode(32);

            return;
        }

        qDebug()<<"resultCode:"<<resultCode;

        if ( 0 == resultCode.compare("0") )
        {
            emit postStatus("当前状态：在线");
            emit postStatusCode(31);

            getUserInfo();  //获取费用信息
        }
        else
        {
            emit postStatus( config.getSetting()->value( "loginerror_"+resultCode ).toString() );
            emit postStatusCode(32);
        }
    }
    else if ( 11 == nowRequestType ) //已得到下线信息
    {
        QString resultCode;

        if ( ! SpiritUtil::getMid(replyContent, resultCode, "resultCode\":\"", "\",\"") )
        {
            emit postStatus("异常：未能获取resultCode！");
            emit postStatusCode(42);

            return;
        }

        qDebug()<<"resultCode:"<<resultCode;

        if ( 0 == resultCode.compare("0") )
        {
            emit postStatus("当前状态：下线");
            emit postStatusCode(41);
        }
        else
        {
            emit postStatus( config.getSetting()->value( "logofferror_"+resultCode ).toString() );
            emit postStatusCode(42);
        }
    }
    else if ( 21 == nowRequestType ) //得到用户费用信息
    {
        QString value;
        if ( SpiritUtil::getMid(replyContent, value, "总流量(公网):", "MB") )
        {
            emit postUserInfo(1, value + "MB" );
        }
        if ( SpiritUtil::getMid(replyContent, value, "剩余流量(公网):", "MB") )
        {
            emit postUserInfo(2, value + "MB" );
        }
        if ( SpiritUtil::getMid(replyContent, value, "已用流量（校园网):", "MB") )
        {
            emit postUserInfo(3, value + "MB" );
        }
        if ( SpiritUtil::getMid(replyContent, value, "剩余金额:", "元") )
        {
            emit postUserInfo(4, value + " 元" );
        }
    }
    else if ( 31 == nowRequestType ) //软件更新信息
    {
        emit postUpdateInfo( replyContent.trimmed() );
    }
}

//函数：获得登录页面的信息
void NetWorker::getLoginProp()
{
    QNetworkRequest request;
    request.setUrl( QUrl( config.initUrl ) );

    emit postStatus("开始获取登录信息...");

    nowRequestType = 1;

    netManager.get( request );
}

//函数：发送登录信息
void NetWorker::postLogin()
{
    emit postStatus("正在登录...");

    QNetworkRequest request;
    request.setUrl( QUrl( config.loginDataUrl.toUtf8() ) );
    request.setRawHeader( "Host", config.hostAddr.toUtf8() );
    request.setRawHeader( "User-Agent", "Mozilla/5.0 (windows NT 6.2; WOW64; rv:17.0) Gecko/20100101 Firefox/17.0" );
    request.setRawHeader( "Accept", "application/json, text/javascript, */*; q=0.01" );
    request.setRawHeader( "Accept-Language", "zh-cn,zh;q=0.8,en-us; q=0.5,en; q=0.3" );
    request.setRawHeader( "Accept-Encoding", "gzip, deflate" );
    request.setRawHeader( "Content-Type", "application/x-www-form-urlencoded; charset=utf-8" );
    request.setRawHeader( "X-Requested-with", "XMLHttpRequest" );
    request.setRawHeader( "Referer", config.loginUrl.toUtf8() );

    QString enPwd;
    if ( ! SpiritUtil::encodePwdByJs( config.pwd, enPwd) )
    {
        emit postStatus("加密密码失败，登录停止");
        emit postStatusCode(32);

        return;
    }

    //qDebug()<<"加密密码： "<<enPwd;

    QString requestData = "accountID=" + config.account + "@zndx.inter&password="+ enPwd
            + "&brasAddress=" + config.last_arg1_value
            + "&userIntranetAddress=" + config.last_arg2_value;

    //qDebug()<<"requestData： "<<requestData;

    nowRequestType = 2; //进行登录
    netManager.post( request, requestData.toUtf8() );
}

//函数：发送下线信息
void NetWorker::postLogoff()
{
    emit postStatus( "正在下线..." );

    QNetworkRequest request;
    request.setUrl( QUrl( config.logoffDataUrl ) );
    request.setRawHeader( "Host", config.hostAddr.toUtf8() );
    request.setRawHeader( "User-Agent", "Mozilla/5.0 (windows NT 6.2; WOW64; rv:17.0) Gecko/20100101 Firefox/17.0" );
    request.setRawHeader( "Accept", "application/json, text/javascript, */*; q=0.01" );
    request.setRawHeader( "Accept-Language", "zh-cn,zh;q=0.8,en-us; q=0.5,en; q=0.3" );
    request.setRawHeader( "Accept-Encoding", "gzip, deflate" );
    request.setRawHeader( "Content-Type", "application/x-www-form-urlencoded; charset=utf-8" );
    request.setRawHeader( "X-Requested-with", "XMLHttpRequest" );
    request.setRawHeader( "Referer", config.logoffUrl.toUtf8() );

    QString requestData = "brasAddress=" + config.last_arg1_value
            + "&userIntranetAddress=" + config.last_arg2_value;

    nowRequestType = 11; //进行下线
    netManager.post( request, requestData.toUtf8() );
}

//函数：获取用户费用等信息
void NetWorker::getUserInfo()
{
    QNetworkRequest request;
    request.setUrl( QUrl( config.logoffUrl ) );
    request.setRawHeader( "Host", config.hostAddr.toUtf8() );
    request.setRawHeader( "User-Agent", "Mozilla/5.0 (windows NT 6.2; WOW64; rv:17.0) Gecko/20100101 Firefox/17.0" );
    request.setRawHeader( "Accept", "application/json, text/javascript, */*; q=0.01" );
    request.setRawHeader( "Accept-Language", "zh-cn,zh;q=0.8,en-us; q=0.5,en; q=0.3" );
    request.setRawHeader( "Accept-Encoding", "gzip, deflate" );
    request.setRawHeader( "Referer", config.loginUrl.toUtf8() );

    nowRequestType = 21; //获取费用信息
    netManager.get( request );
}

//函数：获取软件更新
void NetWorker::getUpdateInfo()
{
    qDebug()<<"开始检查更新";

    QNetworkRequest request;
    request.setUrl( QUrl( config.updateUrl ) );

    nowRequestType = 31; //获取更新信息
    netManager.get( request );
}

//函数：进行登录
void NetWorker::doLogin(QString account, QString pwd)
{
    config.getSetting()->setValue("account", account);
    config.getSetting()->setValue("pwd", SpiritUtil::getXorEncryptDecrypt(pwd, 104) );  //加密写入
    config.readConfig();

    getLoginProp();
}

//函数：进行下线
void NetWorker::doLogoff()
{
    postLogoff();
}
