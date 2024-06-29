#ifndef CLIENT_H
#define CLIENT_H

#include <QWidget>
#include <QLabel>
#include <QTcpSocket>
#include <QVBoxLayout>

class Client : public QWidget {
    Q_OBJECT

public:
    explicit Client(QWidget *parent = nullptr);

private slots:
    void readData();

private:
    QLabel *label;
    QTcpSocket *socket;
    QByteArray buffer; // 用于存储从服务器接收的数据
    void yuyvToRgb888(const uchar *yuyv, uchar *rgb, int width, int height); // 添加YUYV到RGB888的转换函数
};

#endif // CLIENT_H
