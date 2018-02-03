#include "cmyrocket.h"
#include "ui_cmyrocket.h"
#include <QDebug>
#include <QDesktopWidget>
#include <windows.h>
#include <Psapi.h>

CMyRocket::CMyRocket(QWidget *parent) :
    QWidget(parent, Qt::FramelessWindowHint),
    ui(new Ui::CMyRocket)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_TranslucentBackground, true);

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

    /*Just Test By Reginald */
    m_pushButton = new CMyPushButton(this);
    m_pushButton->setIcon(QIcon(":/Resources/ico/little_rocket.ico"));
    m_pushButton->setIconSize(QSize(160, 160));
    m_pushButton->setStyleSheet("border:none;");
    m_pushButton->setMouseTracking(true);
    m_pushButton->resize(300, 300);

    //m_pushButton->setText("");

    resize(g_nActDeskX, g_nActDeskY);
    m_pushButton->move(g_nActDeskX - 222, g_nActDeskY - 222);

    m_timer = new QTimer(this);

    connect(m_timer, SIGNAL(timeout()), this, SLOT(RocketMove()));
//    connect(m_pushButton, SIGNAL(clicked()), this, SLOT(test()));
    ClearMem();
}

CMyRocket::~CMyRocket()
{
    delete ui;
}

void CMyRocket::RocketMove() {
    m_pushButton->setVisible(true);
    m_pushButton->move(m_pushButton->pos().x(), m_pushButton->pos().y() - 20);

    if (m_pushButton->pos().y() <= 0) {
        m_pushButton->close();
        m_timer->stop();
//        destroy();
//        emit(destroyed());
        delete this;
    }
}

void CMyRocket::ClearMem() {
    /* 获取当前的内存状态 */
    MEMORYSTATUSEX stcMemStatusEx = {};
    stcMemStatusEx.dwLength = sizeof(stcMemStatusEx);

    GlobalMemoryStatusEx(&stcMemStatusEx);

    DWORDLONG preUsedMem = stcMemStatusEx.ullTotalPhys - stcMemStatusEx.ullAvailPhys;

    printf("%I64d MB\n", preUsedMem / 1024 / 1024);

    /* 清理内存 */
    DWORD dwPIDList[1000] = {};
    DWORD bufSize = sizeof(dwPIDList);
    DWORD dwNeedSize = 0;

    EnumProcesses(dwPIDList, bufSize, &dwNeedSize);
    for (DWORD i = 0; i < dwNeedSize / sizeof(DWORD); i++) {
        HANDLE hProcess = OpenProcess(PROCESS_SET_QUOTA, false, dwPIDList[i]);

        SetProcessWorkingSetSize(hProcess, -1, -1);
    }

    /* 获取清理后的内存的状态 */
    GlobalMemoryStatusEx(&stcMemStatusEx);
    DWORD afterCleanUsedMem = stcMemStatusEx.ullTotalPhys - stcMemStatusEx.ullAvailPhys;

    printf("%ld MB\n", afterCleanUsedMem / 1024 / 1024);

    printf("%I64d MB\n", (preUsedMem - afterCleanUsedMem) / 1024 / 1024);

    m_timer->start(30);
}
