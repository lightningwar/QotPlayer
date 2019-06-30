#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QDir>
#include <QStandardItemModel>
#include <QFileDialog>
#include <QFileInfo>
#include <QDebug>
#include <QKeyEvent>
#include <QApplication>
#include <QtGui>


class QMediaPlayer;
class QVideoWidget;
class QStandardItemModel;



namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget() override;

private:
    //把秒转换成时分秒
    QString changeTime(qint64 position);

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_14_clicked();

    void on_listView_doubleClicked(const QModelIndex &index);

    void updatePosition(qint64 position);

    void on_pushButton_11_clicked();

    void on_pushButton_10_clicked();

    void on_horizontalSlider_sliderMoved();

    void on_horizontalSlider_sliderPressed();

    void on_horizontalSlider_sliderReleased();

    void onTimerOut();

    void on_horizontalSlider_sliderClicked();

    void on_checkBox_toggled(bool checked);

    void on_horizontalSlider_valueChanged(int value);

private:
    Ui::Widget *ui;
    //播放视频
    QMediaPlayer *player;
    //视频播放框
    QVideoWidget *videoWidget;
    //文件名称对应列表
    QStringList strlistFileName;
    //文件对应路径列表
    QStringList strListUrlName;
    //数据模型
    QStandardItemModel *model;
    //窗口设置为随着窗口变化而变化
    virtual void resizeEvent(QResizeEvent *size) override;
    //定时器
    QTimer * timer;

    int maxValue = 1000;

    bool if_reload=false;

protected:
    //方法重写
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;


};

#endif // WIDGET_H

