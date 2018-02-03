#include "cmyservice.h"
#include "csnapshot.h"
#include "ui_cmyservice.h"
#include <windows.h>
#include <QTreeWidget>
#include <QDebug>
#include <QMenu>

CMyService::CMyService(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CMyService),
    m_curItem_terminate(nullptr)
{
    ui->setupUi(this);

    m_act_service_terminate = new QAction(u8"�رշ���", this);
    connect(m_act_service_terminate, SIGNAL(triggered()), this, SLOT(trigerTerminateService()));

    m_act_service_start = new QAction(u8"��������", this);
    connect(m_act_service_start, SIGNAL(triggered()), this, SLOT(trigerStartService()));

    QTreeWidgetItem* headerItems = new QTreeWidgetItem;
    headerItems->setText( 0 , u8"����" );
    headerItems->setText( 1 , u8"PID" );
    headerItems->setText( 2 , u8"����" );
    headerItems->setText( 3 , u8"״̬" );
    headerItems->setText( 4 , u8"��" );
    ui->treeWidget->setHeaderItem( headerItems );

    CSnapShot snap_obj;

    WCHAR szApplication[MAX_PATH] = { 0 };
    DWORD cchLength = _countof(szApplication);
    QueryFullProcessImageName(GetCurrentProcess(), 0,
        szApplication, &cchLength);

    SHELLEXECUTEINFO sei = { sizeof(SHELLEXECUTEINFO) };
    sei.lpVerb = L"runas"; // ����Ȩ����Ҫ�õ����runas
    sei.lpFile = szApplication;
    sei.lpParameters = NULL;
    sei.nShow = SW_SHOWNORMAL;

    // ����ʹ��ShellExecuteEx
    if (!snap_obj.IsAdmin()) {
        if (ShellExecuteEx(&sei)) {
            ExitProcess(0);
        }
    }

    ServiceRoundRobin();

    this->setStyleSheet("background-color:rgb(166,166,210)");
}

CMyService::~CMyService()
{
    delete ui;
}

void CMyService::ServiceRoundRobin() {
    SC_HANDLE hSCM = OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    DWORD dwServiceNum = 0;
    DWORD dwSize = 0;
    const wchar_t* current_state[] = {L"", L"��ֹͣ", L"��������", L"����ֹͣ",
                                      L"��������", L"�ָ�����", L"������ͣ", L"����ͣ"};

    EnumServicesStatusEx(hSCM,
                         SC_ENUM_PROCESS_INFO,
                         SERVICE_WIN32,
                         SERVICE_STATE_ALL,
                         NULL,
                         0,
                         &dwSize,
                         &dwServiceNum,
                         NULL,
                         NULL);

    LPENUM_SERVICE_STATUS_PROCESS pEnumService = (LPENUM_SERVICE_STATUS_PROCESS)new char[dwSize];
    bool bStatus = FALSE;

    bStatus = EnumServicesStatusEx(hSCM,
                                   SC_ENUM_PROCESS_INFO,
                                   SERVICE_WIN32,
                                   SERVICE_STATE_ALL,
                                   (PBYTE)pEnumService,
                                   dwSize,
                                   &dwSize,
                                   &dwServiceNum,
                                   NULL,
                                   NULL);

    ui->treeWidget->clear();

    for (DWORD i = 0; i < dwServiceNum; i++) {
        pEnumService[i].ServiceStatusProcess.dwCurrentState;
        pEnumService[i].ServiceStatusProcess.dwServiceType;

        SC_HANDLE hService = OpenService(hSCM,
                                         pEnumService[i].lpServiceName,
                                         SERVICE_QUERY_CONFIG);

        QueryServiceConfig(hService, NULL, 0, &dwSize);
        LPQUERY_SERVICE_CONFIG pServiceConfig = \
                (LPQUERY_SERVICE_CONFIG)new char[dwSize];
        QueryServiceConfig(hService, pServiceConfig, dwSize, &dwSize);

        wchar_t t_pid[10] = {};
        QTreeWidgetItem* item = new QTreeWidgetItem;

        _snwprintf_s(t_pid, 10, L"%d",  pEnumService[i].ServiceStatusProcess.dwProcessId);
        item->setText( 0 , QString::fromWCharArray(pEnumService[i].lpServiceName));
        item->setText( 1 , QString::fromWCharArray(t_pid));
        item->setText( 2 , QString::fromWCharArray(pServiceConfig->lpBinaryPathName));
        item->setText( 3 , QString::fromWCharArray(current_state[pEnumService[i].ServiceStatusProcess.dwCurrentState]));

        ui->treeWidget->addTopLevelItem(item);
    }
}

void CMyService::
trigerTerminateService() {
    if (m_curItem_terminate == NULL) {
        return;
    } else {
       //qDebug() << m_curItem_terminate->text(1).toInt();
        wchar_t service_name[MAX_PATH] = {};
        m_curItem_terminate->text(0).toWCharArray(service_name);

        SC_HANDLE hSCM = OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS);
        SC_HANDLE hService = OpenService(hSCM,
                                         service_name,
                                         SERVICE_QUERY_CONFIG  | SERVICE_STOP);

        DWORD dwControl = SERVICE_CONTROL_STOP;
        SERVICE_STATUS serviceStatus = { 0 };
        //�رշ���
        ControlService(hService, dwControl, &serviceStatus);

        CloseServiceHandle(hService);
        CloseServiceHandle(hSCM);

        ServiceRoundRobin();
    }
}

/* https://msdn.microsoft.com/en-us/library/windows/desktop/ms686315(v=vs.85).aspx */
void CMyService::
trigerStartService() {
    if (m_curItem_terminate == NULL) {
        return;
    } else {
       //qDebug() << m_curItem_terminate->text(1).toInt();
        wchar_t service_name[MAX_PATH] = {};
        m_curItem_terminate->text(0).toWCharArray(service_name);

        SC_HANDLE hSCM = OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS);
        SC_HANDLE hService = OpenService(hSCM,
                                         service_name,
                                         SERVICE_ALL_ACCESS);

//        DWORD dwControl = SERVICE_CONTROL_CONTINUE;
//        SERVICE_STATUS serviceStatus = { 0 };
        //��������
        StartService(hService, 0, NULL);

        CloseServiceHandle(hService);
        CloseServiceHandle(hSCM);

        ServiceRoundRobin();
    }
}

void CMyService::on_treeWidget_customContextMenuRequested(const QPoint &pos) {
    //��ȡ��ǰ������Ľڵ�
    m_curItem_terminate = ui->treeWidget->itemAt(pos);

    //����������Ҽ���λ�ò���treeItem�ķ�Χ�ڣ����ڿհ�λ���һ�
    if(m_curItem_terminate == NULL)
        return;

    QMenu *popMenu =new QMenu(this);//����һ���Ҽ������˵�
    popMenu->addAction(m_act_service_terminate);
    popMenu->addAction(m_act_service_start);
    popMenu->exec(QCursor::pos());//�����Ҽ��˵����˵�λ��Ϊ���λ��
}
