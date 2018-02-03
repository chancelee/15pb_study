#include "cmyprocess.h"
#include "cmyservice.h"
#include "cmype.h"
#include "cmycloud.h"
#include "cmytrush.h"
#include "cmygrub.h"
#include "cmyvirus.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <windows.h>
#include <PowrProf.h>
#include <QCoreApplication>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_ram_obj(nullptr),
    m_rocket_obj(nullptr)
{
    ui->setupUi(this);
    ui->pushButton->setIcon(QIcon(":/Resources/ico/resource.ico"));
    ui->pushButton->setIconSize(QSize(100, 100));
    ui->pushButton->setStyleSheet("border:none;");

    ui->pushButton_2->setIcon(QIcon(":/Resources/ico/service.ico"));
    ui->pushButton_2->setIconSize(QSize(100, 100));
    ui->pushButton_2->setStyleSheet("border:none;");

    ui->pushButton_3->setIcon(QIcon(":/Resources/ico/ram.ico"));
    ui->pushButton_3->setIconSize(QSize(100, 100));
    ui->pushButton_3->setStyleSheet("border:none;");

    ui->pushButton_4->setIcon(QIcon(":/Resources/ico/pe.ico"));
    ui->pushButton_4->setIconSize(QSize(100, 100));
    ui->pushButton_4->setStyleSheet("border:none;");

    ui->pushButton_5->setIcon(QIcon(":/Resources/ico/rocket.ico"));
    ui->pushButton_5->setIconSize(QSize(100, 100));
    ui->pushButton_5->setStyleSheet("border:none;");

    ui->pushButton_6->setIcon(QIcon(":/Resources/ico/cloud.ico"));
    ui->pushButton_6->setIconSize(QSize(100, 100));
    ui->pushButton_6->setStyleSheet("border:none;");

    ui->pushButton_7->setIcon(QIcon(":/Resources/ico/trash.ico"));
    ui->pushButton_7->setIconSize(QSize(100, 100));
    ui->pushButton_7->setStyleSheet("border:none;");

    ui->pushButton_8->setIcon(QIcon(":/Resources/ico/grub.ico"));
    ui->pushButton_8->setIconSize(QSize(100, 100));
    ui->pushButton_8->setStyleSheet("border:none;");

    ui->pushButton_9->setIcon(QIcon(":/Resources/ico/virus.ico"));
    ui->pushButton_9->setIconSize(QSize(100, 100));
    ui->pushButton_9->setStyleSheet("border:none;");
    //connect(ui->pushButton, &QPushButton::clicked, [](){ qDebug() << "ButtonClicked";});
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::ShowProcess);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::ShowService);
    connect(ui->pushButton_3, &QPushButton::clicked, this, &MainWindow::ShowRam);
    connect(ui->pushButton_4, &QPushButton::clicked, this, &MainWindow::ShowPE);
    connect(ui->pushButton_5, &QPushButton::clicked, this, &MainWindow::ShowRocket);
    connect(ui->pushButton_6, &QPushButton::clicked, this, &MainWindow::ShowCloud);
    connect(ui->pushButton_7, &QPushButton::clicked, this, &MainWindow::ShowTrash);
    connect(ui->pushButton_8, &QPushButton::clicked, this, &MainWindow::ShowGrub);
    connect(ui->pushButton_9, &QPushButton::clicked, this, &MainWindow::ShowVirus);

    connect(ui->shutdown, SIGNAL(triggered()), this, SLOT(my_shutdown()));
    connect(ui->reboot, SIGNAL(triggered()), this, SLOT(my_reboot()));
    connect(ui->logout, SIGNAL(triggered()), this, SLOT(my_logout()));
    connect(ui->sleep, SIGNAL(triggered()), this, SLOT(my_sleep()));
    connect(ui->lockScreen, SIGNAL(triggered()), this, SLOT(my_lock()));

    //this->setAttribute(Qt::WA_TranslucentBackground, true);
    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(0x00,0xff,0x00,0x00));
    setPalette(pal);
}

MainWindow::~MainWindow()
{
    ::UnregisterHotKey((HWND)this->winId(), 0xa001);
    qDebug() << "Success Un-Register Hotkey" << endl;
    delete ui;
}

//bool MainWindow::nativeEventFilter(const QByteArray &eventType, void *message, long *) {
//    qDebug() << "kkkkkk" << endl;

//    return false;
//}

void MainWindow::PriviToken() {
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;
    // Get a token for this process.

    if (!OpenProcessToken(GetCurrentProcess(),
         TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
       return;

    // Get the LUID for the shutdown privilege.

    LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME,
         &tkp.Privileges[0].Luid);

    tkp.PrivilegeCount = 1;  // one privilege to set
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    // Get the shutdown privilege for this process.

    AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,
         (PTOKEN_PRIVILEGES)NULL, 0);

    if (GetLastError() != ERROR_SUCCESS)
       return;
}

void MainWindow::my_shutdown() {
    qDebug() << "Shutdown" << endl;
    PriviToken();
    ExitWindowsEx(EWX_POWEROFF | EWX_FORCE, 0);
}

void MainWindow::my_reboot() {
    qDebug() << "Reboot" << endl;
    PriviToken();
    ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 0);
}

void MainWindow::my_logout() {
    qDebug() << "Logout" << endl;
    ExitWindowsEx(EWX_LOGOFF | EWX_FORCE, SHTDN_REASON_FLAG_USER_DEFINED);
}

void MainWindow::my_sleep() {
    qDebug() << "Sleep" << endl;
    SetSuspendState(FALSE, FALSE, FALSE);
    //SetSuspendState(TRUE, FALSE, FALSE);
}

void MainWindow::my_lock() {
    qDebug() << "LockScreen" << endl;
    LockWorkStation();
}

void MainWindow::DestroyRocket() {
    if (m_rocket_obj) {
        /* Reginald Notice Here */
        //delete m_rocket_obj;
        m_rocket_obj = nullptr;
        qDebug() << "Destroy Rocket" << endl;
    }
}

void MainWindow::ShowProcess() {
    CMyProcess* process_obj = new CMyProcess();
    process_obj->show();
}

void MainWindow::ShowService() {
    CMyService* service_obj = new CMyService();
    service_obj->show();
}

void MainWindow::ShowRam() {
    if (!m_ram_obj) {
        m_ram_obj = new CMyRam();
        m_ram_obj->show();
    } else {
        m_ram_obj->close();
        if (m_ram_obj) {
            delete[] m_ram_obj;
            m_ram_obj = nullptr;
        }
    }
}

void MainWindow::ShowPE() {
    CMyPE* pe_obj = new CMyPE();
    pe_obj->show();
}

void MainWindow::ShowRocket() {
    if (!m_rocket_obj) {
        m_rocket_obj = new CMyRocket();
        m_rocket_obj->show();
        connect(m_rocket_obj, SIGNAL(destroyed()), this, SLOT(DestroyRocket()));
    } else {
        m_rocket_obj->close();
        if (m_rocket_obj) {
            delete[] m_rocket_obj;
            m_rocket_obj = nullptr;
        }
    }
}

void MainWindow::ShowCloud() {
    CMyCloud* cloud_obj = new CMyCloud();
    cloud_obj->show();
}

void MainWindow::ShowTrash() {
    CMyTrush* trash_obj = new CMyTrush();
    trash_obj->show();
}

void MainWindow::ShowGrub() {
    CMyGrub* grub_obj = new CMyGrub();
    grub_obj->show();
}

void MainWindow::ShowVirus() {
    CMyVirus* virus_obj = new CMyVirus();
    virus_obj->show();
}
