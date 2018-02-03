#include "cmygrub.h"
#include "ui_cmygrub.h"
#include <QTreeWidget>
#include <QDebug>
#include <QMenu>
#include <Strsafe.h>

CMyGrub::CMyGrub(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CMyGrub),
    m_curItem_uninstall(nullptr),
    m_curItem_grub(nullptr)
{
    ui->setupUi(this);

    m_act_uninstall = new QAction(u8"ж��", this);
    m_act_grub = new QAction(u8"�ر�������", this);

    m_timer = new QTimer(this);

    connect(m_timer, SIGNAL(timeout()), this, SLOT(FlushGrub()));
    connect(m_act_uninstall, SIGNAL(triggered()), this, SLOT(trigerUninstall()));
    connect(m_act_grub, SIGNAL(triggered()), this, SLOT(trigerGrub()));

    QTreeWidgetItem* headerItems = new QTreeWidgetItem;
    headerItems->setText( 0 , u8"����" );
    headerItems->setText( 1 , u8"ж�ؼ�" );
    ui->treeWidget_SoftUninstall->setHeaderItem( headerItems );

    if (EnumReg()) {
        for (unsigned int i = 0; i < m_vectSoftInfo.size(); i++) {
            QTreeWidgetItem* item = new QTreeWidgetItem;
            item->setText( 0 , QString::fromWCharArray(m_vectSoftInfo[i].strMidReg));
            item->setText( 1 , QString::fromWCharArray(m_vectSoftInfo[i].strSoftUniPath));
            ui->treeWidget_SoftUninstall->addTopLevelItem(item);
        }
    }

    QTreeWidgetItem* headerItems_grub = new QTreeWidgetItem;
    headerItems_grub->setText( 0 , u8"����" );
    headerItems_grub->setText( 1 , u8"·��" );
    ui->treeWidget_Boot->setHeaderItem( headerItems_grub );

    if (EnumGrub()) {
        for (unsigned int i = 0; i < m_vectGrubInfo.size(); i++) {
            QTreeWidgetItem* item = new QTreeWidgetItem;
            item->setText( 0 , QString::fromWCharArray(m_vectGrubInfo[i].szSoftName));
            item->setText( 1 , QString::fromWCharArray(m_vectGrubInfo[i].strSoftPath));
            ui->treeWidget_Boot->addTopLevelItem(item);
        }
    }

    this->setStyleSheet("background-color:rgb(182,182,109)");
}

CMyGrub::~CMyGrub()
{
    delete ui;
}

bool CMyGrub::EnumReg() {
    HKEY RootKey = HKEY_LOCAL_MACHINE;
    LPCTSTR lpSubKey = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall";
    /* ���ս�Ҫ�򿪵ļ��ľ�� */
    HKEY hkResult = 0;

    /* ��һ���Ѿ����ڵ�ע���� */
    LONG lReturn = RegOpenKeyEx(
                RootKey, // �������
                lpSubKey, // �Ӽ�·��
                0, // ����������Ϊ0
                KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE, // ָ���򿪼������Ȩ��
                &hkResult); // �Ӽ����
    if (lReturn != ERROR_SUCCESS) {
        return false;
    }

    /* ѭ������Uninstall Ŀ¼�µ��Ӽ� */
    DWORD dwIndex = 0;
    while (1) {
        DWORD dwKeyLen = 255;
        /* ע�����Ŀ�� */
        WCHAR szNewKeyName[MAX_PATH] = {};

        LONG lReturn = RegEnumKeyEx(
                    hkResult, // �Ӽ����
                    dwIndex, // ָ����ö�ټ����Ӽ�����������0��ʼ
                    szNewKeyName, // ָ��һ���ڴ汣���Ӽ�������
                    &dwKeyLen, // ָ��szNewKeyName�ڴ�Ĵ�С
                    0, // ����������Ϊ0
                    NULL, // ���ڻ�ȡ�Ӽ�ֵ������
                    NULL, // ָ��һ���ڴ棬���ڻ�ȡ�Ӽ�ֵ������
                    NULL); // ��һ��������ָ����ڴ��С

        if (lReturn == ERROR_NO_MORE_ITEMS)
            break;

        /* ͨ���õ����Ӽ�����������ϳ��µ��Ӽ�·�� */
        SOFTINFO m_SoftInfo = {};
        WCHAR strMidReg[MAX_PATH] = {};
        swprintf_s(strMidReg, L"%s%s%s", lpSubKey, L"\\", szNewKeyName);
        swprintf_s(m_SoftInfo.strMidReg, L"%s", szNewKeyName);

        /* ���µ��Ӽ�����ȡ��� */
        HKEY hkValueKey = 0;
        RegOpenKeyEx(RootKey, strMidReg, 0, KEY_QUERY_VALUE, &hkValueKey);

        /* ��ȡ��ֵ */
        DWORD dwNameLen = 255; // ָ��szBuffer�ڴ�Ĵ�С
        DWORD dwType = 0;
        /* ��ȡ������� */
        RegQueryValueEx(hkValueKey, L"DisplayName", 0, &dwType, (LPBYTE)m_SoftInfo.szSoftName, &dwNameLen);

        /* ���û�У���Ҫ���¸�ֵ�������´��޷���ȡ��Ϣ */
        dwNameLen = 255;
        /* ��ȡж��·�� */
        RegQueryValueEx(hkValueKey, L"UninstallString", 0, &dwType, (LPBYTE)m_SoftInfo.strSoftUniPath, &dwNameLen);

        dwNameLen = 255;
        /* �Ӽ����� */
        dwIndex++;

        m_vectSoftInfo.push_back(m_SoftInfo);
    }

    return true;
}

bool CMyGrub::EnumGrub() {
    HKEY RootKey = HKEY_CURRENT_USER;
    LPCTSTR lpSubKey = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
    /* ���ս�Ҫ�򿪵ļ��ľ�� */
    HKEY hkResult = 0;

    /* ��һ���Ѿ����ڵ�ע���� */
    LONG lReturn = RegOpenKeyEx(
                RootKey, // �������
                lpSubKey, // �Ӽ�·��
                0, // ����������Ϊ0
                KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE, // ָ���򿪼������Ȩ��
                &hkResult); // �Ӽ����
    if (lReturn != ERROR_SUCCESS) {
        return false;
    }

    m_vectGrubInfo.clear();

    /* ѭ������Uninstall Ŀ¼�µ��Ӽ� */
    DWORD dwIndex = 0;
    while (1) {
        DWORD dwKeyLen = 255;
        /* ע�����Ŀ�� */
        WCHAR szNewKeyName[MAX_PATH] = {};
        WCHAR szValue[MAX_PATH] = {};
        DWORD dwValueLen = 0;

        LONG lReturn = RegEnumValue(
                    hkResult, // �Ӽ����
                    dwIndex, // ָ����ö�ټ����Ӽ�����������0��ʼ
                    szNewKeyName, // ָ��һ���ڴ汣���Ӽ�������
                    &dwKeyLen, // ָ��szNewKeyName�ڴ�Ĵ�С
                    0, // ����������Ϊ0
                    NULL, // ���ڻ�ȡ�Ӽ�ֵ������
                    (PBYTE)szValue, // ָ��һ���ڴ棬���ڻ�ȡ�Ӽ�ֵ������
                    &dwValueLen); // ��һ��������ָ����ڴ��С

        if (lReturn == ERROR_NO_MORE_ITEMS)
            break;

        /* ͨ���õ����Ӽ�����������ϳ��µ��Ӽ�·�� */
        GRUBINFO m_GrubInfo = {};
        swprintf_s(m_GrubInfo.szSoftName, L"%s", szNewKeyName);

        /* ���µ��Ӽ�����ȡ��� */
        HKEY hkValueKey = 0;
        RegOpenKeyEx(RootKey, lpSubKey, 0, KEY_QUERY_VALUE, &hkValueKey);

        /* ��ȡ��ֵ */
        DWORD dwNameLen = 255; // ָ��szBuffer�ڴ�Ĵ�С
        DWORD dwType = 0;
        /* ��ȡ������� */
        RegQueryValueEx(hkValueKey, szNewKeyName, 0, &dwType, (LPBYTE)m_GrubInfo.strSoftPath, &dwNameLen);
        /* �Ӽ����� */
        dwIndex++;

        m_vectGrubInfo.push_back(m_GrubInfo);
    }

    return true;
}

void CMyGrub::on_treeWidget_SoftUninstall_customContextMenuRequested(const QPoint &pos) {
    //��ȡ��ǰ������Ľڵ�
    m_curItem_uninstall = ui->treeWidget_SoftUninstall->itemAt(pos);

    //����������Ҽ���λ�ò���treeItem�ķ�Χ�ڣ����ڿհ�λ���һ�
    if(m_curItem_uninstall == NULL)
        return;

    QMenu *popMenu =new QMenu(this);//����һ���Ҽ������˵�
    popMenu->addAction(m_act_uninstall);
    popMenu->exec(QCursor::pos());//�����Ҽ��˵����˵�λ��Ϊ���λ��
}

void CMyGrub::on_treeWidget_Boot_customContextMenuRequested(const QPoint &pos) {
    //��ȡ��ǰ������Ľڵ�
    m_curItem_grub = ui->treeWidget_Boot->itemAt(pos);

    //����������Ҽ���λ�ò���treeItem�ķ�Χ�ڣ����ڿհ�λ���һ�
    if(m_curItem_grub == NULL)
        return;

    QMenu *popMenu =new QMenu(this);//����һ���Ҽ������˵�
    popMenu->addAction(m_act_grub);
    popMenu->exec(QCursor::pos());//�����Ҽ��˵����˵�λ��Ϊ���λ��
}

void CMyGrub::
trigerUninstall() {
    if (m_curItem_uninstall == NULL) {
        return;
    } else {
        SHELLEXECUTEINFO sei = { sizeof(SHELLEXECUTEINFO) };
        wchar_t uninstall_path[MAX_PATH] = {};
        m_curItem_uninstall->text(1).toWCharArray(uninstall_path);
        sei.lpFile = uninstall_path;
        sei.lpParameters = NULL;
        sei.nShow = SW_SHOW;

        ShellExecuteEx(&sei);
    }
}

void CMyGrub::
trigerGrub() {
    if (m_curItem_grub == NULL) {
        return;
    } else {
        wchar_t soft_name[MAX_PATH] = {};
        HKEY hSubKey = 0;
        HKEY RootKey = HKEY_CURRENT_USER;
        LPCTSTR lpSubKey = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
        m_curItem_grub->text(0).toWCharArray(soft_name);

        RegOpenKeyEx(RootKey, lpSubKey, 0, KEY_SET_VALUE, &hSubKey);

        RegDeleteValue(hSubKey, soft_name);

        m_timer->start(500);
    }
}

void CMyGrub::
FlushGrub() {
    m_timer->stop();
    if (EnumGrub()) {
        ui->treeWidget_Boot->clear();
        for (unsigned int i = 0; i < m_vectGrubInfo.size(); i++) {
            QTreeWidgetItem* item = new QTreeWidgetItem;
            item->setText( 0 , QString::fromWCharArray(m_vectGrubInfo[i].szSoftName));
            item->setText( 1 , QString::fromWCharArray(m_vectGrubInfo[i].strSoftPath));
            ui->treeWidget_Boot->addTopLevelItem(item);
        }
    }
}
