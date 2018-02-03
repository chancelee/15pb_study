#ifndef CMYPE_H
#define CMYPE_H

#include <QWidget>
#include <QCloseEvent>
#include <Windows.h>

namespace Ui {
class CMyPE;
}

class CMyPE : public QWidget
{
    Q_OBJECT

public:
    explicit CMyPE(QWidget *parent = 0);
    ~CMyPE();
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    bool IsPeFile(LPBYTE buf);
    void ShowNtHeader(PIMAGE_NT_HEADERS pNt);
    void ShowSections(PIMAGE_NT_HEADERS pNt);
    void ShowDataDir(PIMAGE_NT_HEADERS pNt);
    DWORD VirtualAddr2FileAddr(PIMAGE_NT_HEADERS pNt, DWORD rva);
    void ShowExport(PBYTE buf);
    void ShowImport(PBYTE buf);
    void ShowRelocInfo(PBYTE buf);
    void ShowResource(PBYTE buf);
    void ShowTls(PBYTE buf);
    void ShowDelayDll(PBYTE buf);

private:
    Ui::CMyPE *ui;
};

#endif // CMYPE_H
