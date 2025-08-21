#include "weather.h"
#include "ui_weather.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QThread>

weather_t today_weather = {"","","","","","","",""};
weather_t feature1_weather;
weather_t feature2_weather;
weather_t feature3_weather;
weather_t feature4_weather;
weather_t* weather_array[5] = {&today_weather, &feature1_weather, &feature2_weather, &feature3_weather, &feature4_weather};

lunar_t lunar_day;

Weather::Weather(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Weather)
{
    ui->setupUi(this);
    this->ui->pushButton->setFocusPolicy(Qt::NoFocus);

    //设置背景颜色
    QPalette palette;
    palette.setColor(QPalette::Background,QColor(97,111,125));
    this->setPalette(palette);

    //1.新建网络请求 管理器
    manager  = new QNetworkAccessManager(this);

    //关联管理器的  finished 信号
    connect(manager, &QNetworkAccessManager::finished,this,&Weather::replyFinished);
    //2.往http://qt-project.org 网站发起请求  %E5%B9%BF%E5%B7%9E 广州

    manager->get(QNetworkRequest(QUrl("http://v3.alapi.cn/api/tianqi/seven?token=dxveeh4hevysubdmv4aq2jgxto2q3d&city=广州")));
}

void prase_weather(Ui::Weather *ui, QString msg);

//today weather
void Weather::replyFinished(QNetworkReply *reply)
{
    //读取所有的网络数据
    QString  msg    =   reply->readAll();
    qDebug() << "-----------weather msg-------------" << msg;
    prase_weather(ui,msg);
}

Weather::~Weather()
{
    delete ui;
}
//back main page
void Weather::on_pushButton_clicked()
{
    this->hide();
    this->parentWidget()->show();
}

//下拉框的内容改变了就触发
void Weather::on_comboBox_currentTextChanged(const QString &arg)
{
    city = arg;
    manager->get(QNetworkRequest(QUrl(QString("http://v3.alapi.cn/api/tianqi/seven"
                                              "?token=dxveeh4hevysubdmv4aq2jgxto2q3d&city=%1").arg(arg))));
}

void prase_weather(Ui::Weather *ui, QString msg)
{
    qDebug() << msg;
    //QString tmp;

    // 解析 JSON
    QJsonDocument doc = QJsonDocument::fromJson(msg.toUtf8());
    if (!doc.isObject()) {
        qDebug() << "Invalid JSON format";
        return;
    }

    QJsonObject rootObj = doc.object();
    if (!rootObj.contains("data") || !rootObj["data"].isArray()) {
        qDebug() << "Missing or invalid 'data' field";
        return;
    }

    QJsonArray dataArray = rootObj["data"].toArray();

    for (int i = 0; i < 5; ++i) {
        if (!dataArray[i].isObject()) continue;

        QJsonObject dataObj = dataArray[i].toObject();

        weather_array[i]->temp = dataObj["temp_day"].toString();
        weather_array[i]->min_temp = dataObj["temp_night"].toString();
        weather_array[i]->max_temp = dataObj["temp_day"].toString();
        weather_array[i]->wind = dataObj["wind_day"].toString();
        weather_array[i]->wind_speed = dataObj["wind_day_level"].toString();
        weather_array[i]->wind_scale = dataObj["wind_day_level"].toString();
        weather_array[i]->weather = dataObj["wea_day"].toString();
        weather_array[i]->update_time = dataObj["date"].toString();
    }

    ui->label_temp->setText(QString("温度: %1°C").arg(today_weather.temp));
    ui->label_temprange->setText(QString("%1 ~ %2°C").arg(today_weather.min_temp).arg(today_weather.max_temp));
    ui->label_weather->setText(today_weather.weather);
    ui->label_wind->setText(QString("%1 %2").arg(today_weather.wind).arg(today_weather.wind_speed));
    ui->label_updatetime->setText(QString("更新时间: %1").arg(today_weather.update_time));

    ui->label_f1_date->setText(feature1_weather.update_time);
    ui->label_f1_range->setText(QString("%1 ~ %2°C").arg(feature1_weather.min_temp).arg(feature1_weather.max_temp));
    ui->label_f1_weather->setText(feature1_weather.weather);
    ui->label_f1_wind->setText(QString("%1 %2").arg(feature1_weather.wind).arg(feature1_weather.wind_scale));

    ui->label_f2_date->setText(feature2_weather.update_time);
    ui->label_f2_range->setText(QString("%1 ~ %2°C").arg(feature2_weather.min_temp).arg(feature2_weather.max_temp));
    ui->label_f2_weather->setText(feature2_weather.weather);
    ui->label_f2_wind->setText(QString("%1 %2").arg(feature2_weather.wind).arg(feature2_weather.wind_scale));

    ui->label_f3_date->setText(feature3_weather.update_time);
    ui->label_f3_range->setText(QString("%1 ~ %2°C").arg(feature3_weather.min_temp).arg(feature3_weather.max_temp));
    ui->label_f3_weather->setText(feature3_weather.weather);
    ui->label_f3_wind->setText(QString("%1 %2").arg(feature3_weather.wind).arg(feature3_weather.wind_scale));

    ui->label_f4_date->setText(feature4_weather.update_time);
    ui->label_f4_range->setText(QString("%1 ~ %2°C").arg(feature4_weather.min_temp).arg(feature4_weather.max_temp));
    ui->label_f4_weather->setText(feature4_weather.weather);
    ui->label_f4_wind->setText(QString("%1 %2").arg(feature4_weather.wind).arg(feature4_weather.wind_scale));

    qDebug() << feature1_weather.weather;
    qDebug() << feature1_weather.update_time;
    qDebug() << feature1_weather.max_temp;
    qDebug() << feature1_weather.min_temp;
    qDebug() << feature1_weather.wind;
    qDebug() << feature1_weather.wind_scale;
    qDebug() << feature1_weather.wind_speed;
    qDebug() << "----------------------------------------";
    qDebug() << feature2_weather.weather;
    qDebug() << feature2_weather.update_time;
    qDebug() << feature2_weather.max_temp;
    qDebug() << feature2_weather.min_temp;
    qDebug() << feature2_weather.wind;
    qDebug() << feature2_weather.wind_scale;
    qDebug() << feature2_weather.wind_speed;
    qDebug() << "----------------------------------------";
    qDebug() << feature3_weather.weather;
    qDebug() << feature3_weather.update_time;
    qDebug() << feature3_weather.max_temp;
    qDebug() << feature3_weather.min_temp;
    qDebug() << feature3_weather.wind;
    qDebug() << feature3_weather.wind_scale;
    qDebug() << feature3_weather.wind_speed;
    qDebug() << "----------------------------------------";
    qDebug() << feature4_weather.weather;
    qDebug() << feature4_weather.update_time;
    qDebug() << feature4_weather.max_temp;
    qDebug() << feature4_weather.min_temp;
    qDebug() << feature4_weather.wind;
    qDebug() << feature4_weather.wind_scale;
    qDebug() << feature4_weather.wind_speed;
    qDebug() << "----------------------------------------";


}



















































