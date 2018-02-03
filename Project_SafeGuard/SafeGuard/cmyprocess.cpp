#include "cmyprocess.h"
#include "ui_cmyprocess.h"
#include "csnapshot.h"
//#include "cpuusage.h"
#include <QComboBox>
#include <vector>
#include <Psapi.h>
#include <QDebug>
#include <QMenu>

using std::vector;
bool CALLBACK WndEnumProc(HWND hWnd, LPARAM lParam);

CMyProcess::CMyProcess(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CMyProcess),
    m_curItem_terminate(nullptr)
{
    ui->setupUi(this);

    m_act_process = new QAction(u8"关闭进程", this);
    connect(m_act_process, SIGNAL(triggered()), this, SLOT(trigerTerminateProcess()));

    QTreeWidgetItem* headerItems = new QTreeWidgetItem;
    headerItems->setText( 0 , u8"名称" );
    headerItems->setText( 1 , u8"PID" );
    headerItems->setText( 2 , u8"CPU" );
    headerItems->setText( 3 , u8"内存" );
    headerItems->setText( 4 , u8"磁盘" );
    headerItems->setText( 5 , u8"网络" );
    ui->treeWidget_Process->setHeaderItem( headerItems );

    CSnapShot snap_obj;

    if (snap_obj.ProcessRoundRobin() && snap_obj.ThreadRoundRobin()) {
        for (unsigned int i = 0; i < snap_obj.m_Process_vec.size(); i++) {
            wchar_t t_pid[10] = {};
            wchar_t t_cpu[30] = {};

            if (snap_obj.m_Process_vec[i].th32ProcessID) {
//                CpuUsage cu(snap_obj.m_Process_vec[i].th32ProcessID);
                _snwprintf_s(t_cpu, 30, L"%d%%",  0);
            }
            _snwprintf_s(t_pid, 10, L"%d", snap_obj.m_Process_vec[i].th32ProcessID);
//            m_ListCtrl.InsertOnLineItem(2, snap_obj.m_Thread[i].szExeFile, t_pid);
            QTreeWidgetItem* item = new QTreeWidgetItem;
            item->setText( 0 , QString::fromWCharArray(snap_obj.m_Process_vec[i].szExeFile));
            item->setText( 1 , QString::fromWCharArray(t_pid));

            if (snap_obj.m_Process_vec[i].th32ProcessID) {
                item->setText( 2 , QString::fromWCharArray(t_cpu));
            }

            ui->treeWidget_Process->addTopLevelItem(item);

            DWORD t_ThreadCount = 0;
            for (unsigned int j = 0; j < snap_obj.m_Thread_vec.size(); j++) {
                if (snap_obj.m_Thread_vec[j].th32OwnerProcessID == snap_obj.m_Process_vec[i].th32ProcessID) {
                    wchar_t t_ThreadId[10] = {};
                    _snwprintf_s(t_ThreadId, 10, L"%d", snap_obj.m_Thread_vec[j].th32ThreadID);
                    QTreeWidgetItem* subItem = new QTreeWidgetItem;
                    subItem->setText( 0 , QString::number(t_ThreadCount++, 10) );
                    subItem->setText( 1 , QString::fromWCharArray(t_ThreadId) );
                    item->addChild( subItem );
                }
            }
        }
    }

    /* Tab-2 */
    QTreeWidgetItem* headerItems_module = new QTreeWidgetItem;
    headerItems_module->setText( 0 , u8"名称" );
    headerItems_module->setText( 1 , u8"PID" );
    headerItems_module->setText( 2 , u8"CPU" );
    headerItems_module->setText( 3 , u8"内存" );
    headerItems_module->setText( 4 , u8"磁盘" );
    headerItems_module->setText( 5 , u8"网络" );
    ui->treeWidget_Module->setHeaderItem( headerItems_module );

    for (unsigned int i = 0; i < snap_obj.m_Process_vec.size(); i++) {
        wchar_t t_pid[10] = {};
        wchar_t t_cpu[30] = {};

        if (snap_obj.m_Process_vec[i].th32ProcessID) {
//            CpuUsage cu(snap_obj.m_Process_vec[i].th32ProcessID);
            _snwprintf_s(t_cpu, 30, L"%d%%",  0);
        }
        _snwprintf_s(t_pid, 10, L"%d", snap_obj.m_Process_vec[i].th32ProcessID);

        QTreeWidgetItem* item = new QTreeWidgetItem;
        item->setText( 0 , QString::fromWCharArray(snap_obj.m_Process_vec[i].szExeFile));
        item->setText( 1 , QString::fromWCharArray(t_pid));

        if (snap_obj.m_Process_vec[i].th32ProcessID) {
            item->setText( 2 , QString::fromWCharArray(t_cpu));
        }

        ui->treeWidget_Module->addTopLevelItem(item);

        if (snap_obj.ModuleRoundRobin(snap_obj.m_Process_vec[i].th32ProcessID)) {
            //for (unsigned int j = 0; j < snap_obj.m_dwBuffSize / sizeof(HMODULE); ++j) {
            for (unsigned int j = 0; j < snap_obj.m_Module_vec.size(); ++j) {
//                MODULEINFO stcModuleInfo = {};
//                wchar_t szModuleName[MAX_PATH] = {};

//                GetModuleInformation(snap_obj.m_hProcess, snap_obj.m_pModuleHandleArr[j], &stcModuleInfo, sizeof(MODULEINFO));

//                GetModuleFileNameEx(snap_obj.m_hProcess, snap_obj.m_pModuleHandleArr[j], (LPWSTR)&szModuleName, MAX_PATH);

                QTreeWidgetItem* subItem = new QTreeWidgetItem;
                subItem->setText( 0 , QString::fromWCharArray(snap_obj.m_Module_vec[j].szModule));
                subItem->setText( 1 , QString::fromWCharArray(snap_obj.m_Module_vec[j].szExePath));
//                subItem->setText( 0 , QString::fromWCharArray(szModuleName) );
                //subItem->setText( 1 , QString::number(stcModuleInfo.SizeOfImage, 10) );
                item->addChild( subItem );
            }
        }
    }


    /* Tab-3 */
    QTreeWidgetItem* headerItems_desktop = new QTreeWidgetItem;
    headerItems_desktop->setText( 0 , u8"名称" );
    headerItems_desktop->setText( 1 , u8"窗口类" );
    headerItems_desktop->setText( 2 , u8"窗口属性" );
    ui->treeWidget_Window->setHeaderItem( headerItems_desktop );

    EnumWindows((WNDENUMPROC)WndEnumProc, (LPARAM)ui->treeWidget_Window);

    this->setStyleSheet("background-color:rgb(82,106,169)");
}

CMyProcess::~CMyProcess()
{
    delete ui;
}

bool CALLBACK WndEnumProc(HWND hWnd, LPARAM lParam) {
    wchar_t szWindowName[128] = {};
    wchar_t szClassName[128] = {};
    wchar_t szIsvisible[10] = {};
    QTreeWidget* tree_desktop = (QTreeWidget*)lParam;

    GetWindowText(hWnd, szWindowName, MAXBYTE);
    GetClassName(hWnd, szClassName, MAXBYTE);
     _snwprintf_s(szIsvisible, 10, L"%s", IsWindowVisible(hWnd) ? L"可见" : L"隐藏");
//    qDebug() << QString::fromWCharArray(szWindowName);
//    qDebug() << QString::fromWCharArray(szClassName);


     QTreeWidgetItem* item = new QTreeWidgetItem;
     item->setText( 0 , QString::fromWCharArray(szWindowName));
     item->setText( 1 , QString::fromWCharArray(szClassName));
     item->setText( 2 , QString::fromWCharArray(szIsvisible));

     tree_desktop->addTopLevelItem(item);

    return TRUE;
}

BOOL CMyProcess::
TerminateProcessFromID(DWORD dwId) {
    BOOL bRet = FALSE;

    //打开目标程序，取得句柄
    HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwId);
    if(hProcess != NULL) {
        //终止进程
        bRet = ::TerminateProcess(hProcess, 0);
    }

    return bRet;
}

void CMyProcess::
trigerTerminateProcess() {
    if (m_curItem_terminate == NULL) {
        return;
    } else {
       //qDebug() << m_curItem_terminate->text(1).toInt();
        TerminateProcessFromID(m_curItem_terminate->text(1).toInt());
        m_curItem_terminate->setBackgroundColor(0, QColor(255, 0, 0));
    }
}

void CMyProcess::on_treeWidget_Process_customContextMenuRequested(const QPoint &pos) {
    //获取当前被点击的节点
    m_curItem_terminate = ui->treeWidget_Process->itemAt(pos);

    //这种情况是右键的位置不在treeItem的范围内，即在空白位置右击
    if(m_curItem_terminate == NULL)
        return;

    QMenu *popMenu =new QMenu(this);//定义一个右键弹出菜单
    popMenu->addAction(m_act_process);
    popMenu->exec(QCursor::pos());//弹出右键菜单，菜单位置为光标位置
}
