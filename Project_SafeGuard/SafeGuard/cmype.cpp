#include "cmype.h"
#include "ui_cmype.h"
#include <QDebug>
#include <QMimeData>
#include <QMessageBox>
#include <time.h>
#include <QTreeWidget>

CMyPE::CMyPE(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CMyPE)
{
    ui->setupUi(this);

    ui->FileAllign->setText("0x200");
    ui->BlockAllign->setText("0x1000");

    ui->tableWidget->setColumnCount(6);
    ui->tableWidget->setRowCount(10);
    QStringList header;
    header << u8"名称"
           << "VOffset" << "VSize"
           << "ROffset" << "RSize"
           << u8"标志";
    ui->tableWidget->setHorizontalHeaderLabels(header);

    ui->tab_DataDir->setColumnCount(3);
    ui->tab_DataDir->setRowCount(16);
    QStringList header_dataDir;
    header_dataDir << u8"表项" << "RVA" << u8"大小";
    ui->tab_DataDir->setHorizontalHeaderLabels(header_dataDir);

    ui->tab_export->setColumnCount(3);
    QStringList header_export;
    header_export << u8"序号" << u8"函数地址[RVA]" << u8"函数名";
    ui->tab_export->setHorizontalHeaderLabels(header_export);

    QTreeWidgetItem* headerItems = new QTreeWidgetItem;
    headerItems->setText( 0 , u8"DLL[名称]" );
    headerItems->setText( 1 , u8"导入方式" );
    headerItems->setText( 2 , "Hint" );
    headerItems->setText( 3 , u8"函数名" );
    ui->Import_Table->setHeaderItem( headerItems );

    QTreeWidgetItem* headerItems_Relocation = new QTreeWidgetItem;
    headerItems_Relocation->setText( 0 , u8"区域起始RVA" );
    headerItems_Relocation->setText( 1 , u8"区域大小" );
    headerItems_Relocation->setText( 2 , u8"需要进行重定位的RVA" );
    headerItems_Relocation->setText( 3 , u8"进行重定位的RVA在文件的偏移量" );
    headerItems_Relocation->setText( 4 , u8"类型[一般为3]" );
    headerItems_Relocation->setText( 5 , u8"File Offset位置的数据VA" );
    headerItems_Relocation->setText( 6 , u8"重定位数据[VA-ImgBase再转为文件偏移]" );
    ui->treeWidget_Relocation->setHeaderItem( headerItems_Relocation );


    QTreeWidgetItem* headerItems_Resource = new QTreeWidgetItem;
    headerItems_Resource->setText( 0 , u8"资源种类" );
    headerItems_Resource->setText( 1 , u8"资源名称" );
    headerItems_Resource->setText( 2 , u8"代码页" );
    headerItems_Resource->setText( 3 , u8"资源信息偏移" );
    headerItems_Resource->setText( 4 , u8"资源RVA" );
    headerItems_Resource->setText( 5 , u8"资源Size" );
    ui->treeWidget_Resource->setHeaderItem( headerItems_Resource );

    this->setStyleSheet("background-color:rgb(164,214,215)");
}

CMyPE::~CMyPE()
{
    delete ui;
}

//当用户拖动文件到窗口部件上时候，就会触发dragEnterEvent事件
void CMyPE::
dragEnterEvent(QDragEnterEvent *event) {
    //如果为文件，则支持拖放
    if (event->mimeData()->hasFormat("text/uri-list"))
        event->acceptProposedAction();
}

//当用户放下这个文件后，就会触发dropEvent事件
void CMyPE::
dropEvent(QDropEvent *event) {
    //注意：这里如果有多文件存在，意思是用户一下子拖动了多个文件，而不是拖动一个目录
    //如果想读取整个目录，则在不同的操作平台下，自己编写函数实现读取整个目录文件名
    QList<QUrl> urls = event->mimeData()->urls();
    if(urls.isEmpty())
        return;

    //往文本框中追加文件名
    foreach(QUrl url, urls) {
        QString file_name = url.toLocalFile();
        qDebug() << file_name;

        HANDLE hFile = CreateFile(file_name.toStdWString().c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        DWORD dwSize = GetFileSize(hFile, NULL);
        // 分配内存
        LPBYTE p = new BYTE[dwSize]{};
        // 把文件内容读到内存中
        ReadFile(hFile, p, dwSize, &dwSize, NULL);
        if (IsPeFile(p)) {
            IMAGE_DOS_HEADER *pDos = (PIMAGE_DOS_HEADER)p;
            IMAGE_NT_HEADERS *pNt = (PIMAGE_NT_HEADERS)(p + pDos->e_lfanew);
            ShowNtHeader(pNt);
            ShowSections(pNt);
            ShowDataDir(pNt);
            ShowExport(p);
            ShowImport(p);
            ShowRelocInfo(p);
            ShowResource(p);
            ShowTls(p);
            ShowDelayDll(p);
        } else {
            QMessageBox::critical(this,"Critical",tr("Are you sure PE ???"));
        }
    }
}

bool CMyPE::
IsPeFile(LPBYTE buf) {
    // 1.MZ
    IMAGE_DOS_HEADER *pDos = (PIMAGE_DOS_HEADER)buf;
    if (pDos->e_magic != IMAGE_DOS_SIGNATURE) {
        return false;
    }

    // 2.PE
    IMAGE_NT_HEADERS *pNt = (PIMAGE_NT_HEADERS)(buf + pDos->e_lfanew);
    if (pNt->Signature != IMAGE_NT_SIGNATURE) {
        return false;
    }

    return true;
}

VOID CMyPE::
ShowNtHeader(PIMAGE_NT_HEADERS pNt) {
    char nt_section_num[MAX_PATH] = {};
    wchar_t nt_date[MAX_PATH] = {};
    char nt_exdSize[MAX_PATH] = {};
    char nt_flagnum[MAX_PATH] = {};
    char nt_code_base[MAX_PATH] = {};
    char nt_data_base[MAX_PATH] = {};
    char nt_img_size[MAX_PATH] = {};
    char nt_img_base[MAX_PATH] = {};
    char nt_oep[MAX_PATH] = {};

    // 1.区段数目
    sprintf_s(nt_section_num, MAX_PATH, "%d", pNt->FileHeader.NumberOfSections);
    ui->SectionNum->setText(nt_section_num);

    // 2. 日期
    tm t1 = {};
    time_t t2 = pNt->FileHeader.TimeDateStamp;
    gmtime_s(&t1, &t2);
    swprintf_s(nt_date, MAX_PATH, L"时间：%d月%d日 %d时%d分%d秒\n", t1.tm_mon + 1, t1.tm_mday, t1.tm_hour + 1, t1.tm_min, t1.tm_sec);
    ui->DateTime->setText(QString::fromWCharArray(nt_date));

    // 3.扩展头大小/标志字
    sprintf_s(nt_exdSize, MAX_PATH, "0x%X\n", pNt->FileHeader.SizeOfOptionalHeader);
    ui->ExdSize->setText(nt_exdSize);

    sprintf_s(nt_flagnum, MAX_PATH, "0x%X\n", pNt->OptionalHeader.Magic);
    ui->FlagNum->setText(nt_flagnum);

    // 4.数据基址
    sprintf_s(nt_data_base, MAX_PATH, "0x%X\n", pNt->OptionalHeader.BaseOfData);
    ui->DataBaseAddr->setText(nt_data_base);

    // 5.代码基址
    sprintf_s(nt_code_base, MAX_PATH, "0x%X\n", pNt->OptionalHeader.BaseOfCode);
    ui->CodeBaseAddr->setText(nt_code_base);

    // 6.镜像大小
    sprintf_s(nt_img_size, MAX_PATH, "0x%X\n", pNt->OptionalHeader.SizeOfImage);
    ui->ImgSize->setText(nt_img_size);

    // 7.镜像基址
    sprintf_s(nt_img_base, MAX_PATH, "0x%X\n", pNt->OptionalHeader.ImageBase);
    ui->ImgBase->setText(nt_img_base);

    // 8.入口点
    sprintf_s(nt_oep, MAX_PATH, "0x%X\n", pNt->OptionalHeader.AddressOfEntryPoint);
    ui->EntryPoint->setText(nt_oep);
}

/* 区段表 */
void CMyPE::
ShowSections(PIMAGE_NT_HEADERS pNt) {
    PIMAGE_SECTION_HEADER pSec = IMAGE_FIRST_SECTION(pNt);
    ui->tableWidget->clearContents();
    for (size_t i = 0; i < pNt->FileHeader.NumberOfSections; i++) {
        char nt_virtualAddr[MAX_PATH] = {};
        char nt_virtualSize[MAX_PATH] = {};
        char nt_rawAddr[MAX_PATH] = {};
        char nt_rawSize[MAX_PATH] = {};
        char nt_charater[MAX_PATH] = {};

        sprintf_s(nt_virtualAddr, MAX_PATH, "0x%X\n", pSec[i].VirtualAddress);
        sprintf_s(nt_virtualSize, MAX_PATH, "0x%X\n", pSec[i].Misc.VirtualSize);
        sprintf_s(nt_rawAddr, MAX_PATH, "0x%X\n", pSec[i].PointerToRawData);
        sprintf_s(nt_rawSize, MAX_PATH, "0x%X\n", pSec[i].SizeOfRawData);
        sprintf_s(nt_charater, MAX_PATH, "0x%X\n", pSec[i].Characteristics);

        ui->tableWidget->setItem(i, 0, new QTableWidgetItem((char*)pSec[i].Name));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(nt_virtualAddr));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(nt_virtualSize));
        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(nt_rawAddr));
        ui->tableWidget->setItem(i, 4, new QTableWidgetItem(nt_rawSize));
        ui->tableWidget->setItem(i, 5, new QTableWidgetItem(nt_charater));
    }
}

/* 目录表 */

void CMyPE::
ShowDataDir(PIMAGE_NT_HEADERS pNt) {
    const char* DataDir[16] = {
        u8"导出表", u8"导入表", u8"资源", u8"异常",
        u8"安全", u8"重定位", u8"调试", u8"版权",
        u8"全局指针", u8"TLS表", u8"载入配置", u8"输入范围",
        u8"IAT", u8"延迟输入", u8"COM", u8"保留"
    };
    char nt_datadir_rva[MAX_PATH] = {};
    char nt_datadir_size[MAX_PATH] = {};

    ui->tab_DataDir->clearContents();

    PIMAGE_DATA_DIRECTORY pExport = &pNt->OptionalHeader.DataDirectory[0];
    for (int i = 0; i < IMAGE_NUMBEROF_DIRECTORY_ENTRIES; i++) {
        sprintf_s(nt_datadir_rva, MAX_PATH, "0x%X", pExport[i].VirtualAddress);
        sprintf_s(nt_datadir_size, MAX_PATH, "0x%X", pExport[i].Size);
        ui->tab_DataDir->setItem(i, 0, new QTableWidgetItem(DataDir[i]));
        ui->tab_DataDir->setItem(i, 1, new QTableWidgetItem(nt_datadir_rva));
        ui->tab_DataDir->setItem(i, 2, new QTableWidgetItem(nt_datadir_size));
    }
}

/* 虚拟地址转文件偏移 */
DWORD CMyPE::
VirtualAddr2FileAddr(PIMAGE_NT_HEADERS pNt, DWORD rva) {
    PIMAGE_SECTION_HEADER pSec = IMAGE_FIRST_SECTION(pNt);

    for (size_t i = 0; i < pNt->FileHeader.NumberOfSections; i++) {
        // 落在哪个区段内  大于虚拟地址 小于 虚拟地址+区段在文件中的大小
        if (rva >= pSec[i].VirtualAddress && rva <= pSec[i].VirtualAddress + pSec[i].SizeOfRawData) {
            if (pSec[i].SizeOfRawData == 0) {
                return 0;
            }
            // rva在内存中的区段偏移 + 区段在文件中的基址
            return rva - pSec[i].VirtualAddress + pSec[i].PointerToRawData;
        }
    }

    return 0;
}

// 导出表
void CMyPE::
ShowExport(PBYTE buf) {
    // 先找到导出表位置
    // 定位到数据目录表
    IMAGE_DOS_HEADER *pDos = (PIMAGE_DOS_HEADER)buf;
    IMAGE_NT_HEADERS *pNt = (PIMAGE_NT_HEADERS)(buf + pDos->e_lfanew);
    PIMAGE_DATA_DIRECTORY pExportDir = &pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];

    DWORD dwFOA = VirtualAddr2FileAddr(pNt, pExportDir->VirtualAddress);
    PIMAGE_EXPORT_DIRECTORY pExportData = (PIMAGE_EXPORT_DIRECTORY)(buf + dwFOA);

    // 函数地址表地址 转文件偏移 + buf 得到数组在文件中的位置
    PDWORD pAddrTab = (PDWORD)(buf + VirtualAddr2FileAddr(pNt, pExportData->AddressOfFunctions));
    // 名称表地址
    PDWORD pFunNameTab = (PDWORD)(buf + VirtualAddr2FileAddr(pNt, pExportData->AddressOfNames));
    // 序号表地址
    PWORD pOrdTab = (PWORD)(buf + VirtualAddr2FileAddr(pNt, pExportData->AddressOfNameOrdinals));

    ui->tab_export->clearContents();
    ui->tab_export->setRowCount(pExportData->NumberOfFunctions);

    for (size_t i = 0; i < pExportData->NumberOfFunctions; i++) {
        char t_export_name[64] = {};
        char t_export_ordinal[20] = {};
        char t_export_rva[20] = {};

        // 导出序号
        sprintf_s(t_export_ordinal, 20, "%d", i + pExportData->Base);
        // 地址
        sprintf_s(t_export_rva, 20, "0x%08X ", pAddrTab[i]);
        // 名称
        bool bFind = false;

        for (size_t j = 0; j < pExportData->NumberOfNames; j++) {
            if (pOrdTab[j] == i) {
                // 说明这个函数由名称
                // 名称偏移
                sprintf_s(t_export_name, 64, "%s", buf + VirtualAddr2FileAddr(pNt, pFunNameTab[j]));
                bFind = true;
                break;
            }
        }

        if (!bFind) {
            sprintf_s(t_export_name, 64, "---");
        }

        ui->tab_export->setItem(i, 0, new QTableWidgetItem(t_export_ordinal));
        ui->tab_export->setItem(i, 1, new QTableWidgetItem(t_export_rva));
        ui->tab_export->setItem(i, 2, new QTableWidgetItem(t_export_name));
    }
}

/* 导入表 */
void CMyPE::
ShowImport(PBYTE buf) {
    // 先找到导入表数组位置
    // 定位到数据目录表
    IMAGE_DOS_HEADER *pDos = (PIMAGE_DOS_HEADER)buf;
    IMAGE_NT_HEADERS *pNt = (PIMAGE_NT_HEADERS)(buf + pDos->e_lfanew);
    PIMAGE_DATA_DIRECTORY pImportDir = &pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];

    DWORD dwFOA = VirtualAddr2FileAddr(pNt, pImportDir->VirtualAddress);
    PIMAGE_IMPORT_DESCRIPTOR pImportData = (PIMAGE_IMPORT_DESCRIPTOR)(buf + dwFOA);

    ui->Import_Table->clear();

    while (pImportData->OriginalFirstThunk) {
        QTreeWidgetItem* item = new QTreeWidgetItem;
        wchar_t dll_name[128] = {};
        // DLL名称：
        //_snwprintf_s(dll_name, 128, L"%s", (wchar_t*)(buf + VirtualAddr2FileAddr(pNt, pImportData->Name)));
        MultiByteToWideChar(CP_ACP, NULL,
                            (char*)(buf + VirtualAddr2FileAddr(pNt, pImportData->Name)),
                            -1, dll_name,
                            strlen((char*)(buf + VirtualAddr2FileAddr(pNt, pImportData->Name))));
        item->setText( 0 , QString::fromWCharArray(dll_name));

        // 函数名称或序号
        // 数组首地址
        PIMAGE_THUNK_DATA pDataTab = (PIMAGE_THUNK_DATA)(buf + VirtualAddr2FileAddr(pNt, pImportData->OriginalFirstThunk));

        while (pDataTab->u1.Ordinal) {
            QTreeWidgetItem* subItem = new QTreeWidgetItem;
            wchar_t import_method[128] = {};
            wchar_t import_hint[10] = {};
            wchar_t import_fun_name[128] = {};
            // 是名称还是序号导入
            if (IMAGE_SNAP_BY_ORDINAL(pDataTab->u1.Ordinal)) {
                _snwprintf_s(import_method, 128, L"序号导入->[%d]", pDataTab->u1.Ordinal & 0xffff);
                _snwprintf_s(import_hint, 10, L"---");
                _snwprintf_s(import_fun_name, 10, L"---");
            } else {
                // 名称导入
                PIMAGE_IMPORT_BY_NAME pHintName =
                    (PIMAGE_IMPORT_BY_NAME)(buf + VirtualAddr2FileAddr(pNt, pDataTab->u1.AddressOfData));
                _snwprintf_s(import_method, 128, L"名称导入");
                _snwprintf_s(import_hint, 10, L"%d", pHintName->Hint);
                //_snwprintf_s(import_fun_name, 10, L"%s", pHintName->Name);
                MultiByteToWideChar(CP_ACP, NULL, pHintName->Name, -1, import_fun_name, strlen(pHintName->Name));
            }

            subItem->setText( 1 , QString::fromWCharArray(import_method));
            subItem->setText( 2 , QString::fromWCharArray(import_hint) );
            subItem->setText( 3 , QString::fromWCharArray(import_fun_name) );

            item->addChild( subItem );

            pDataTab++;
        }

        ui->Import_Table->addTopLevelItem(item);

        pImportData++;
    }
}

/* 重定位表 */
void CMyPE::
ShowRelocInfo(PBYTE buf) {
    // 先找到重定位信息的起始地址
    // 重定位的数据目录表
    IMAGE_DOS_HEADER *pDos = (PIMAGE_DOS_HEADER)buf;
    IMAGE_NT_HEADERS *pNt = (PIMAGE_NT_HEADERS)(buf + pDos->e_lfanew);
    PIMAGE_DATA_DIRECTORY pDataDir = &pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];

    // 重定位信息位置
    PIMAGE_BASE_RELOCATION pReloc = (PIMAGE_BASE_RELOCATION)(VirtualAddr2FileAddr(pNt, pDataDir->VirtualAddress) + buf);

    ui->treeWidget_Relocation->clear();

    DWORD dwCount = 0;
    while (dwCount <= pDataDir->Size && pReloc->VirtualAddress) {
        wchar_t relocation_start_rva[128] = {};
        wchar_t relocation_block_size[128] = {};
        QTreeWidgetItem* item = new QTreeWidgetItem;
        // 输出每一个需要重定位快的偏移
        _snwprintf_s(relocation_start_rva, 128, L"0x%X", pReloc->VirtualAddress);
        item->setText( 0 , QString::fromWCharArray(relocation_start_rva));

        // 由于SizeOfBlock里包含了头结构体的大小，因此，需要减去，另外，这个头指示的区域内，剩下的，16个Byte一组
        DWORD dwSubCount = (pReloc->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);
        _snwprintf_s(relocation_block_size, 128, L"%d", dwSubCount);
        item->setText( 1 , QString::fromWCharArray(relocation_block_size));

        typedef struct TypeOffset {
            WORD Offset : 12;
            WORD Type : 4;
        } TYPEOFFSET;

        // 结构体数组首地址
        TYPEOFFSET *pInfo = (TYPEOFFSET*)(pReloc + 1);
        for (unsigned int i = 0; i < dwSubCount; i++) {
            QTreeWidgetItem* subItem = new QTreeWidgetItem;
            wchar_t relocation_sub_rva[128] = {};
            wchar_t relocation_sub_fileoffset[128] = {};
            wchar_t relocation_sub_type[10] = {};
            wchar_t relocation_va[128] = {};
            wchar_t relocation_data[MAX_PATH] = {};

            _snwprintf_s(relocation_sub_type, 128, L"%d", pInfo[i].Type);

            if (pInfo[i].Type == 3) {
                // 找到需要重定位的数据的RVA
                DWORD dwRVA = pReloc->VirtualAddress + pInfo[i].Offset;
                _snwprintf_s(relocation_sub_rva, 128, L"0x%X", dwRVA);

                // 需要重定位的数据的地址
                PDWORD pAddr = (PDWORD)(buf + VirtualAddr2FileAddr(pNt, dwRVA));
                _snwprintf_s(relocation_sub_fileoffset, 128, L"0x%X", VirtualAddr2FileAddr(pNt, dwRVA));
                _snwprintf_s(relocation_va, 128, L"0x%X", *pAddr);

                // 这个一个VA，所以，需要先转换为RVA以后，再操作
                // 找到需要重定位的数据实际指向的内容
                // 虚拟地址转RVA
                (*pAddr - pNt->OptionalHeader.ImageBase);
                //再转文件偏移
                VirtualAddr2FileAddr(pNt, *pAddr - pNt->OptionalHeader.ImageBase);
                //再加文件首地址
                PBYTE pShowData = buf + VirtualAddr2FileAddr(pNt, *pAddr - pNt->OptionalHeader.ImageBase);
                _snwprintf_s(relocation_data, MAX_PATH, L"%02X %02X %02X %02X %02X %02X %02X %02X",
                    *pShowData, *(pShowData + 1), *(pShowData + 2), *(pShowData + 3),
                    *(pShowData + 4), *(pShowData + 5), *(pShowData + 6), *(pShowData + 7));
            } else {
                DWORD dwRVA = pReloc->VirtualAddress + pInfo[i].Offset;
                _snwprintf_s(relocation_sub_rva, 128, L"0x%X", dwRVA);
                _snwprintf_s(relocation_sub_fileoffset, 128, L"0x%X", VirtualAddr2FileAddr(pNt, dwRVA));
                _snwprintf_s(relocation_va, 128, L"---");
                _snwprintf_s(relocation_data, 128, L"---");

            }

            subItem->setText( 2 , QString::fromWCharArray(relocation_sub_rva) );
            subItem->setText( 3 , QString::fromWCharArray(relocation_sub_fileoffset) );
            subItem->setText( 4 , QString::fromWCharArray(relocation_sub_type) );
            subItem->setText( 5 , QString::fromWCharArray(relocation_va) );
            subItem->setText( 6 , QString::fromWCharArray(relocation_data) );

            item->addChild( subItem );
        }

        ui->treeWidget_Relocation->addTopLevelItem(item);
        // 递归条件
        pReloc = (PIMAGE_BASE_RELOCATION)((DWORD)pReloc + pReloc->SizeOfBlock);
        dwCount += pReloc->SizeOfBlock;
    }
}

/* 资源表 */
void CMyPE::
ShowResource(PBYTE buf) {
    // 标准资源指针数组
    const wchar_t* szRes[17] = {
        L"", L"光标", L"位图", L"图标",
    L"菜单", L"对话框", L"字符串列表", L"字体目录",
    L"字体", L"快捷键", L"非格式化资源", L"消息列表",
    L"光标组", L"", L"图标组", L"", L"版本信息" };

    // 先找到资源起始位置：第一层目录起始地址
    IMAGE_DOS_HEADER *pDos = (PIMAGE_DOS_HEADER)buf;
    IMAGE_NT_HEADERS *pNt = (PIMAGE_NT_HEADERS)(buf + pDos->e_lfanew);

    PIMAGE_DATA_DIRECTORY pDataDir = &pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE];

    PIMAGE_RESOURCE_DIRECTORY pDirFst = (PIMAGE_RESOURCE_DIRECTORY)(
                VirtualAddr2FileAddr(pNt, pDataDir->VirtualAddress) + buf);

    // 资源种类数
    DWORD dwCountKind = pDirFst->NumberOfIdEntries + pDirFst->NumberOfNamedEntries;

    // 第一层目录的结构体数组起始地址
    PIMAGE_RESOURCE_DIRECTORY_ENTRY  pDataFst = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)(pDirFst + 1);// 不要++

    ui->treeWidget_Resource->clear();

    for (unsigned int i = 0; i < dwCountKind; i++) {
        QTreeWidgetItem* item = new QTreeWidgetItem;
        wchar_t resource_kind[MAX_PATH] = {};
        // 是以名称命名的资源还是以ID命名的资源
        if (pDataFst[i].NameIsString) {
            // 找到名称偏移
            PIMAGE_RESOURCE_DIR_STRING_U pNameU = (PIMAGE_RESOURCE_DIR_STRING_U)(pDataFst[i].NameOffset + (DWORD)pDirFst);
            // 拷贝出来名称
            WCHAR *pName = new WCHAR[pNameU->Length + 1]{};
            memcpy_s(pName, sizeof(WCHAR)*pNameU->Length, pNameU->NameString, sizeof(WCHAR)*pNameU->Length);
            //_snwprintf_s(resource_kind, MAX_PATH, L"%S", pName);
            memcpy_s(resource_kind, MAX_PATH, pNameU->NameString, sizeof(WCHAR)*pNameU->Length);

            delete[] pName;
        } else {
            // 以ID命名的标准资源或非标准资源
            if ((pDataFst[i].Id >= 1 && pDataFst[i].Id <= 12) ||
                pDataFst[i].Id == 14 || pDataFst[i].Id == 16) {
                // 标准资源
                //_snwprintf_s(resource_kind, MAX_PATH, L"%S", szRes[pDataFst[i].Id]);
                memcpy_s(resource_kind, MAX_PATH, szRes[pDataFst[i].Id], wcslen(szRes[pDataFst[i].Id]));
            } else {
                // 非标准资源直接输出ID
                _snwprintf_s(resource_kind, MAX_PATH, L"%d", pDataFst[i].Id);
            }
        }

        item->setText( 0 , QString::fromWCharArray(resource_kind));

        // 下面是第二层目录 起始地址
        PIMAGE_RESOURCE_DIRECTORY pDirSec = (PIMAGE_RESOURCE_DIRECTORY)(pDataFst[i].OffsetToDirectory + (DWORD)pDirFst);
        // 该种资源有多少个
        DWORD dwCountID = pDirSec->NumberOfIdEntries + pDirSec->NumberOfNamedEntries;
        // 资源数组的起始地址
        PIMAGE_RESOURCE_DIRECTORY_ENTRY pDataSec = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)(pDirSec + 1);
        for (unsigned int j = 0; j < dwCountID; j++) {
            QTreeWidgetItem* subItem = new QTreeWidgetItem;
            wchar_t resource_name[MAX_PATH] = {};
            // 这个资源是ID还是字符串
            if (pDataSec[j].NameIsString) {
                // 找到名称偏移
                PIMAGE_RESOURCE_DIR_STRING_U pNameU = (PIMAGE_RESOURCE_DIR_STRING_U)(pDataSec[j].NameOffset + (DWORD)pDirFst);
                // 拷贝出来名称
                WCHAR *pName = new WCHAR[pNameU->Length + 1]{};
                memcpy_s(pName, sizeof(WCHAR)*pNameU->Length, pNameU->NameString, sizeof(WCHAR)*pNameU->Length);
                //_snwprintf_s(resource_name, MAX_PATH, L"%S", pName);
                memcpy_s(resource_name, MAX_PATH, pNameU->NameString, sizeof(WCHAR)*pNameU->Length);
                delete[] pName;
            } else {
                _snwprintf_s(resource_name, MAX_PATH, L"%d", pDataSec[j].Id);
            }

            subItem->setText( 1 , QString::fromWCharArray(resource_name) );

            // 第三层目录
            PIMAGE_RESOURCE_DIRECTORY pDirThr = (PIMAGE_RESOURCE_DIRECTORY)((DWORD)pDirFst + pDataSec[j].OffsetToDirectory);
            // 数据
            PIMAGE_RESOURCE_DIRECTORY_ENTRY pDataThr = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)(pDirThr + 1);
            // 代码页
            wchar_t resource_CodePage[128] = {};
            wchar_t resource_offset[128] = {};
            wchar_t resource_rva[128] = {};
            wchar_t resource_size[128] = {};

            /* 代码页 */
            _snwprintf_s(resource_CodePage, MAX_PATH, L"%d", pDataThr->Name);
            // 资源具体信息偏移
            _snwprintf_s(resource_offset, MAX_PATH, L"%X", pDataThr->OffsetToData);

            // 最终信息
            PIMAGE_RESOURCE_DATA_ENTRY pLast = (PIMAGE_RESOURCE_DATA_ENTRY)(pDataThr->OffsetToData + (DWORD)pDirFst);
            _snwprintf_s(resource_rva, MAX_PATH, L"%X", pLast->OffsetToData);
            _snwprintf_s(resource_size, MAX_PATH, L"%X", pLast->Size);

            subItem->setText( 2 , QString::fromWCharArray(resource_CodePage) );
            subItem->setText( 3 , QString::fromWCharArray(resource_offset) );
            subItem->setText( 4 , QString::fromWCharArray(resource_rva) );
            subItem->setText( 5 , QString::fromWCharArray(resource_size) );

            item->addChild( subItem );
        }

        ui->treeWidget_Resource->addTopLevelItem(item);
    }
}

/* TLS */
void CMyPE::
ShowTls(PBYTE buf) {
    wchar_t tls_start_rva[128] = {};
    wchar_t tls_end_rva[128] = {};
    wchar_t tls_AddressIndex[128] = {};
    wchar_t tls_AddressCallBack[128] = {};
    wchar_t tls_FillZero[128] = {};
    wchar_t tls_Character[128] = {};

    IMAGE_DOS_HEADER *pDos = (PIMAGE_DOS_HEADER)buf;
    IMAGE_NT_HEADERS *pNt = (PIMAGE_NT_HEADERS)(buf + pDos->e_lfanew);

    PIMAGE_DATA_DIRECTORY pTlsDir = &pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS];
    if (!pTlsDir->Size) {
        ui->TLS_BlockStartAddr->clear();
        ui->TLS_BlockEndAddr->clear();
        ui->TLS_IndexVA->clear();
        ui->TLS_CallBackVA->clear();
        ui->TLS_Reserved->clear();
        ui->TLS_Character->clear();
        return;
    }

    DWORD dwFOA = VirtualAddr2FileAddr(pNt, pTlsDir->VirtualAddress);
    PIMAGE_TLS_DIRECTORY pTlsData = (PIMAGE_TLS_DIRECTORY)(buf + dwFOA);

    swprintf_s(tls_start_rva, 128, L"%X", pTlsData->StartAddressOfRawData);
    swprintf_s(tls_end_rva, 128, L"%X", pTlsData->EndAddressOfRawData);
    swprintf_s(tls_AddressIndex, 128, L"%X", pTlsData->AddressOfIndex);
    swprintf_s(tls_AddressCallBack, 128, L"%X", pTlsData->AddressOfCallBacks);
    swprintf_s(tls_FillZero, 128, L"%X", pTlsData->SizeOfZeroFill);
    swprintf_s(tls_Character, 128, L"%X", pTlsData->Characteristics);

    ui->TLS_BlockStartAddr->setText(QString::fromWCharArray(tls_start_rva));
    ui->TLS_BlockEndAddr->setText(QString::fromWCharArray(tls_end_rva));
    ui->TLS_IndexVA->setText(QString::fromWCharArray(tls_AddressIndex));
    ui->TLS_CallBackVA->setText(QString::fromWCharArray(tls_AddressCallBack));
    ui->TLS_Reserved->setText(QString::fromWCharArray(tls_FillZero));
    ui->TLS_Character->setText(QString::fromWCharArray(tls_Character));
}

void CMyPE::
ShowDelayDll(PBYTE buf) {
    wchar_t delayDllName_RVA[128] = {};
    wchar_t delayDllIAT_RVA[128] = {};
    wchar_t delayDllINT_RVA[128] = {};
    wchar_t delayDllTime[128] = {};

    IMAGE_DOS_HEADER *pDos = (PIMAGE_DOS_HEADER)buf;
    IMAGE_NT_HEADERS *pNt = (PIMAGE_NT_HEADERS)(buf + pDos->e_lfanew);

    PIMAGE_DATA_DIRECTORY pDelayDllDir = &pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT];
    if (!pDelayDllDir->Size) {
        ui->lineEdit_DllNameRVA->clear();
        ui->lineEdit_IAT_RVA->clear();
        ui->lineEdit_INT_RVA->clear();
        ui->lineEdit_Time->clear();
        return;
    }

    DWORD dwFOA = VirtualAddr2FileAddr(pNt, pDelayDllDir->VirtualAddress);
    PIMAGE_DELAYLOAD_DESCRIPTOR pDelayDllData = (PIMAGE_DELAYLOAD_DESCRIPTOR)(buf + dwFOA);

    swprintf_s(delayDllName_RVA, 128, L"%X", pDelayDllData->DllNameRVA);
    swprintf_s(delayDllIAT_RVA, 128, L"%X", pDelayDllData->ImportAddressTableRVA);
    swprintf_s(delayDllINT_RVA, 128, L"%X", pDelayDllData->ImportNameTableRVA);
    swprintf_s(delayDllTime, 128, L"%X", pDelayDllData->TimeDateStamp);

    ui->lineEdit_DllNameRVA->setText(QString::fromWCharArray(delayDllName_RVA));
    ui->lineEdit_IAT_RVA->setText(QString::fromWCharArray(delayDllIAT_RVA));
    ui->lineEdit_INT_RVA->setText(QString::fromWCharArray(delayDllINT_RVA));
    ui->lineEdit_Time->setText(QString::fromWCharArray(delayDllTime));
}
