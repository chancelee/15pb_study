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

    m_act_uninstall = new QAction(u8"卸载", this);
    m_act_grub = new QAction(u8"关闭启动项", this);

    m_timer = new QTimer(this);

    connect(m_timer, SIGNAL(timeout()), this, SLOT(FlushGrub()));
    connect(m_act_uninstall, SIGNAL(triggered()), this, SLOT(trigerUninstall()));
    connect(m_act_grub, SIGNAL(triggered()), this, SLOT(trigerGrub()));

    QTreeWidgetItem* headerItems = new QTreeWidgetItem;
    headerItems->setText( 0 , u8"键名" );
    headerItems->setText( 1 , u8"卸载键" );
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
    headerItems_grub->setText( 0 , u8"名称" );
    headerItems_grub->setText( 1 , u8"路径" );
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
    /* 接收将要打开的键的句柄 */
    HKEY hkResult = 0;

    /* 打开一个已经存在的注册表键 */
    LONG lReturn = RegOpenKeyEx(
                RootKey, // 根键句柄
                lpSubKey, // 子键路径
                0, // 保留，必须为0
                KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE, // 指定打开键句柄的权限
                &hkResult); // 子键句柄
    if (lReturn != ERROR_SUCCESS) {
        return false;
    }

    /* 循环遍历Uninstall 目录下的子键 */
    DWORD dwIndex = 0;
    while (1) {
        DWORD dwKeyLen = 255;
        /* 注册表项目名 */
        WCHAR szNewKeyName[MAX_PATH] = {};

        LONG lReturn = RegEnumKeyEx(
                    hkResult, // 子键句柄
                    dwIndex, // 指定被枚举键下子键的索引，从0开始
                    szNewKeyName, // 指向一块内存保存子键的名称
                    &dwKeyLen, // 指向szNewKeyName内存的大小
                    0, // 保留，必须为0
                    NULL, // 用于获取子键值的类型
                    NULL, // 指向一块内存，用于获取子键值的数据
                    NULL); // 上一个参数所指向的内存大小

        if (lReturn == ERROR_NO_MORE_ITEMS)
            break;

        /* 通过得到的子键名称重新组合成新的子键路径 */
        SOFTINFO m_SoftInfo = {};
        WCHAR strMidReg[MAX_PATH] = {};
        swprintf_s(strMidReg, L"%s%s%s", lpSubKey, L"\\", szNewKeyName);
        swprintf_s(m_SoftInfo.strMidReg, L"%s", szNewKeyName);

        /* 打开新的子键，获取句柄 */
        HKEY hkValueKey = 0;
        RegOpenKeyEx(RootKey, strMidReg, 0, KEY_QUERY_VALUE, &hkValueKey);

        /* 获取键值 */
        DWORD dwNameLen = 255; // 指向szBuffer内存的大小
        DWORD dwType = 0;
        /* 获取软件名称 */
        RegQueryValueEx(hkValueKey, L"DisplayName", 0, &dwType, (LPBYTE)m_SoftInfo.szSoftName, &dwNameLen);

        /* 如果没有，需要重新赋值，否则，下次无法获取信息 */
        dwNameLen = 255;
        /* 获取卸载路径 */
        RegQueryValueEx(hkValueKey, L"UninstallString", 0, &dwType, (LPBYTE)m_SoftInfo.strSoftUniPath, &dwNameLen);

        dwNameLen = 255;
        /* 子键索引 */
        dwIndex++;

        m_vectSoftInfo.push_back(m_SoftInfo);
    }

    return true;
}

bool CMyGrub::EnumGrub() {
    HKEY RootKey = HKEY_CURRENT_USER;
    LPCTSTR lpSubKey = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
    /* 接收将要打开的键的句柄 */
    HKEY hkResult = 0;

    /* 打开一个已经存在的注册表键 */
    LONG lReturn = RegOpenKeyEx(
                RootKey, // 根键句柄
                lpSubKey, // 子键路径
                0, // 保留，必须为0
                KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE, // 指定打开键句柄的权限
                &hkResult); // 子键句柄
    if (lReturn != ERROR_SUCCESS) {
        return false;
    }

    m_vectGrubInfo.clear();

    /* 循环遍历Uninstall 目录下的子键 */
    DWORD dwIndex = 0;
    while (1) {
        DWORD dwKeyLen = 255;
        /* 注册表项目名 */
        WCHAR szNewKeyName[MAX_PATH] = {};
        WCHAR szValue[MAX_PATH] = {};
        DWORD dwValueLen = 0;

        LONG lReturn = RegEnumValue(
                    hkResult, // 子键句柄
                    dwIndex, // 指定被枚举键下子键的索引，从0开始
                    szNewKeyName, // 指向一块内存保存子键的名称
                    &dwKeyLen, // 指向szNewKeyName内存的大小
                    0, // 保留，必须为0
                    NULL, // 用于获取子键值的类型
                    (PBYTE)szValue, // 指向一块内存，用于获取子键值的数据
                    &dwValueLen); // 上一个参数所指向的内存大小

        if (lReturn == ERROR_NO_MORE_ITEMS)
            break;

        /* 通过得到的子键名称重新组合成新的子键路径 */
        GRUBINFO m_GrubInfo = {};
        swprintf_s(m_GrubInfo.szSoftName, L"%s", szNewKeyName);

        /* 打开新的子键，获取句柄 */
        HKEY hkValueKey = 0;
        RegOpenKeyEx(RootKey, lpSubKey, 0, KEY_QUERY_VALUE, &hkValueKey);

        /* 获取键值 */
        DWORD dwNameLen = 255; // 指向szBuffer内存的大小
        DWORD dwType = 0;
        /* 获取软件名称 */
        RegQueryValueEx(hkValueKey, szNewKeyName, 0, &dwType, (LPBYTE)m_GrubInfo.strSoftPath, &dwNameLen);
        /* 子键索引 */
        dwIndex++;

        m_vectGrubInfo.push_back(m_GrubInfo);
    }

    return true;
}

void CMyGrub::on_treeWidget_SoftUninstall_customContextMenuRequested(const QPoint &pos) {
    //获取当前被点击的节点
    m_curItem_uninstall = ui->treeWidget_SoftUninstall->itemAt(pos);

    //这种情况是右键的位置不在treeItem的范围内，即在空白位置右击
    if(m_curItem_uninstall == NULL)
        return;

    QMenu *popMenu =new QMenu(this);//定义一个右键弹出菜单
    popMenu->addAction(m_act_uninstall);
    popMenu->exec(QCursor::pos());//弹出右键菜单，菜单位置为光标位置
}

void CMyGrub::on_treeWidget_Boot_customContextMenuRequested(const QPoint &pos) {
    //获取当前被点击的节点
    m_curItem_grub = ui->treeWidget_Boot->itemAt(pos);

    //这种情况是右键的位置不在treeItem的范围内，即在空白位置右击
    if(m_curItem_grub == NULL)
        return;

    QMenu *popMenu =new QMenu(this);//定义一个右键弹出菜单
    popMenu->addAction(m_act_grub);
    popMenu->exec(QCursor::pos());//弹出右键菜单，菜单位置为光标位置
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
