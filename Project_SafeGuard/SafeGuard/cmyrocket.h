#ifndef CMYROCKET_H
#define CMYROCKET_H

#include "cmypushbutton.h"

#include <QTimer>
#include <QWidget>

namespace Ui {
class CMyRocket;
}

class CMyRocket : public QWidget
{
    Q_OBJECT

public:
    explicit CMyRocket(QWidget *parent = 0);
    ~CMyRocket();
    void ClearMem();

private slots:
    void RocketMove();

private:
    Ui::CMyRocket *ui;
    CMyPushButton* m_pushButton;
    QTimer* m_timer;
};

#endif // CMYROCKET_H
