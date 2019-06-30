#include "widget.h"
#include "ui_widget.h"
#include "customslider.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    //激活键盘
    this->grabKeyboard();
    //激活UI界面
    ui->setupUi(this);

    player = new QMediaPlayer(this);

    videoWidget = new QVideoWidget(this);

    //将视频流自适应视频控件
    player -> setVideoOutput(videoWidget);

    connect(player, &QMediaPlayer::positionChanged, this, &Widget::updatePosition);

    //亮度
    ui -> horizontalSlider_2 -> setValue(videoWidget -> brightness());
    connect(ui -> horizontalSlider_2, &QSlider::sliderMoved, videoWidget, &QVideoWidget::setBrightness);
    connect(videoWidget, &QVideoWidget::brightnessChanged, ui -> horizontalSlider_2, &QSlider::value);

    //色相
    ui -> horizontalSlider_4 -> setValue(videoWidget -> hue());
    connect(ui -> horizontalSlider_4, &QSlider::sliderMoved, videoWidget, &QVideoWidget::setHue);
    connect(videoWidget, &QVideoWidget::hueChanged, ui -> horizontalSlider_4, &QSlider::value);

    //饱和度
    ui -> horizontalSlider_5 -> setValue(videoWidget -> saturation());
    connect(ui -> horizontalSlider_5, &QSlider::sliderMoved, videoWidget, &QVideoWidget::setSaturation);
    connect(videoWidget, &QVideoWidget::saturationChanged, ui -> horizontalSlider_5, &QSlider::value);

    //音量
    ui -> horizontalSlider_3 -> setValue(player -> volume());
    connect(ui -> horizontalSlider_3, &QSlider::sliderMoved, player, &QMediaPlayer::setVolume);
    connect(player, &QMediaPlayer::volumeChanged, ui -> horizontalSlider_3, &QSlider::value);

//    进度条
//    ui -> horizontalSlider -> setValue(player -> Position());
//    connect(ui -> horizontalSlider, &QSlider::sliderMoved, player, &QMediaPlayer::setPosition);
//    connect(player, &QMediaPlayer::positionChanged, ui -> horizontalSlider, &QSlider::value);

    ui->horizontalSlider->setEnabled(false);
    connect(ui->horizontalSlider,&CustomSlider::sliderPressed,this,&Widget::on_horizontalSlider_sliderPressed);
    connect(ui->horizontalSlider,&CustomSlider::sliderMoved,this,&Widget::on_horizontalSlider_sliderMoved);
    connect(ui->horizontalSlider,&CustomSlider::sliderReleased,this,&Widget::on_horizontalSlider_sliderReleased);

    //设置进度条最大值
    ui->horizontalSlider->setRange(0,maxValue);

    //数据模型
    model = new QStandardItemModel(this);
    strlistFileName.clear();
    strListUrlName.clear();

    //禁止编辑
    ui -> listView -> setEditTriggers(QAbstractItemView::NoEditTriggers);
}

Widget::~Widget()
{
    delete player;
    delete timer;
    delete videoWidget;
    delete ui;
}


//控件自适应（方法重写），窗口可随意伸缩，视频控件也跟随自适应
void Widget::resizeEvent(QResizeEvent *size)
{
    //获取graphicsView控件几何
    //qDebug()<<ui -> graphicsView -> geometry();
    //坐标，大小自适应
    videoWidget->setGeometry(ui -> graphicsView -> geometry().x() + 3, ui -> graphicsView -> geometry().y() + 3, ui -> graphicsView -> geometry().width() - 6, ui -> graphicsView -> geometry().height() - 6);
}


//更新进度条显示并更新播放时间
void Widget::updatePosition(qint64 position)
{
    //对应到秒
    //进度条滚动（已抛弃）
    //ui -> horizontalSlider -> setMaximum(player -> duration() / 1000);
    //ui -> horizontalSlider -> setValue(position / 1000);

    //播放进度时间
    ui -> label_3 -> setText("            " + changeTime(player -> duration() / 1000));
    ui -> label -> setText(changeTime(position / 1000));
}


//把秒转换成时分秒
QString Widget::changeTime(qint64 position)
{
    int min = int (position / 60);
    int second = position % 60;
    int hour = 0;
    if(min >= 60)
    {
        hour = min / 60;
        min = min % 60;
    }

    QString strhour = (hour > 9) ? QString::number(hour) : tr("0") + QString::number(hour);
    QString strmin = (min > 9) ? QString::number(min) : tr("0") + QString::number(min);
    QString strsecond = (second > 9) ? QString::number(second) : tr("0") + QString::number(second);

    return strhour + ":" + strmin + ":" + strsecond;
}


//点击播放
void Widget::on_pushButton_clicked()
{   if(!player->media().isNull())
    {
        //ui->horizontalSlider->setEnabled(true);
        if(player->position() == 0)
        {
            delete timer;
            timer = new QTimer();
            //如果想看起来流畅些，可以把时间间隔调小
            timer -> setInterval(50);
            timer -> start();
            //将timer连接至onTimerOut槽函数
            connect(timer, SIGNAL(timeout()), this, SLOT(onTimerOut()));
        }
        player -> play();

        ui -> label_2 -> setText("      播放状态");
        ui -> pushButton_2 -> setEnabled(true);
    }
}

//点击暂停
void Widget::on_pushButton_2_clicked()
{
    if(!player->media().isNull())
    {
        player -> pause();
        ui -> label_2 -> setText("      暂停状态");
    }
}

//点击结束
void Widget::on_pushButton_3_clicked()
{
    if(!player->media().isNull())
    {
        player -> stop();
        ui -> label_2 -> setText("      停止状态");
        if(player -> state() == QMediaPlayer::StoppedState)
            ui -> pushButton_2 -> setEnabled(false);
    }
}


//打开文件
void Widget::on_pushButton_5_clicked()
{
    //选择视频文件
    QFileInfo filename = QFileDialog::getOpenFileName(this, tr("选择视频文件"), ".", tr("视频格式(*.avi *.mp4 *.flv *.wmv *.rmvb *mov) ;; 音频格式(*.mp3 *.wav *.aiff *.midi *flac *aac) ;; 所有文件(*.*)"));

    if(filename.path()!="")
    {
        if(!player->media().isNull())
        {
            delete timer;
            ui -> horizontalSlider -> setValue(0);
            player->setPosition(ui -> horizontalSlider -> value());
        }
        player -> setMedia(QUrl::fromLocalFile(filename.filePath()));
        ui -> horizontalSlider->setEnabled(true);

        //文件存放在列表
        QStringList num;
        num<<QString(filename.fileName());

        QStringListModel *model = new QStringListModel(num);

        strListUrlName.append(filename.filePath());
        ui -> listView ->setModel(model);

        timer = new QTimer();
        player -> play();
        //如果想看起来流畅些，可以把时间间隔调小
        timer -> setInterval(50);
        timer -> start();
        //将timer连接至onTimerOut槽函数
        connect(timer, SIGNAL(timeout()), this, SLOT(onTimerOut()));

        ui -> label_2 -> setText("      播放状态");
    }
}


//打开目录
void Widget::on_pushButton_7_clicked()
{
    //点击打开目录时清空列表
    strlistFileName.clear();
    strListUrlName.clear();
    model -> clear();
    //player -> stop();

    //选择文件夹
    QString dirname = QFileDialog::getExistingDirectory(this, tr("打开目录", "."));
    QDir dir(dirname);
    QStringList SupportedVideoType;
    //文件类型，因平台不同，解码器需要根据需要安装
    SupportedVideoType
        << QStringLiteral("*.mp3")
        << QStringLiteral("*.wav")
        << QStringLiteral("*.aiff")
        << QStringLiteral("*.midi")
        << QStringLiteral("*.aac")
        << QStringLiteral("*.mp4")
        << QStringLiteral("*.wmv")
        << QStringLiteral("*.avi")
        << QStringLiteral("*.mov")
        << QStringLiteral("*.flv")
        << QStringLiteral("*.rmvb");
    dir.setNameFilters(QStringList(SupportedVideoType));
    //获取文件名列表
    strlistFileName = dir.entryList();
    //获取目录路径
    QString strPathName = dir.absolutePath();
    //文件个数
    int count = strlistFileName.count();

    for(int i = 0; i < count; i++)
    {
        QStandardItem *item = new QStandardItem(strlistFileName.at(i));
        model -> appendRow(item);
        //文件路径+文件名称
        strListUrlName.append(strPathName + "/" + strlistFileName.at(i));
    }
    //文件存放在列表
    ui -> listView -> setModel(model);
}


//全屏
void Widget::on_pushButton_14_clicked()
{
    videoWidget -> setFullScreen(true);

}


//按键
//Press和Release同时进行
void Widget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
        //F11退出全屏
        case Qt::Key_F11:
            if(videoWidget->isFullScreen())
            {
                videoWidget -> setFullScreen(false);
                resizeEvent(nullptr);
            }
            break;
        //Space控制播放/暂停
        case Qt::Key_Space:
            if(player -> state() == QMediaPlayer::PlayingState)
            {
                player -> pause();
                ui -> label_2 -> setText("      暂停状态");

            }
            else if(player -> state() == QMediaPlayer::PausedState)
            {
                player->play();
                ui -> label_2 -> setText("      播放状态");
            }
            break;
        default:
            break;


    }
}

void Widget::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key())
    {
        //F11退出全屏
        case Qt::Key_F11:
            if(videoWidget->isFullScreen())
            {
                videoWidget -> setFullScreen(false);
                resizeEvent(nullptr);
            }
            break;
//        //Space控制播放/暂停
//        case Qt::Key_Space:
//            if(player -> state() == QMediaPlayer::PlayingState)
//            {
//                player -> pause();
//                ui -> label_2 -> setText("      暂停状态");
//            }
//            else
//            {
//                player->play();
//                ui -> label_2 -> setText("      播放状态");
//            }
//        break;
        default:
            break;
    }
}


//定时器
void Widget::onTimerOut()
{
    ui->horizontalSlider->setValue(player->position()*maxValue/player->duration());
}


//双击列表打开文件
void Widget::on_listView_doubleClicked(const QModelIndex &index)
{
    ui -> horizontalSlider->setEnabled(true);
    //非第一次双击
    if(!player->media().isNull())
    {
        delete timer;
        ui -> horizontalSlider -> setValue(0);
        player->setPosition(ui -> horizontalSlider -> value());
    }

    player -> setMedia(QUrl::fromLocalFile(strListUrlName.at(index.row())));

    timer = new QTimer();
    //如果想看起来流畅些，可以把时间间隔调小，如100ms
    timer->setInterval(50);
    timer->start();
    //将timer连接至onTimerOut槽函数
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimerOut()));
    player -> play();
    ui -> label_2 -> setText("      播放状态");
}


//拖拽播放进度条
//move和released同时进行
void Widget::on_horizontalSlider_sliderMoved()
{
    timer->stop();
    player->pause();
    player->setPosition(ui -> horizontalSlider -> value() * player -> duration() / maxValue);
}

void Widget::on_horizontalSlider_sliderReleased()
{
    player->play();
    timer->start();
    player->setPosition(ui -> horizontalSlider -> value() * player -> duration() / maxValue);
}


void Widget::on_horizontalSlider_sliderClicked()
{
    player->setPosition(ui -> horizontalSlider -> value() * player -> duration() / maxValue);
}


//未实现的进度条“指哪打哪”
void Widget::on_horizontalSlider_sliderPressed()
{
    player -> pause();
    player->setPosition(ui->horizontalSlider->value()*player->duration()/maxValue);
}


//快进
void Widget::on_pushButton_11_clicked()
{
    //快进5秒（5000毫秒）
    if(player -> position() < player -> duration() - 5000)
        player -> setPosition(ui -> horizontalSlider -> value() * player -> duration() / maxValue + 5000);
    if(player -> position() >= player -> duration() - 5000 && player -> position() <= player -> duration())
        player -> setPosition(player -> duration());
}

//快退
void Widget::on_pushButton_10_clicked()
{
    //快退5秒（5000毫秒）
    player->setPosition(ui->horizontalSlider->value()*player->duration()/maxValue - 5000);
}


//静音
void Widget::on_checkBox_toggled(bool checked)
{
    if(checked == 0)
        player ->setMuted(false);
    if(checked == 1)
        player ->setMuted(true);
}


//监测进度条
void Widget::on_horizontalSlider_valueChanged(int value)
{
//    qDebug()<<player->position();
//    qDebug()<<player->duration();
    //播放完
    if(player->position() == player->duration())
    {
        player->stop();
//        delete player;
//        delete timer;
    }
}

