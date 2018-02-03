#include "cmycloud.h"
#include "ui_cmycloud.h"
#include <QDebug>

CMyCloud::CMyCloud(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CMyCloud)
{
    ui->setupUi(this);

    ui->pushButton->setIcon(QIcon(":/Resources/ico/send.ico"));
    ui->pushButton->setIconSize(QSize(100, 100));
    ui->pushButton->setStyleSheet("border:none;");

    ui->pushButton_2->setIcon(QIcon(":/Resources/ico/recv.ico"));
    ui->pushButton_2->setIconSize(QSize(100, 100));
    ui->pushButton_2->setStyleSheet("border:none;");

    this->setStyleSheet("background-color:rgb(255,255,181)");

    m_client = new QTcpSocket(this);
    m_client->connectToHost(QHostAddress("127.0.0.1"), 27015);
    connect(m_client, SIGNAL(readyRead()), this, SLOT(readyReadSlot()));
    //client->write(data);
}

void CMyCloud::readyReadSlot() {
    MYMESSAGE msg = {};

    m_client->read((char*)&msg.messageHeader, sizeof(MYMESSAGE_HEADER));

    if (msg.messageHeader.messageSize) {
        msg.messageContent = new char[msg.messageHeader.messageSize + 1]{};
        m_client->read(msg.messageContent, msg.messageHeader.messageSize);

//        qDebug() << msg.messageContent << endl;
//        qDebug() << QString(msg.messageContent) << endl;
        ui->textEdit->setText(msg.messageContent);

        if (msg.messageContent) {
            delete[] msg.messageContent;
            msg.messageContent = nullptr;
        }
    }
}

void CMyCloud::closeEvent(QCloseEvent* event) {
    m_client->close();
    event->accept();
}

CMyCloud::~CMyCloud()
{
    delete ui;
}

/* Send Virus To Cloud */
void CMyCloud::on_pushButton_clicked() {
    if (ui->lineEdit->text().size()) {
        MYMESSAGE msg = {};

        msg.messageHeader.messageType = virus_md5_reg;
        msg.messageContent = new char[ui->lineEdit->text().size() + 1]{};
//        qDebug() << ui->lineEdit->text();
//        qDebug() << ui->lineEdit->text().toStdString().c_str();
//        qDebug() << ui->lineEdit->text().size();
//        qDebug() << strlen(ui->lineEdit->text().toStdString().c_str());
        strcpy_s(msg.messageContent,
                 ui->lineEdit->text().size() + 1,
                 ui->lineEdit->text().toStdString().c_str());
        msg.messageHeader.messageSize = strlen(msg.messageContent);

        m_client->write((char*)&msg.messageHeader, sizeof(MYMESSAGE_HEADER));
        m_client->write(msg.messageContent, msg.messageHeader.messageSize);

        if (msg.messageContent) {
            delete[] msg.messageContent;
            msg.messageContent = nullptr;
        }
    } else {
        ui->textEdit->setText("Can't NULL");
    }
}

/* Query Virus From Cloud */
void CMyCloud::on_pushButton_2_clicked() {
    if (ui->lineEdit->text().size()) {
        MYMESSAGE msg = {};

        msg.messageHeader.messageType = virus_md5_query;
        msg.messageContent = new char[ui->lineEdit->text().size() + 1]{};
//        qDebug() << ui->lineEdit->text();
//        qDebug() << ui->lineEdit->text().toStdString().c_str();
//        qDebug() << ui->lineEdit->text().size();
//        qDebug() << strlen(ui->lineEdit->text().toStdString().c_str());
        strcpy_s(msg.messageContent,
                 ui->lineEdit->text().size() + 1,
                 ui->lineEdit->text().toStdString().c_str());
        msg.messageHeader.messageSize = strlen(msg.messageContent);

        m_client->write((char*)&msg.messageHeader, sizeof(MYMESSAGE_HEADER));
        m_client->write(msg.messageContent, msg.messageHeader.messageSize);

        if (msg.messageContent) {
            delete[] msg.messageContent;
            msg.messageContent = nullptr;
        }
    } else {
        ui->textEdit->setText("Can't NULL");
    }
}

