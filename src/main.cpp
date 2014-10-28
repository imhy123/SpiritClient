#if defined(_MSC_VER) && (_MSC_VER >= 1600)
    #pragma execution_character_set("utf-8")
#endif

#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;

    w.setWindowFlags(Qt::FramelessWindowHint);

    QStringList argList = QApplication::arguments();
    if ( argList.size() > 1 )
    {
        QString arg1 = argList.at( 1 );
        if ( arg1.compare("/min") == 0 )
        {
            w.hide();
        }
        else
        {
            w.show();
        }
    }
    else
    {
        w.show();
    }

    return a.exec();
}
