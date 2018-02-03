#ifndef CMYCLOUD_H
#define CMYCLOUD_H

#include <QWidget>
#include <QHostAddress>
#include <QTcpSocket>
#include <QCloseEvent>

namespace Ui {
class CMyCloud;
}

class CMyCloud : public QWidget
{
    Q_OBJECT

    typedef struct _MYMESSAGE_HEADER {
        int messageType;
        int messageSize;
    }MYMESSAGE_HEADER;

    typedef struct _MYMESSAGE {
        MYMESSAGE_HEADER messageHeader;
        char* messageContent;
    }MYMESSAGE;

    enum MessageType {
        sendRegister = 1,
        sendLogin,
        sendMessage_room,
        sendPic_room,
        sendMessage_single,
        sendPic_single,
        getUserList,
        getdFriendList,
        addFriends,
        other, // Remote Help
        client_close,
        virus_md5_reg,
        virus_md5_query,
    };

public:
    explicit CMyCloud(QWidget *parent = 0);
    ~CMyCloud();

private slots:
    /* Send Virus To Cloud */
    void on_pushButton_clicked();
    /* Query Virus From Cloud */
    void on_pushButton_2_clicked();
    /* Recv Msg From Server */
    void readyReadSlot();

protected:
    void closeEvent(QCloseEvent* event);

private:
    Ui::CMyCloud *ui;
    QTcpSocket* m_client;
};

#endif // CMYCLOUD_H
