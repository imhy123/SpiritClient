#if defined(_MSC_VER) && (_MSC_VER >= 1600)
    #pragma execution_character_set("utf-8")
#endif

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QPoint>
#include <QMouseEvent>
#include <QSystemTrayIcon>
#include <QPropertyAnimation>
#include <QDesktopServices>
#include <QTimer>
#include <QTime>
#include <QSharedMemory>
#include <QLocalServer>
#include <QLocalSocket>
#include <QDebug>

#include "configworker.h"
#include "networker.h"
#include "regworker.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public slots:
    void onToTrayClicked();  //最小化到托盘按钮点击事件
    void onTrayIconClicked(QSystemTrayIcon::ActivationReason reason);  //托盘图标点击事件
    void onTrayMessageClicked(); //托盘消息点击事件
    void infoWidgetAction(); //信息面板动画触发事件
    void infoWidgetFinish(); //信息面板动画结束事件
    void singleReceiveMessage(); //LocalServer接收到了消息
    void onAutoLoginChecked(bool);  //自动登录复选框被点击
    void onAutoStartChecked(bool);  //开机自启复选框被点击
    void onAutoReconnectChecked(bool);  //定时重连复选框被点击
    void onChargeUrlClicked(); //交费链接被点击
    void onLoginClicked(); //登录按钮被点击
    void onLogoffClicked(); //下线按钮被点击

    void getStatus(QString text); //在状态栏显示状态信息，设计为槽以接收其它工具类发送过来的消息
    void getStatusCode(int); //接收网络工作类发送过来的状态码
    void setUserInfo(int,QString);  //设置用户的费用等信息
    void getUpdateInfo(QString);  //接收更新信息

    void timerActived();  //响应定时器的消息

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    ConfigWorker config;

    NetWorker netWorker;

    RegWorker regWorker;

    //消息提示框
    QMessageBox msgBox;

    //版本信息
    QString versionStr;

    //定时器，每秒钟唤醒一次，更新界面上的时间
    QTimer timer;

    //用于计录上网时间
    QTime timeRecord;

    //定时重连的时间
    QTime timeAutoReconnect;

    //是否处于定时重连流程中
    bool inAutoReconnect;

    //是否已检查更新
    bool updateChecked;

    //当前是否是最新版本
    bool isNewestVersion;

    QString updateInfoUrl;

    //--用SharedMemory和LocalSocket来实现进程的单例，并能通信要求已有窗口显示。用Qt原生库可以实现跨平台
    QSharedMemory singleSharedMemory;
    QLocalServer* singleLocalServer;
    const int SingleTimeout;
    bool isFirstInstance();
    void createSingleLocalServer();
    void singleSendShowMessage();

    //--信息面板动画--
    QPropertyAnimation *infoWidgetAnimation; //动画在setupUi后才能初始化，因此这里干脆用指针
    void infoWidgetOut();
    void infoWidgetIn();
    bool infoWidgetActionFinished;
    bool infoWidgetOriginal;

    //--托盘图标--
    QSystemTrayIcon trayIcon;

    //--用于窗口拖动--
    QPoint windowPos;
    QPoint mousePos;
    QPoint dPos;

    //--窗口鼠标事件，用于窗口拖动--
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
};

#endif // MAINWINDOW_H
