#include "cmytrush.h"
#include "ui_cmytrush.h"
#include <QDebug>
#include <QFileDialog>
#include <Strsafe.h>
#include <Shlwapi.h>
#include <windows.h>
#include <WinInet.h>

CMyTrush::CMyTrush(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CMyTrush),
    IsDocSelected(false)
{
    ui->setupUi(this);
    this->setStyleSheet("background-color:rgb(128,255,128)");

    m_act_BrowserClean = new QAction(u8"清楚浏览器缓存", this);
    m_act_BrowserSearch = new QAction(u8"查找浏览器缓存", this);

    m_menu = new QMenu(u8"操作", this);
    m_menu->addAction(m_act_BrowserClean);
    m_menu->addAction(m_act_BrowserSearch);

    m_menuBar = new QMenuBar(this);
    m_menuBar->addMenu(m_menu);

    connect(m_act_BrowserClean, SIGNAL(triggered()), this, SLOT(trigerBrowserClean()));
    connect(m_act_BrowserSearch, SIGNAL(triggered()), this, SLOT(trigerBrowserSearch()));
}

CMyTrush::~CMyTrush()
{
    delete ui;
}

void CMyTrush::
trigerBrowserClean() {
    LPINTERNET_CACHE_ENTRY_INFO StorageCache;
    DWORD StorageSize = 0;
    FindFirstUrlCacheEntry(NULL, NULL, &StorageSize);

    StorageCache = (LPINTERNET_CACHE_ENTRY_INFO)new char[StorageSize + 1]{};
    HANDLE hFirstCache = FindFirstUrlCacheEntry(NULL, StorageCache, &StorageSize);
    if (!hFirstCache)
        return;

    ui->textEdit->clear();

    do {
        FindNextUrlCacheEntry(hFirstCache, NULL, &StorageSize);
        if (StorageCache)
            delete[] StorageCache;
        StorageCache = (LPINTERNET_CACHE_ENTRY_INFO)new char[StorageSize + 1]{};
        if (!FindNextUrlCacheEntry(hFirstCache, StorageCache, &StorageSize)) {
            if (GetLastError() == ERROR_NO_MORE_ITEMS)
                break;
        }
        ui->textEdit->append(QString::fromWCharArray(StorageCache->lpszSourceUrlName));
        ui->textEdit->append(QString::fromWCharArray(StorageCache->lpszLocalFileName));

        DeleteUrlCacheEntry(StorageCache->lpszSourceUrlName);
    } while (1);
    ui->textEdit->append(u8"清理完成");
}

void CMyTrush::
trigerBrowserSearch() {
    LPINTERNET_CACHE_ENTRY_INFO StorageCache;
    DWORD StorageSize = 0;
    FindFirstUrlCacheEntry(NULL, NULL, &StorageSize);

    StorageCache = (LPINTERNET_CACHE_ENTRY_INFO)new char[StorageSize + 1]{};
    HANDLE hFirstCache = FindFirstUrlCacheEntry(NULL, StorageCache, &StorageSize);
    if (!hFirstCache)
        return;

    ui->textEdit->clear();

    do {
        FindNextUrlCacheEntry(hFirstCache, NULL, &StorageSize);
        if (StorageCache)
            delete[] StorageCache;
        StorageCache = (LPINTERNET_CACHE_ENTRY_INFO)new char[StorageSize + 1]{};
        if (!FindNextUrlCacheEntry(hFirstCache, StorageCache, &StorageSize)) {
            if (GetLastError() == ERROR_NO_MORE_ITEMS)
                break;
        }
        ui->textEdit->append(QString::fromWCharArray(StorageCache->lpszSourceUrlName));
        ui->textEdit->append(QString::fromWCharArray(StorageCache->lpszLocalFileName));
    } while (1);
    ui->textEdit->append(u8"查询完成");
}

void CMyTrush::on_pushButton_2_clicked()
{
//    QString path = QFileDialog::getOpenFileName();
//    qDebug() << path;

    QString file_path = QFileDialog::getExistingDirectory(NULL, tr(u8"选择文件夹"),"D:\\",QFileDialog::ShowDirsOnly);
    if(file_path.isEmpty()) {
        return;
    } else {
        qDebug() << file_path << endl;
        qDebug() << file_path.toStdString().c_str() << endl;
        qDebug() << file_path.toHtmlEscaped();
//        ui->textEdit->clear();
//        ui->textEdit->append(file_path);
        ui->textEdit->setText(file_path);
        IsDocSelected = true;
    }
}

LARGE_INTEGER CMyTrush::EnumFoldAndDeleteFile(WCHAR* lpFoldPath, WCHAR* lpFileType) {
    WCHAR szFoldPath[MAX_PATH] = {};
    LARGE_INTEGER qwFileTotalSize = {};
    WIN32_FIND_DATA w32FindData = {};

    StringCchPrintf(szFoldPath, MAX_PATH, L"%s\\*", lpFoldPath);
    HANDLE hFindFile = FindFirstFile(szFoldPath, &w32FindData);

    ui->textEdit->clear();
    QString DelSuffix = ui->lineEdit->text();
    QStringList DelSuffixBuff = DelSuffix.split(" ");

    do {
        if ((!wcscmp(w32FindData.cFileName, L".")) || (!wcscmp(w32FindData.cFileName, L"..")))
            continue;

        if (w32FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            WCHAR szSubfoldPath[MAX_PATH];
            StringCchPrintf(szSubfoldPath, MAX_PATH, L"%s\\", lpFoldPath);
            StringCchCat(szSubfoldPath, MAX_PATH, w32FindData.cFileName);
            qwFileTotalSize = EnumFoldAndDeleteFile(szSubfoldPath, lpFileType);
        } else {
            WCHAR szFullPath[MAX_PATH];
            StringCchPrintf(szFullPath, MAX_PATH, L"%s\\", lpFoldPath);
            StringCchCat(szFullPath, MAX_PATH, w32FindData.cFileName);
            LPWSTR FileSuffix = PathFindExtension(szFullPath);

            for (int i = 0; i < DelSuffixBuff.size(); i++) {
//                qDebug() << DelSuffixBuff.at(i) << endl;
                wchar_t MyDelSuffix[MAX_PATH] = {};
                DelSuffixBuff.at(i).toWCharArray(MyDelSuffix);

                if (!lstrcmp(FileSuffix, MyDelSuffix)) {
                    ULARGE_INTEGER fileSize;
                    fileSize.HighPart = w32FindData.nFileSizeHigh;
                    fileSize.LowPart = w32FindData.nFileSizeLow;
                    qwFileTotalSize.QuadPart += fileSize.QuadPart;

                    DeleteFile(szFullPath);
                    ui->textEdit->append(QString::fromWCharArray(szFullPath));
                }
            }
        }
    } while (FindNextFile(hFindFile, &w32FindData));

    return qwFileTotalSize;
}

void CMyTrush::on_pushButton_clicked()
{
    if (!IsDocSelected) {
        ui->textEdit->clear();
        return;
    }

    wchar_t DelDoc[MAX_PATH] = {};

    ui->textEdit->toPlainText().toWCharArray(DelDoc);

    qDebug() << ui->textEdit->toPlainText().toStdString().c_str() << endl;

    EnumFoldAndDeleteFile(DelDoc, NULL);

    IsDocSelected = false;
}
