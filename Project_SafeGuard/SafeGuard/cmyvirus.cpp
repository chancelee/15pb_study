#include "cmyvirus.h"
#include "ui_cmyvirus.h"
#include "md5.h"
#include <QDebug>
#include <QFileDialog>
#include <Strsafe.h>
#include <Shlwapi.h>
#include <QMenuBar>

void WCHARTOCHAR(wchar_t* szWchar, unsigned int uSize, char* szChar);
void CHARTOWCHAR(char* szChar, unsigned int uSize, wchar_t* szWchar);

void WCHARTOCHAR(wchar_t* szWchar, unsigned int uSize, char* szChar) {
    WideCharToMultiByte(CP_ACP, NULL, szWchar, -1, szChar, uSize, NULL, FALSE);
}

void CHARTOWCHAR(char* szChar, unsigned int uSize, wchar_t* szWchar) {
    MultiByteToWideChar(CP_ACP, NULL, szChar, -1, szWchar, uSize);
}

CMyVirus::CMyVirus(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CMyVirus)
{
    ui->setupUi(this);

//    qDebug() << md5FileValue((char*)"E:\\software\\Qt\\workspace\\SafeGuard\\md5_virus\\00.txt") << endl;
//    qDebug() << md5FileValue((char*)"E:\\software\\Qt\\workspace\\SafeGuard\\md5_virus\\01.txt") << endl;
//    qDebug() << md5FileValue((char*)"E:\\software\\Qt\\workspace\\SafeGuard\\md5_virus\\02.txt") << endl;

    ui->pushButton->setIcon(QIcon(":/Resources/ico/md5.ico"));
    ui->pushButton->setIconSize(QSize(100, 100));
    ui->pushButton->setStyleSheet("border:none;");

    ui->pushButton_2->setIcon(QIcon(":/Resources/ico/doc.ico"));
    ui->pushButton_2->setIconSize(QSize(100, 100));
    ui->pushButton_2->setStyleSheet("border:none;");

    ui->pushButton_3->setIcon(QIcon(":/Resources/ico/cleaner.ico"));
    ui->pushButton_3->setIconSize(QSize(100, 100));
    ui->pushButton_3->setStyleSheet("border:none;");

    m_act_snapshot = new QAction(u8"生成快照", this);
    m_act_lightning = new QAction(u8"查杀", this);

    m_menu = new QMenu("WhiteList", this);
    m_menu->addAction(m_act_snapshot);
    m_menu->addAction(m_act_lightning);

    m_menuBar = new QMenuBar(this);
    m_menuBar->addMenu(m_menu);

    connect(m_act_snapshot, SIGNAL(triggered()), this, SLOT(trigerSnapShot()));
    connect(m_act_lightning, SIGNAL(triggered()), this, SLOT(trigerLightning()));

    this->setStyleSheet("background-color:rgb(207,190,175)");

    StorageMd5();
}

CMyVirus::~CMyVirus() {
    delete ui;
}

void CMyVirus::
trigerSnapShot() {
    m_snap_obj.ProcessRoundRobin();
    ui->textEdit->setText(u8"生成快照成功");
}

void CMyVirus::
trigerLightning() {
    bool InWhiteList = false;

    CSnapShot snap_twice_obj;
    snap_twice_obj.ProcessRoundRobin();

    ui->textEdit->setText(u8"开始白名单查杀");

    for (unsigned int i = 0; i < snap_twice_obj.m_Process_vec.size(); i++) {
        for (unsigned int j = 0; j < m_snap_obj.m_Process_vec.size(); j++) {
            if (snap_twice_obj.m_Process_vec[i].th32ProcessID == m_snap_obj.m_Process_vec[j].th32ProcessID) {
                InWhiteList = true;
                break;
            }
        }

        if (InWhiteList) {
            InWhiteList = false;
            continue;
        } else {
            wchar_t t_pid[128] = {};
            _snwprintf_s(t_pid, 128, L"%d", snap_twice_obj.m_Process_vec[i].th32ProcessID);
            TerminateProcessFromID(snap_twice_obj.m_Process_vec[i].th32ProcessID);
            ui->textEdit->append(QString::fromWCharArray(t_pid));
        }
    }

    ui->textEdit->append(u8"查杀完成");

}

BOOL CMyVirus::
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

void CMyVirus::StorageMd5() {
    HANDLE hFile = CreateFile(
                L"E:\\software\\Qt\\workspace\\SafeGuard\\virus\\md5",
                GENERIC_READ,
                0,
                NULL,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                NULL);

    char ch[128] = {};
    DWORD dwReads;

    ReadFile(hFile, ch, 128, &dwReads, NULL);

    m_md5 = QString(QLatin1String(ch));

//    qDebug() << m_md5 << endl;

    CloseHandle(hFile);
}

LARGE_INTEGER CMyVirus::EnumFoldAndDeleteVirus(WCHAR* lpFoldPath, WCHAR* lpFileType) {
    WCHAR szFoldPath[MAX_PATH] = {};
    LARGE_INTEGER qwFileTotalSize = {};
    WIN32_FIND_DATA w32FindData = {};

    StringCchPrintf(szFoldPath, MAX_PATH, L"%s\\*", lpFoldPath);
    HANDLE hFindFile = FindFirstFile(szFoldPath, &w32FindData);

    ui->textEdit->clear();

    do {
        if ((!wcscmp(w32FindData.cFileName, L".")) || (!wcscmp(w32FindData.cFileName, L"..")))
            continue;

        if (w32FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            WCHAR szSubfoldPath[MAX_PATH];
            StringCchPrintf(szSubfoldPath, MAX_PATH, L"%s\\", lpFoldPath);
            StringCchCat(szSubfoldPath, MAX_PATH, w32FindData.cFileName);
            qwFileTotalSize = EnumFoldAndDeleteVirus(szSubfoldPath, lpFileType);
        } else {
            WCHAR szFullPath[MAX_PATH];
            char szFullPath_char[MAX_PATH];
            bool md5_checked = false;
            StringCchPrintf(szFullPath, MAX_PATH, L"%s\\", lpFoldPath);
            StringCchCat(szFullPath, MAX_PATH, w32FindData.cFileName);
            WCHARTOCHAR(szFullPath, MAX_PATH, szFullPath_char);

            QStringList m_list = m_md5.split("\r\n");
            for (int i = 0; i < m_list.size(); i++) {
                if (QString(QLatin1String(md5FileValue(szFullPath_char))) == m_list[i]) {
                    ULARGE_INTEGER fileSize;
                    fileSize.HighPart = w32FindData.nFileSizeHigh;
                    fileSize.LowPart = w32FindData.nFileSizeLow;
                    qwFileTotalSize.QuadPart += fileSize.QuadPart;
                    md5_checked = true;
                    //ui->textEdit->append(QString::fromWCharArray(szFullPath)+ ":" + m_list[i]);
                    break;
                }
            }

            if (!md5_checked) {
                DeleteFile(szFullPath);
                ui->textEdit->append(QString::fromWCharArray(szFullPath));
            }
        }
    } while (FindNextFile(hFindFile, &w32FindData));

    return qwFileTotalSize;
}

/* Md5 Button */
void CMyVirus::on_pushButton_clicked() {

    QString path = QFileDialog::getOpenFileName();
    qDebug() << path;
    ui->textEdit->clear();

    if (!path.size())
        return;

    QStringList m_list = m_md5.split("\r\n");
    for (int i = 0; i < m_list.size(); i++) {
        QString file_md5_str = QString(QLatin1String(md5FileValue((char*)path.toStdString().c_str())));
        if (file_md5_str == m_list[i]) {
            ui->textEdit->setText(path + ":" + m_list[i]);
            break;
        } else {
            ui->textEdit->setText("Can't Find " + path + " MD5, May Be a Virus");
            ui->textEdit->append(u8"属性信息：\nMD5: ");
            ui->textEdit->append(file_md5_str);
            WIN32_FIND_DATA w32FindData = {};
            SYSTEMTIME sys_create_time;
            SYSTEMTIME sys_LastWrite_time;
            wchar_t w_path[256] = {};
            wchar_t w_create_time[60] = {};
            wchar_t w_LastWrite_time[60] = {};
            wchar_t w_FileSize[128] = {};

            CHARTOWCHAR((char*)path.toStdString().c_str(), path.size(), w_path);
            HANDLE hFindFile = FindFirstFile(w_path, &w32FindData);
            if (hFindFile == INVALID_HANDLE_VALUE) {
                return;
            }

            FileTimeToSystemTime(&w32FindData.ftCreationTime, &sys_create_time);
            FileTimeToSystemTime(&w32FindData.ftLastWriteTime, &sys_LastWrite_time);

            _snwprintf_s(w_create_time, 60, L"创建时间 : %d-%d-%d %d:%d:%d",
                         sys_create_time.wYear, sys_create_time.wMonth, sys_create_time.wDay,
                         sys_create_time.wHour, sys_create_time.wMinute, sys_create_time.wSecond);

            _snwprintf_s(w_LastWrite_time, 60, L"修改时间 : %d-%d-%d %d:%d:%d",
                         sys_LastWrite_time.wYear, sys_LastWrite_time.wMonth, sys_LastWrite_time.wDay,
                         sys_LastWrite_time.wHour, sys_LastWrite_time.wMinute, sys_LastWrite_time.wSecond);

            _snwprintf_s(w_FileSize, 60, L"文件大小 : %d MB",
                         (w32FindData.nFileSizeHigh * (MAXDWORD + 1) + w32FindData.nFileSizeLow) / 1024 / 1024);

            ui->textEdit->append(QString::fromWCharArray(w_create_time));
            ui->textEdit->append(QString::fromWCharArray(w_LastWrite_time));
            ui->textEdit->append(QString::fromWCharArray(w_FileSize));
//            ui->textEdit->append(w32FindData.ftLastWriteTime.dwLowDateTime);
        }
    }
}

/* Doc Button */
void CMyVirus::on_pushButton_2_clicked() {
    QString file_path = QFileDialog::getExistingDirectory(NULL, tr(u8"选择文件夹"),"E:\\",QFileDialog::ShowDirsOnly);
    if(file_path.isEmpty()) {
        return;
    } else {
//        qDebug() << file_path << endl;
//        qDebug() << file_path.toStdString().c_str() << endl;
//        qDebug() << file_path.toHtmlEscaped();
        //        ui->textEdit->clear();
        //        ui->textEdit->append(file_path);

        wchar_t DelDoc[MAX_PATH] = {};
        file_path.toWCharArray(DelDoc);

        EnumFoldAndDeleteVirus(DelDoc, NULL);
    }
}

/* Process/Disk Button */
void CMyVirus::on_pushButton_3_clicked() {
    ui->textEdit->setText(u8"清空回收站");
    SHQUERYRBINFO RecycleBinInfo = {sizeof(RecycleBinInfo)};

    SHQueryRecycleBin(NULL, &RecycleBinInfo);

    SHEmptyRecycleBin(NULL, NULL, SHERB_NOCONFIRMATION | SHERB_NOPROGRESSUI | SHERB_NOSOUND);
    ui->textEdit->append(u8"已清空");
}
