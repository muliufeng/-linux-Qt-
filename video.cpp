#include "video.h"
#include "ui_video.h"

bool playing_flag = 0;
int current_bright = 0;

bool start_once = 1;

QString total_length; //视频总时长
QString current_pos; //视频当前进度

Video::Video(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Video)
{
    ui->setupUi(this);
    this->ui->pushButton->setFocusPolicy(Qt::NoFocus);
    this->ui->horizontalSlider->setFocusPolicy(Qt::NoFocus);
    this->ui->horizontalSlider_2->setFocusPolicy(Qt::NoFocus);
    this->ui->horizontalSlider_3->setFocusPolicy(Qt::NoFocus);


    //设置背景颜色
    QPalette palette;
    palette.setColor(QPalette::Background,QColor(33,34,36));
    this->setPalette(palette);

    process = new QProcess(this);

    ui->listWidget->addItem("eagle.avi");
    ui->listWidget->addItem("this.avi");
    ui->listWidget->setCurrentRow(0);

    ui->videoWidget->lower(); // 让 videoWidget 降到最下层
    ui->videoWidget->setStyleSheet("background: transparent;");

    ui->videoWidget->setAttribute(Qt::WA_OpaquePaintEvent);
    ui->videoWidget->setAttribute(Qt::WA_NoSystemBackground);
    ui->videoWidget->setAttribute(Qt::WA_PaintOnScreen);
    ui->videoWidget->setAttribute(Qt::WA_TranslucentBackground);
    ui->videoWidget->setAutoFillBackground(false);


    //set volume range
    ui->horizontalSlider->setRange(0,100);
    //se default volume
    ui->horizontalSlider->setValue(80);
    ui->label_volume_percent->setText(QString("%1%").arg(ui->horizontalSlider->value()));


    //set brightness range
    ui->horizontalSlider_2->setRange(0,100);
    //se default brightness
    ui->horizontalSlider_2->setValue(50); current_bright = 50;
    ui->label_brightness_percent->setText(QString("%1%").arg(ui->horizontalSlider_2->value()));


    //获取总时长
    connect(process,&QProcess::readyRead,[=](){

        QString msg = QString(process->readAll());
        QString tmp = msg;

        qDebug() << "[--------------all---------------]" <<tmp;
        if(tmp.indexOf("ANS_LENGTH=") != -1)
        {
            qDebug() << "[--------------length tmp msg---------------]" <<tmp;
            int start = tmp.indexOf(QString("ANS_LENGTH="));

            QString len  = tmp.mid(start+11, 100);
            len.chop(5);

            total_length = len;
            qDebug() << "[--------------length:---------------]" <<total_length;

            QString minute; minute.sprintf("%02d", total_length.toInt()/60);
            QString second; second.sprintf("%02d", total_length.toInt()%60);

            ui->label_total_length->setText(QString("%1:%2").arg(minute).arg(second));
            ui->horizontalSlider_3->setRange(0,total_length.toInt());
        }
        else if(msg.indexOf("ANS_TIME_POSITION=") != -1)
        {
            int start = msg.indexOf(QString("ANS_TIME_POSITION="));
            QString pos  = msg.mid(start+18, 100);
            pos.chop(4);
            current_pos= pos;

            QString minute; minute.sprintf("%02d", current_pos.toInt()/60);
            QString second; second.sprintf("%02d", current_pos.toInt()%60);

            ui->label_current_pos->setText(QString("%1:%2").arg(minute).arg(second));
            ui->horizontalSlider_3->setValue(current_pos.toInt());

            if(current_pos == QString("%1").arg(total_length.toInt()-2))
            {
                on_pushButton_3_clicked();//next song
            }
        }
    });


    timer = new QTimer(this);

    connect(timer,&QTimer::timeout,[=](){

        process->write("get_time_pos\n");
    });
}

Video::~Video()
{
    delete ui;
}

//back main page
void Video::on_pushButton_clicked()
{
    //判断当前进程的状态
    if(process->state() == QProcess::Running)
    {
        //杀死当前进程
        process->kill();
        //回收资源
        process->waitForFinished();
    }
    //this->hide();
    this->parentWidget()->show();
    delete this;   //返回直接删除掉这个页面

    start_once=1;
}

//bool start_stop_flag =
//start video
void Video::on_pushButton_2_clicked()
{


    if(start_once)
    {
        WId winID = ui->videoWidget->winId();
        start_once =0;
        process->start(QString("mplayer -slave -zoom -x 640 -y 380 -osdlevel 0 -wid %1 ./avi/%2").arg(winID).arg(ui->listWidget->currentItem()->text()));
	process->write(QString("volume %1 1\n").arg(ui->horizontalSlider->value()).toStdString().c_str());

        //set playing_flag
        //qDebug()<< "start";   //播放
        playing_flag = 1;

        ui->pushButton_2->setStyleSheet("border-image: url(:/img/pause.png);");


        //获取总时长
        process->write("get_time_length\n");


        timer->start(1000); //ms

        return;
    }

    if(playing_flag)   //暂停
    {
        process->write("pause\n");
        playing_flag = 0;
        //qDebug()<< "stop";
        ui->pushButton_2->setStyleSheet("border-image: url(:/img/play.png);");

        timer->stop();
    }
    else
    {
        process->write("pause\n");   //播放
        playing_flag = 1;
        //qDebug()<< "start";
        ui->pushButton_2->setStyleSheet("border-image: url(:/img/pause.png);");

        timer->start();
    }
}


//skip-forward  next video
void Video::on_pushButton_3_clicked()
{
    if(process->state() == QProcess::Running)
    {
        //杀死当前进程
        process->kill();
        //回收资源
        process->waitForFinished();
    }
    int index = ui->listWidget->currentRow();
    index = (index+1)%2;
    ui->listWidget->setCurrentRow(index);
    WId winID = ui->videoWidget->winId();
    process->start(QString("mplayer -slave -zoom -x 640 -y 380 -osdlevel 0 -wid %1 ./avi/%2").arg(winID).arg(ui->listWidget->currentItem()->text()));
    process->write(QString("volume %1 1\n").arg(ui->horizontalSlider->value()).toStdString().c_str());

    //set playing_flag
    playing_flag = 1;
    ui->pushButton_2->setStyleSheet("border-image: url(:/img/pause.png);");

    //获取总时长
    process->write("get_time_length\n");
}


//skip-backward  prev video
void Video::on_pushButton_4_clicked()
{
    if(process->state() == QProcess::Running)
    {
        //杀死当前进程
        process->kill();
        //回收资源
        process->waitForFinished();
    }
    int index = ui->listWidget->currentRow();
    index = index - 1;
    if(index == -1)
    {
        index = 1;
    }
    WId winID = ui->videoWidget->winId();
    ui->listWidget->setCurrentRow(index);
    process->start(QString("mplayer -slave -zoom -x 640 -y 380 -osdlevel 0 -ao alsa -volume 80 -wid %1 ./avi/%2").arg(winID).arg(ui->listWidget->currentItem()->text()));
    process->write(QString("volume %1 1\n").arg(ui->horizontalSlider->value()).toStdString().c_str());

    //set playing_flag
    playing_flag = 1;
    ui->pushButton_2->setStyleSheet("border-image: url(:/img/pause.png);");

    //获取总时长
    process->write("get_time_length\n");

}

//fast-forward  10s
void Video::on_pushButton_5_clicked()
{
    process->write("seek +10\n");
}

//fast_backward 10s
void Video::on_pushButton_6_clicked()
{
    process->write("seek -10\n");
}

//volume bar value is changed
void Video::on_horizontalSlider_valueChanged(int value)
{
    ui->label_volume_percent->setText(QString("%1%").arg(value));

    if(playing_flag)
    {
        process->write(QString("volume %1 1\n").arg(value).toStdString().c_str());
    }

}

//brightness value change
void Video::on_horizontalSlider_2_valueChanged(int value)
{
    ui->label_brightness_percent->setText(QString("%1%").arg(value));

    int bringhtbess = value;
    if(playing_flag)
    {

        if(  bringhtbess >= current_bright)
        {
            process->write(QString("brightness %1\n").arg((bringhtbess - current_bright)*2).toStdString().c_str());
        }
        else
        {
            process->write(QString("brightness %1\n").arg((bringhtbess - current_bright)*2).toStdString().c_str());
        }

        current_bright = value;
    }
}

//user drag slide move will be triggered
//void Video::on_horizontalSlider_3_sliderMoved(int position)
//{
//    qDebug() << "user drag";

//    timer->stop();
//    if(playing_flag)
//    {

//        if(  position >= current_pos.toInt())
//        {
//            process->write(QString("seek %1\n").arg((position - current_pos.toInt())).toStdString().c_str());
//        }
//        else
//        {
//            process->write(QString("seek %1\n").arg((position - current_pos.toInt())).toStdString().c_str());
//        }

//        current_pos = position;
//    }
//    timer->start();
//}


//process bar press
void Video::on_horizontalSlider_3_sliderPressed()
{
    timer->stop();

}
//process bar release
void Video::on_horizontalSlider_3_sliderReleased()
{
    int pos = ui->horizontalSlider_3->value();

    if(playing_flag)
    {

        if(  pos >= current_pos.toInt())
        {
            process->write(QString("seek %1\n").arg((pos - current_pos.toInt())).toStdString().c_str());
        }
        else
        {
            process->write(QString("seek %1\n").arg((pos - current_pos.toInt())).toStdString().c_str());
        }

        current_pos = pos;
    }
    timer->start();
}
