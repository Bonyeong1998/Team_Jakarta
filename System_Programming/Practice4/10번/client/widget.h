#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

namespace Ui {
class Widget;
}

class QTcpSocket;

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void readyRead(); //서버로부터 메시지가 온경우
    void connected(); //연결이 완료되었으면 서버에게 연결되었다고 알림
    void on_connectButton_clicked(); //연결 버튼을 누름
    void on_sendButton_clicked(); //채팅 메세지 전송 버튼을 누름

    void on_messageLineEdit_returnPressed();

private:
    Ui::Widget *ui;
    QTcpSocket* socket; //나의 소켓
};

#endif // WIDGET_H
