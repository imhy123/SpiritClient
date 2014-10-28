#if defined(_MSC_VER) && (_MSC_VER >= 1600)
    #pragma execution_character_set("utf-8")
#endif

#ifndef MOUSEWIDGET_H
#define MOUSEWIDGET_H

#include <QWidget>
#include <QStyleOption>
#include <QPainter>

class MouseWidget : public QWidget
{
    Q_OBJECT

signals:
    void mouseEntered();

public:
    explicit MouseWidget(QWidget *parent = 0);
    void paintEvent(QPaintEvent *event);
    void enterEvent(QEvent *event);
};

#endif // MOUSEWIDGET_H
