#include "client.h"
#include <QDebug>

const int WIDTH = 640;
const int HEIGHT = 480;

Client::Client(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    label = new QLabel(this);
    layout->addWidget(label);
    setLayout(layout);

    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::readyRead, this, &Client::readData);
    connect(socket, &QTcpSocket::connected, this, []() {
        qDebug() << "Connected to server";
    });
    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error), this, [](QAbstractSocket::SocketError socketError) {
        qDebug() << "Socket error:" << socketError;
    });

    qDebug() << "Connecting to server...";
    socket->connectToHost("192.168.250.100", 8080); // 设置为虚拟机的 IP 地址和端口

    if (!socket->waitForConnected(5000)) {  // 等待5秒连接
        qDebug() << "Connection failed:" << socket->errorString();
    } else {
        qDebug() << "Connection successful";
    }

    qDebug() << "Client window created";
    setWindowTitle("Video Display"); // 设置窗口标题
    resize(WIDTH, HEIGHT); // 设置窗口大小为视频分辨率
}

void Client::readData() {
    buffer.append(socket->readAll());
    qDebug() << "Data received. Current buffer size:" << buffer.size();
    int frameSize = WIDTH * HEIGHT * 2; // YUYV格式每个像素2字节
    while (buffer.size() >= frameSize) {
        QByteArray frameData = buffer.left(frameSize);
        buffer.remove(0, frameSize);

        uchar *yuyvData = reinterpret_cast<uchar*>(frameData.data());
        uchar *rgbData = new uchar[WIDTH * HEIGHT * 3];

        // 将YUYV数据转换为RGB888
        yuyvToRgb888(yuyvData, rgbData, WIDTH, HEIGHT);

        QImage image(rgbData, WIDTH, HEIGHT, QImage::Format_RGB888);
        if (image.isNull()) {
            qDebug() << "Failed to create image";
        } else {
            label->setPixmap(QPixmap::fromImage(image));
            label->show(); // 显示标签
        }

        delete[] rgbData;
    }
}

void Client::yuyvToRgb888(const uchar *yuyv, uchar *rgb, int width, int height) {
    for (int i = 0; i < width * height; i += 2) {
        int y0 = yuyv[i * 2 + 0] & 0xff;
        int u  = yuyv[i * 2 + 1] & 0xff;
        int y1 = yuyv[i * 2 + 2] & 0xff;
        int v  = yuyv[i * 2 + 3] & 0xff;

        int c0 = y0 - 16;
        int c1 = y1 - 16;
        int d = u - 128;
        int e = v - 128;

        int r0 = (298 * c0 + 409 * e + 128) >> 8;
        int g0 = (298 * c0 - 100 * d - 208 * e + 128) >> 8;
        int b0 = (298 * c0 + 516 * d + 128) >> 8;

        int r1 = (298 * c1 + 409 * e + 128) >> 8;
        int g1 = (298 * c1 - 100 * d - 208 * e + 128) >> 8;
        int b1 = (298 * c1 + 516 * d + 128) >> 8;

        rgb[i * 3 + 0] = qBound(0, r0, 255);
        rgb[i * 3 + 1] = qBound(0, g0, 255);
        rgb[i * 3 + 2] = qBound(0, b0, 255);

        rgb[(i + 1) * 3 + 0] = qBound(0, r1, 255);
        rgb[(i + 1) * 3 + 1] = qBound(0, g1, 255);
        rgb[(i + 1) * 3 + 2] = qBound(0, b1, 255);
    }
}
