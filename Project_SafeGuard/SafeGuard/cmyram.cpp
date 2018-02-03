#include "cmyram.h"
#include "ui_cmyram.h"
#include <QSplashScreen>
#include <QPixmap>
#include <QBitmap>
#include <windows.h>
#include <QDebug>
#include <QDesktopWidget>

CMyRam::CMyRam(QWidget *parent) :
    QWidget(parent, Qt::FramelessWindowHint),
    ui(new Ui::CMyRam),
    m_tick(0)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_TranslucentBackground, true);

    m_pushButton = new CMyPushButton(this);
    m_pushButton->setIcon(QIcon(":/Resources/png/eye.png"));
    m_pushButton->setIconSize(QSize(160, 160));
    m_pushButton->setStyleSheet("border:none;");
    m_pushButton->setMouseTracking(true);
    m_pushButton->resize(300, 300);

    m_pushButton->setText("CPU");


    QDesktopWidget* desktopWidget = QApplication::desktop();
    //获取可用桌面大小
    QRect deskRect = desktopWidget->availableGeometry();
    //获取设备屏幕大小
    QRect screenRect = desktopWidget->screenGeometry();

    int g_nActScreenX = screenRect.width();
    int g_nActScreenY = screenRect.height();

    int g_nActDeskX = deskRect.width();
    int g_nActDeskY = deskRect.height();

    int scrcount = desktopWidget->screenCount();

    char buf[128] = {};
    sprintf_s(buf, 128, "Screen:(%d, %d), Desk(%d, %d), Count(%d)",
              g_nActScreenX, g_nActScreenY, g_nActDeskX, g_nActDeskY, scrcount);

    qDebug() << QString(buf);


    resize(g_nActDeskX, g_nActDeskY);
    m_pushButton->move(g_nActDeskX - 222, g_nActDeskY - 222);

    m_timer = new QTimer(this);

    connect(m_timer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
//    connect(m_pushButton, SIGNAL(clicked()), this, SLOT(test()));

    m_timer->start(1000);
}

CMyRam::~CMyRam() {
    delete ui;
}

void CMyRam::timerUpdate() {
    m_timer->stop();

    FILETIME idleTime, kernelTime, userTime;
    GetSystemTimes(&idleTime, &kernelTime, &userTime);

    if (!m_tick) {
        m_idletime1 = (double)(idleTime.dwHighDateTime * 4.294967296E9) + (double)(idleTime.dwLowDateTime);
        m_kerneltime1 = (double)(kernelTime.dwHighDateTime * 4.294967296E9) + (double)(kernelTime.dwLowDateTime);
        m_usertime1 = (double)(userTime.dwHighDateTime * 4.294967296E9) + (double)(userTime.dwLowDateTime);
        m_tick++;
    } else {
        m_idletime2 = (double)(idleTime.dwHighDateTime * 4.294967296E9) + (double)(idleTime.dwLowDateTime);
        m_kerneltime2 = (double)(kernelTime.dwHighDateTime * 4.294967296E9) + (double)(kernelTime.dwLowDateTime);
        m_usertime2 = (double)(userTime.dwHighDateTime * 4.294967296E9) + (double)(userTime.dwLowDateTime);
        m_tick = 0;

        double cpu_usage = 100.0 - ((m_idletime2 - m_idletime1) /
                ((m_kerneltime2 - m_kerneltime1) + (m_usertime2 - m_usertime1))) * 100.0;

        MEMORYSTATUS mem_usage = {};
        GlobalMemoryStatus(&mem_usage);

        char buf[128] = {};
        sprintf_s(buf, 128, "%lf%%(%d%%)", cpu_usage, mem_usage.dwMemoryLoad);
        m_pushButton->setText(QString(buf));

    }

    m_timer->start(1000);
}
