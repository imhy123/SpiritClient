#if defined(_MSC_VER) && (_MSC_VER >= 1600)
    #pragma execution_character_set("utf-8")
#endif

#include "configworker.h"

ConfigWorker::ConfigWorker(QObject *parent) :
    QObject(parent)
{
    //使用绝对路径，因为在开机启动时需要使用绝对路径才能正确定位
    filePath = QApplication::applicationDirPath() + "/config.ini";
    setting = new QSettings( filePath, QSettings::IniFormat, this );

    readConfig();
}

QSettings* ConfigWorker::getSetting()
{
    return setting;
}

//将配置读入本对象的属性中，方便调用
void ConfigWorker::readConfig()
{
    //检查是否已有配置文件，配置项是否正确
    QFileInfo fi( filePath );
    if ( !fi.exists() )
    {
        setDefault();
    }
    else if ( !isValidConfig() )
    {
        setDefault();
    }

    initUrl = setting->value( "initUrl" ).toString();
    hostAddr = setting->value( "hostAddr" ).toString();
    loginUrl = setting->value( "loginUrl" ).toString();
    loginDataUrl = setting->value( "loginDataUrl" ).toString();
    account = setting->value( "account" ).toString();
    pwd = setting->value( "pwd" ).toString();
    if ( pwd.length() > 0 )
        pwd = SpiritUtil::getXorEncryptDecrypt(pwd, 104);  //进行解密
    logoffUrl = setting->value( "logoffUrl" ).toString();
    logoffDataUrl = setting->value( "logoffDataUrl" ).toString();
    last_arg1_value = setting->value( "last_arg1_value" ).toString();
    last_arg2_value = setting->value( "last_arg2_value" ).toString();
    blogUrl = setting->value( "blogUrl" ).toString();
    updateUrl = setting->value( "updateUrl" ).toString();
    chargeUrl = setting->value( "chargeUrl" ).toString();

    autoLogin = setting->value( "autoLogin" ).toBool();
    autoStart = setting->value( "autoStart" ).toBool();
    autoReconnect = setting->value( "autoReconnect" ).toBool();
}

//检查必要项是否有值
bool ConfigWorker::isValidConfig()
{
    if( setting->value( "initUrl" ).toString().isEmpty() ) return false;
    if( setting->value( "hostAddr" ).toString().isEmpty() ) return false;
    if( setting->value( "loginUrl" ).toString().isEmpty() ) return false;
    if( setting->value( "loginDataUrl" ).toString().isEmpty() ) return false;
    if( setting->value( "logoffUrl" ).toString().isEmpty() ) return false;
    if( setting->value( "logoffDataUrl" ).toString().isEmpty() ) return false;
    if( setting->value( "chargeUrl" ).toString().isEmpty() ) return false;
    return true;
}

//写入默认值
void ConfigWorker::setDefault()
{
    setting->setValue("initUrl","http://www.google.com/");
    setting->setValue("hostAddr","61.137.86.87:8080");
    setting->setValue("loginUrl","http://61.137.86.87:8080/portalNat444/index.jsp");
    setting->setValue("loginDataUrl","http://61.137.86.87:8080/portalNat444/AccessServices/login");
    setting->setValue("account","");
    setting->setValue("pwd","");
    setting->setValue("logoffUrl","http://61.137.86.87:8080/portalNat444/main2.jsp");
    setting->setValue("logoffDataUrl","http://61.137.86.87:8080/portalNat444/AccessServices/logout?");
    setting->setValue("last_arg1_value","");
    setting->setValue("last_arg2_value","");
    setting->setValue("blogUrl","http://www.spirithy.com/");
    setting->setValue("updateUrl","http://www.spirithy.com/SpiritClientUpdate.php");
    setting->setValue("chargeUrl","http://hn.189.cn/hnselfservice/topup/topup!topupIndex.action?isCorp=0");

    setting->setValue("autoLogin", false);
    setting->setValue("autoStart", false);
    setting->setValue("autoReconnect", false);

    //登录时的错误信息
    setting->setValue("loginerror_1", "登录失败：密码错误或其他原因");
    setting->setValue("loginerror_2", "登录失败：用户连接已经存在");
    setting->setValue("loginerror_3", "登录失败：接入服务器繁忙");
    setting->setValue("loginerror_4", "登录失败：未知错误");
    setting->setValue("loginerror_6", "登录失败：认证响应超时");
    setting->setValue("loginerror_7", "登录失败：捕获用户网络地址错误");
    setting->setValue("loginerror_8", "登录失败：服务器网络连接异常");
    setting->setValue("loginerror_9", "登录失败：认证服务脚本执行异常");
    setting->setValue("loginerror_10", "登录失败：校验码错误");
    setting->setValue("loginerror_11", "登录失败：密码过于简单，请修改");
    setting->setValue("loginerror_12", "登录失败：无法获取网络地址");
    setting->setValue("loginerror_13", "登录失败：无法获取接入点设备地址");
    setting->setValue("loginerror_14", "登录失败：无法获取套餐信息");
    setting->setValue("loginerror_16", "登录失败：请从其他页面导航");
    setting->setValue("loginerror_17", "登录失败：连接已失效");

    //下线时的错误信息
    setting->setValue("logofferror_1", "下线失败：服务器拒绝请求");
    setting->setValue("logofferror_2", "下线失败：下线请求执行失败");
    setting->setValue("logofferror_3", "下线失败：您已经下线");
    setting->setValue("logofferror_4", "下线失败：服务器响应超时");
    setting->setValue("logofferror_5", "下线失败：后台网络连接异常");
    setting->setValue("logofferror_6", "下线失败：服务脚本执行异常");
    setting->setValue("logofferror_7", "下线失败：无法获取您的网络地址");
    setting->setValue("logofferror_8", "下线失败：无法获取接入点设备地址");
    setting->setValue("logofferror_9", "下线失败：请从其他页面导航");
}
