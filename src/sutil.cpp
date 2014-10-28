#if defined(_MSC_VER) && (_MSC_VER >= 1600)
    #pragma execution_character_set("utf-8")
#endif

#include "sutil.h"

bool SpiritUtil::readFile(const QString& path, QString& content)
{
    if( path.isEmpty() )
        return false;

    QFile file( path );

    if( !file.open(QIODevice::ReadOnly|QIODevice::Text) )
    {
        return false;
    }

    QTextStream fs(&file);
    content = fs.readAll();

    file.close();
    return true;
}

//用于parse一个html文件（字符串），取出某id的dom中的value
bool SpiritUtil::getValueById(QString& page, QString& value, QString id)
{
    int i,pStart,pTail,ntmp;
    QString subStr;

    subStr = "id=\"" + id + "\"";

    pStart = page.indexOf( subStr );
    if ( -1 == pStart )
        return false;

    for (i=pStart; i>=0; i--)
    {
        if ( '<' == page[i] )
        {
            ntmp = i;
            break;
        }
    }

    subStr =  "value=\"";
    pStart = page.indexOf( subStr, ntmp );

    pStart += 7;
    pTail = page.indexOf( "\"", pStart );

    value = page.mid( pStart, pTail-pStart );

    return true;
}


//parse一个字符串，取出指定的两个子串中间的字符串
bool SpiritUtil::getMid(QString& page, QString& value, QString start, QString tail)
{
    int pStart,pTail;

    pStart = page.indexOf( start );
    if ( -1 == pStart )
        return false;

    pStart = pStart + start.length();

    pTail = page.indexOf( tail, pStart );

    value = page.mid( pStart, pTail-pStart );

    return true;
}

//通过运行js文件来加密密码
bool SpiritUtil::encodePwdByJs(QString pwd, QString& encodePwd)
{
    QString fileName = QApplication::applicationDirPath() + "/encode.js";

    qDebug()<<"js文件地址： "<<fileName;

    QString contents;
    if ( ! SpiritUtil::readFile( fileName, contents)  )
    {
        QMessageBox msg;
        msg.setText( "未能读取密码加密js文件" );
        msg.exec();

        return false;
    }

    QScriptEngine scriptEngine;
    scriptEngine.globalObject().setProperty("thePwd", pwd );
    scriptEngine.evaluate(contents, fileName);

    QScriptValue enPwdValue = scriptEngine.globalObject().property("enPwd");
    encodePwd = enPwdValue.toString();

    return true;
}

//简单的异或加解密
QString SpiritUtil::getXorEncryptDecrypt(const QString &source, const char &key)
{
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QByteArray byteArray = codec->fromUnicode(source);

    for(int i=0; i<byteArray.size(); i++){
        byteArray[i] = byteArray[i] ^ key;
    }

    QString result = codec->toUnicode(byteArray);
    return result;
}
