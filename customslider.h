#ifndef CUSTOMSLIDER_H
#define CUSTOMSLIDER_H
#include <QSlider>
#include <QMouseEvent>
#include <QCoreApplication>


class CustomSlider:public QSlider
{
    Q_OBJECT
public:
    CustomSlider(QWidget *parent = nullptr) : QSlider(parent)
    {
    }
protected:
    //重写QSlider的mousePressEvent事件
    void mousePressEvent(QMouseEvent *ev);
signals:
    //自定义的鼠标单击信号，用于捕获并处理
    void costomSliderClicked();
};

#endif // CUSTOMSLIDER_H

