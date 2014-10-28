#if defined(_MSC_VER) && (_MSC_VER >= 1600)
    #pragma execution_character_set("utf-8")
#endif

#include "mousewidget.h"

MouseWidget::MouseWidget(QWidget *parent) :
    QWidget(parent)
{
}

void MouseWidget::paintEvent(QPaintEvent *event)
{
     QStyleOption opt;
     opt.init(this);
     QPainter p(this);
     style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void MouseWidget::enterEvent(QEvent *event)
{
    emit mouseEntered();
}
