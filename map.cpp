#include "map.h"
#include "ui_map.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

int zoom = 10;
int zoom_prev;

QString currentLocation; // 保存经纬度

Map::Map(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Map)
{
    ui->setupUi(this);
    this->ui->pushButton->setFocusPolicy(Qt::NoFocus);

    manager = new QNetworkAccessManager(this);

    // 第一步：获取当前 IP 的定位信息
    QString locationUrl = QString("https://api.map.baidu.com/location/ip?ak=NXtV6WV9MtjhvTVmk5rLt911TkRBE38w&coor=bd09ll");
    QNetworkRequest locRequest{QUrl(locationUrl)};  // 使用大括号初始化
    QNetworkReply *locReply = manager->get(locRequest);  // 现在可以正确传递

    connect(locReply, &QNetworkReply::finished, this, [=]() {
        QByteArray data = locReply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isObject()) {
            QJsonObject obj = doc.object();
            if (obj.contains("content")) {
                QJsonObject content = obj.value("content").toObject();
                QJsonObject point = content.value("point").toObject();
                QString x = point.value("x").toString();
                QString y = point.value("y").toString();
                currentLocation = QString("%1,%2").arg(x).arg(y);
                qDebug() << "定位成功:" << currentLocation;
                loadMap(currentLocation, zoom);
            }
        }
        locReply->deleteLater();
    });
}

Map::~Map()
{
    delete ui;
}

void Map::loadMap(const QString &center, int zoomLevel)
{
    request = new QNetworkRequest();
    request->setUrl(QUrl(QString("https://api.map.baidu.com/staticimage?center=%1&markers=%1&width=800&height=480&zoom=%2&ak=NXtV6WV9MtjhvTVmk5rLt911TkRBE38w")
                          .arg(center).arg(zoomLevel)));

    reply = manager->get(*request);
    connect(reply, &QIODevice::readyRead, this, &Map::slot_ready_read);
    connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(slot_download(qint64,qint64)));

    file = new QFile("./static.png");
    if(file->open(QIODevice::ReadWrite)) {
        qDebug() << "文件打开成功";
    } else {
        qDebug() << "文件打开失败";
    }
}

void Map::slot_ready_read()
{
    file->write(reply->readAll());
}

void Map::slot_download(qint64 current, qint64 total)
{
    if(total < 100) {
        zoom = zoom_prev;
        return;
    }
    if(current == total) {
        file->close();
        QPalette palette;
        palette.setBrush(QPalette::Window, QBrush(QPixmap("./static.png")));  // 使用 Window 替代 Background
        this->setPalette(palette);
    }
}

void Map::on_pushButton_clicked()
{
    this->hide();
    this->parentWidget()->show();
}

void Map::on_pushButton_2_clicked()
{
    zoom_prev = zoom;
    zoom += 3;
    if(zoom > 19) zoom = 19;
    loadMap(currentLocation, zoom);
}

void Map::on_pushButton_3_clicked()
{
    zoom_prev = zoom;
    zoom -= 3;
    if(zoom < 10) zoom = 10;
    loadMap(currentLocation, zoom);
}

void Map::on_comboBox_currentTextChanged(const QString &arg)
{
    currentLocation = arg;
    loadMap(currentLocation, zoom);
}