#ifndef CMYRAM_H
#define CMYRAM_H

#include <QWidget>
#include <QCloseEvent>
#include <QPushButton>
#include <QTimer>
#include "cmypushbutton.h"

namespace Ui {
class CMyRam;
}

class CMyRam : public QWidget
{
    Q_OBJECT

public:
    explicit CMyRam(QWidget *parent = 0);
    ~CMyRam();

private slots:
    void timerUpdate();

private:
    Ui::CMyRam *ui;
    CMyPushButton* m_pushButton;
    QTimer* m_timer;

    double m_idletime1;
    double m_kerneltime1;
    double m_usertime1;

    double m_idletime2;
    double m_kerneltime2;
    double m_usertime2;

    int m_tick;
};

#endif // CMYRAM_H
