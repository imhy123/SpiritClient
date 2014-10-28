#if defined(_MSC_VER) && (_MSC_VER >= 1600)
    #pragma execution_character_set("utf-8")
#endif

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    netWorker(config),
    versionStr("SpiritClient V2.1"),
    timeAutoReconnect(11,0,0),
    inAutoReconnect(false),
    updateChecked(false),
    isNewestVersion(true),
    singleSharedMemory("SpiritClient"),
    SingleTimeout(1000)
{
    ui->setupUi(this);

    //LocalServer、单实例判断
    singleLocalServer = NULL;
    if ( ! isFirstInstance() )
    {
        singleSendShowMessage();

        //这里使用定时器而非exit(-1)来“优雅”地结束程序
        QTimer::singleShot(0, qApp, SLOT( quit() ) );
    }
    else
    {
        //创建LocalServer接受其他实例的消息
        createSingleLocalServer();

        //版本信息
        ui->titleLabel->setText( versionStr );

        //绑定窗口按钮信号和槽
        connect( ui->closeButton, SIGNAL( clicked() ), this, SLOT( close() ) );
        connect( ui->minButton, SIGNAL( clicked() ), this, SLOT( showMinimized() ) );
        connect( ui->toTrayButton, SIGNAL( clicked() ), this, SLOT( onToTrayClicked() ) );

        //绑定登录下线按钮的信号和槽
        connect( ui->loginButton, SIGNAL( clicked() ), this, SLOT( onLoginClicked() ) );
        connect( ui->logoffButton, SIGNAL( clicked() ), this, SLOT( onLogoffClicked() ) );

        //绑定密码输入框的回车事件
        connect( ui->pwdEdit, SIGNAL( returnPressed() ), this, SLOT( onLoginClicked() ) );

        //绑定复选框信号和槽
        connect( ui->autoLoginCbox, SIGNAL( toggled(bool) ), this, SLOT( onAutoLoginChecked(bool) ) );
        connect( ui->autoStartCbox, SIGNAL( toggled(bool) ), this, SLOT( onAutoStartChecked(bool) ) );
        connect( ui->reconnectCbox, SIGNAL( toggled(bool) ), this, SLOT( onAutoReconnectChecked(bool) ) );

        //绑定交费链接信号和槽
        connect( ui->chargeLinkButton, SIGNAL( clicked() ), this, SLOT( onChargeUrlClicked() ) );

        //定时器
        connect( &timer, SIGNAL( timeout() ), this, SLOT( timerActived() ) );

        //托盘图标
        QIcon icon(":/images/Resource/SpiritClient_16x16.png");
        trayIcon.setIcon(icon);
        trayIcon.setToolTip( versionStr );
        connect( &trayIcon, SIGNAL( activated(QSystemTrayIcon::ActivationReason) ),
                 this, SLOT( onTrayIconClicked(QSystemTrayIcon::ActivationReason) ) );
        connect( &trayIcon, SIGNAL( messageClicked() ), this, SLOT( onTrayMessageClicked() ) );
        trayIcon.show();

        //信息面板动画
        infoWidgetAnimation = new QPropertyAnimation( ui->infoWidget, "geometry", this);
        connect( ui->infoWidget, SIGNAL( mouseEntered() ), this, SLOT( infoWidgetAction() ) );
        connect( infoWidgetAnimation, SIGNAL( finished() ), this, SLOT( infoWidgetFinish() ) );
        infoWidgetActionFinished = true;
        infoWidgetOriginal = true;

        //绑定网络连接与槽
        connect( &netWorker, SIGNAL( postStatus(QString) ), this, SLOT( getStatus(QString) ) );
        connect( &netWorker, SIGNAL( postStatusCode(int) ), this, SLOT( getStatusCode(int) ) );
        connect( &netWorker, SIGNAL( postUserInfo(int,QString) ), this, SLOT( setUserInfo(int,QString) ) );
        connect( &netWorker, SIGNAL( postUpdateInfo(QString) ), this, SLOT( getUpdateInfo(QString) ) );

        //根据配置文件在界面上进行初始化，如复选框、账号、密码
        if ( config.autoLogin ) ui->autoLoginCbox->setChecked(true);
        if ( config.autoStart ) ui->autoStartCbox->setChecked(true);
        if ( config.autoReconnect ) ui->reconnectCbox->setChecked(true);
        if ( config.account.length() > 0 ) ui->accountEdit->setText( config.account );
        if ( config.pwd.length() > 0 ) ui->pwdEdit->setText( config.pwd );

        //假如勾选了自动登录
        if ( config.autoLogin )
        {
            //onLoginClicked();
            QTimer::singleShot( 500, this, SLOT( onLoginClicked() ) );
        }
    }
}

MainWindow::~MainWindow()
{
    if ( NULL != singleLocalServer )
        singleLocalServer->close();
    delete ui;
}

//槽：响应定时器的消息
void MainWindow::timerActived()
{
    timeRecord = timeRecord.addSecs(1);
    ui->onlineTimeELabel->setText( timeRecord.toString() );

    //定时重连
    if ( timeRecord >= timeAutoReconnect && !inAutoReconnect )
    {
        inAutoReconnect = true;
        onLogoffClicked();
    }

    //检查更新
    if ( !updateChecked && timeRecord >= QTime(0,0,30) )
    {
        updateChecked = true;
        netWorker.getUpdateInfo();
    }
}

//槽：登录按钮被点击
void MainWindow::onLoginClicked()
{
    ui->loginButton->setFocus();

    if ( ui->accountEdit->text().length() <= 0 || ui->pwdEdit->text().length() <= 0 )
    {
        getStatus( "请输入账号及密码" );
        return;
    }

    ui->loginButton->setEnabled(false);
    ui->logoffButton->setEnabled(false);

    netWorker.doLogin( ui->accountEdit->text(), ui->pwdEdit->text());
}

//槽：下线按钮被点击
void MainWindow::onLogoffClicked()
{
    ui->logoffButton->setFocus();

    ui->loginButton->setEnabled(false);
    ui->logoffButton->setEnabled(false);

    netWorker.doLogoff();
}

//槽：接收网络工作类发送过来的状态码
void MainWindow::getStatusCode(int code)
{
    if ( 31 == code ) //成功登录
    {
        timeRecord.setHMS(0,0,0);
        timer.start( 1000 );

        ui->logoffButton->setEnabled(true);

        //假如处于定时重连中
        if ( inAutoReconnect )
        {
            inAutoReconnect = false;
            trayIcon.showMessage( versionStr, "已成功进行了定时重连" );
        }
        else if ( !this->isVisible() ) //开机自启、自动登录时
        {
            trayIcon.showMessage( versionStr, "已成功登录" );
        }
    }
    else if ( 32 == code ) //登录失败
    {
        //登录、下线按键都打开，因为有可能是因为已在线而失败，此时应提供下线功能
        //采取“消极”策略，在出现失败的情况下，打开两个按键，方便用户操作
        ui->loginButton->setEnabled(true);
        ui->logoffButton->setEnabled(true);

        //假如处于定时重连中
        if ( inAutoReconnect )
        {
            inAutoReconnect = false;
            trayIcon.showMessage( versionStr, "进行定时重连时登录失败！", QSystemTrayIcon::Warning );
        }
        else if ( !this->isVisible() ) //开机自启、自动登录时
        {
            trayIcon.showMessage( versionStr, "登录失败！",  QSystemTrayIcon::Warning );
        }
    }
    else if ( 41 == code ) //成功下线
    {
        timer.stop();
        timeRecord.setHMS(0,0,0);

        ui->onlineTimeELabel->setText( "暂无" );
        ui->totalFlowELabel->setText( "暂无" );
        ui->residualFlowELabel->setText( "暂无" );
        ui->interFlowELabel->setText( "暂无" );
        ui->residualMoneyELabel->setText( "暂无" );

        ui->loginButton->setEnabled(true);

        //假如处于定时重连中
        if ( inAutoReconnect )
        {
            onLoginClicked();
        }
    }
    else if ( 42 == code )  //下线失败
    {
        //采取“消极”策略，在出现失败的情况下，打开两个按键，方便用户操作
        ui->loginButton->setEnabled(true);
        ui->logoffButton->setEnabled(true);

        //假如处于定时重连中
        if ( inAutoReconnect )
        {
            inAutoReconnect = false;
            trayIcon.showMessage( versionStr, "进行定时重连时下线失败！", QSystemTrayIcon::Warning );
        }
    }
}

//槽：接收更新信息
void MainWindow::getUpdateInfo(QString content)
{
    qDebug()<<"更新信息："<<content;

    if ( content.length() <= 0 ) return;

    QStringList strList = content.split(",");
    if ( strList.length() != 2 ) return;

    if ( versionStr.compare( strList.at(0) ) != 0 )
    {
        trayIcon.showMessage( versionStr, "有新的版本：" + strList.at(0) + " ，点击此消息查看更新" );
        isNewestVersion = false;
        updateInfoUrl = strList.at(1).trimmed();
    }
}

//槽：设置用户的费用等信息
void MainWindow::setUserInfo(int index,QString content)
{
    switch (index)
    {
        case 1:
            ui->totalFlowELabel->setText(content);
            break;
        case 2:
            ui->residualFlowELabel->setText(content);
            break;
        case 3:
            ui->interFlowELabel->setText(content);
            break;
        case 4:
            ui->residualMoneyELabel->setText(content);
            break;
    }
}

//槽：在状态栏显示状态信息，设计为槽以接收其它工具类发送过来的消息
void MainWindow::getStatus(QString text)
{
    ui->statusELabel->setText(text);
}

//槽：自动登录复选框被点击
void MainWindow::onAutoLoginChecked(bool checked)
{
    if ( checked && !config.autoLogin )
    {
        config.getSetting()->setValue("autoLogin", true);
        config.readConfig();
    }
    else if ( !checked && config.autoLogin )
    {
        config.getSetting()->setValue("autoLogin", false);
        config.readConfig();
    }
}

//槽：开机自启复选框被点击
void MainWindow::onAutoStartChecked(bool checked)
{
    //加一个 config.autoStart 的判断，因为在初始化时设置复选框状态时也会触发这个槽。且若是在开机启动的情况下，因为路径原因会带来麻烦。
    if ( checked && !config.autoStart )
    {
        config.getSetting()->setValue("autoStart", true);
        config.readConfig();
        regWorker.writeAutoStart();
    }
    else if ( !checked && config.autoStart )
    {
        config.getSetting()->setValue("autoStart", false);
        config.readConfig();
        regWorker.delAutoStart();
    }
}

//槽：定时重连复选框被点击
void MainWindow::onAutoReconnectChecked(bool checked)
{
    if ( checked && !config.autoReconnect )
    {
        config.getSetting()->setValue("autoReconnect", true);
        config.readConfig();
    }
    else if ( !checked && config.autoReconnect )
    {
        config.getSetting()->setValue("autoReconnect", false);
        config.readConfig();
    }
}

//函数：使用共享内存来判断进程是否已存在
bool MainWindow::isFirstInstance()
{
    if ( singleSharedMemory.attach() )
    {
        qDebug() << "already have a instance!!";
        return false;
    }
    else
    {
        if( !singleSharedMemory.create(1) )
        {
            QMessageBox::warning( this, "警告", "创建共享内存时失败："+singleSharedMemory.errorString() );
        }
        qDebug() << "This is the only instance";
        return true;
    }
}

//函数：创建LocalServer服务器
void MainWindow::createSingleLocalServer()
{
    singleLocalServer = new QLocalServer(this);
    singleLocalServer->listen("SpiritClient");
    connect( singleLocalServer, SIGNAL( newConnection() ), this, SLOT( singleReceiveMessage() ) );
}

//函数：向已存在的进程创建的LocalServer服务器发送消息，要求其显示
void MainWindow::singleSendShowMessage()
{
    QLocalSocket singleLocalSocket;
    singleLocalSocket.connectToServer("SpiritClient");
    if ( singleLocalSocket.waitForConnected(SingleTimeout) )
    {
        singleLocalSocket.write("show");
        if( !singleLocalSocket.waitForBytesWritten(SingleTimeout) )
        {
            QMessageBox::warning( this, "警告", "唤醒已存在的进程时超时！" );
        }
        singleLocalSocket.disconnectFromServer();
    }
    else
    {
        QMessageBox::warning( this, "警告", "连接到已存在的进程时超时！" );
    }
}

//槽：LocalServer接收到了消息
void MainWindow::singleReceiveMessage()
{
    //这个指针会在singleLocalServer销毁时自动删除
    QLocalSocket *singleConnectSocket = singleLocalServer->nextPendingConnection();
    if( !singleConnectSocket->waitForReadyRead(SingleTimeout) )
    {
        QMessageBox::warning( this, "警告", "读取其它进程实例消息时超时！" );
    }
    else
    {
        qDebug()<<"收到了来自其它进程的消息 "<<QString::fromUtf8( singleConnectSocket->readAll().constData() );
        //恢复窗口显示
        this->setWindowState(Qt::WindowActive);
        this->show();
    }
    singleConnectSocket->disconnectFromServer();
}

//槽：交费链接被点击
void MainWindow::onChargeUrlClicked()
{
    QUrl chargeUrl( config.chargeUrl );

    // QUrl::isValid方法似乎不起作用，为了安全起见(防止shell执行)，这里使用正则表达式来校验网址
    QRegExp regExp( "(http|https)://([\\w-]+\\.)+[\\w-]+(/[\\w- ./!?%&=]*)?" );
    if ( regExp.exactMatch(config.chargeUrl) )
    {
        QDesktopServices::openUrl( chargeUrl );
    }
    else
    {
        QMessageBox::warning( this, "警告", "配置文件中的交费链接无效，您可以在删除客户端所在目录下的config.ini文件后重启客户端重试" );
    }
}

//槽：响应信息面板鼠标进入，并控制移入或移出
void MainWindow::infoWidgetAction()
{
    if ( infoWidgetActionFinished )
    {
        if ( infoWidgetOriginal )
        {
            infoWidgetOut();
            infoWidgetOriginal = false;
        }
        else
        {
            infoWidgetIn();
            infoWidgetOriginal = true;
        }
    }
}

//槽：响应信息面板动画完成信号
void MainWindow::infoWidgetFinish()
{
    infoWidgetActionFinished = true;
}

//信息面板移出
void MainWindow::infoWidgetOut()
{
    infoWidgetAnimation->setDuration(450);
    infoWidgetAnimation->setStartValue(QRect(0, 0, 290, 300));
    infoWidgetAnimation->setEndValue(QRect(290, 0, 290, 300));

    infoWidgetActionFinished = false;
    infoWidgetAnimation->start();
}

//信息面板移入
void MainWindow::infoWidgetIn()
{
    infoWidgetAnimation->setDuration(450);
    infoWidgetAnimation->setStartValue(QRect(290, 0, 290, 300));
    infoWidgetAnimation->setEndValue(QRect(0, 0, 290, 300));

    infoWidgetActionFinished = false;
    infoWidgetAnimation->start();
}

//槽：托盘消息点击事件
void MainWindow::onTrayMessageClicked()
{
    if ( ! isNewestVersion )
    {
        // QUrl::isValid方法似乎不起作用，为了安全起见(防止shell执行)，这里使用正则表达式来校验网址
        QRegExp regExp( "(http|https)://([\\w-]+\\.)+[\\w-]+(/[\\w- ./!?%&=]*)?" );

        if ( regExp.exactMatch( updateInfoUrl ) )
        {
            QDesktopServices::openUrl( QUrl(updateInfoUrl) );
        }
    }
    else
    {
        QDesktopServices::openUrl( QUrl(config.blogUrl) );
    }
}

//槽：托盘图标点击事件
void MainWindow::onTrayIconClicked(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason)
    {
        //单击
        case QSystemTrayIcon::Trigger:
            //恢复窗口显示
            this->setWindowState(Qt::WindowActive);
            this->show();
            //trayIcon.hide();
            break;
        //双击
        case QSystemTrayIcon::DoubleClick:
            break;
        default:
            break;
    }
}

//槽：最小化到托盘按钮点击事件
void MainWindow::onToTrayClicked()
{
    //trayIcon.show();
    this->hide();
}

//事件：响应鼠标在控件上按下的事件
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    this->windowPos = this->pos();
    this->mousePos = event->globalPos();
    this->dPos = mousePos - windowPos;
}

//事件：响应鼠标在控件上移动的事件，一般是鼠标按下后移动的事件
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalPos() - this->dPos);
}
