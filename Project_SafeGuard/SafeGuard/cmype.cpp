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
    header << u8"����"
           << "VOffset" << "VSize"
           << "ROffset" << "RSize"
           << u8"��־";
    ui->tableWidget->setHorizontalHeaderLabels(header);

    ui->tab_DataDir->setColumnCount(3);
    ui->tab_DataDir->setRowCount(16);
    QStringList header_dataDir;
    header_dataDir << u8"����" << "RVA" << u8"��С";
    ui->tab_DataDir->setHorizontalHeaderLabels(header_dataDir);

    ui->tab_export->setColumnCount(3);
    QStringList header_export;
    header_export << u8"���" << u8"������ַ[RVA]" << u8"������";
    ui->tab_export->setHorizontalHeaderLabels(header_export);

    QTreeWidgetItem* headerItems = new QTreeWidgetItem;
    headerItems->setText( 0 , u8"DLL[����]" );
    headerItems->setText( 1 , u8"���뷽ʽ" );
    headerItems->setText( 2 , "Hint" );
    headerItems->setText( 3 , u8"������" );
    ui->Import_Table->setHeaderItem( headerItems );

    QTreeWidgetItem* headerItems_Relocation = new QTreeWidgetItem;
    headerItems_Relocation->setText( 0 , u8"������ʼRVA" );
    headerItems_Relocation->setText( 1 , u8"�����С" );
    headerItems_Relocation->setText( 2 , u8"��Ҫ�����ض�λ��RVA" );
    headerItems_Relocation->setText( 3 , u8"�����ض�λ��RVA���ļ���ƫ����" );
    headerItems_Relocation->setText( 4 , u8"����[һ��Ϊ3]" );
    headerItems_Relocation->setText( 5 , u8"File Offsetλ�õ�����VA" );
    headerItems_Relocation->setText( 6 , u8"�ض�λ����[VA-ImgBase��תΪ�ļ�ƫ��]" );
    ui->treeWidget_Relocation->setHeaderItem( headerItems_Relocation );


    QTreeWidgetItem* headerItems_Resource = new QTreeWidgetItem;
    headerItems_Resource->setText( 0 , u8"��Դ����" );
    headerItems_Resource->setText( 1 , u8"��Դ����" );
    headerItems_Resource->setText( 2 , u8"����ҳ" );
    headerItems_Resource->setText( 3 , u8"��Դ��Ϣƫ��" );
    headerItems_Resource->setText( 4 , u8"��ԴRVA" );
    headerItems_Resource->setText( 5 , u8"��ԴSize" );
    ui->treeWidget_Resource->setHeaderItem( headerItems_Resource );

    this->setStyleSheet("background-color:rgb(164,214,215)");
}

CMyPE::~CMyPE()
{
    delete ui;
}

//���û��϶��ļ������ڲ�����ʱ�򣬾ͻᴥ��dragEnterEvent�¼�
void CMyPE::
dragEnterEvent(QDragEnterEvent *event) {
    //���Ϊ�ļ�����֧���Ϸ�
    if (event->mimeData()->hasFormat("text/uri-list"))
        event->acceptProposedAction();
}

//���û���������ļ��󣬾ͻᴥ��dropEvent�¼�
void CMyPE::
dropEvent(QDropEvent *event) {
    //ע�⣺��������ж��ļ����ڣ���˼���û�һ�����϶��˶���ļ����������϶�һ��Ŀ¼
    //������ȡ����Ŀ¼�����ڲ�ͬ�Ĳ���ƽ̨�£��Լ���д����ʵ�ֶ�ȡ����Ŀ¼�ļ���
    QList<QUrl> urls = event->mimeData()->urls();
    if(urls.isEmpty())
        return;

    //���ı�����׷���ļ���
    foreach(QUrl url, urls) {
        QString file_name = url.toLocalFile();
        qDebug() << file_name;

        HANDLE hFile = CreateFile(file_name.toStdWString().c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        DWORD dwSize = GetFileSize(hFile, NULL);
        // �����ڴ�
        LPBYTE p = new BYTE[dwSize]{};
        // ���ļ����ݶ����ڴ���
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

    // 1.������Ŀ
    sprintf_s(nt_section_num, MAX_PATH, "%d", pNt->FileHeader.NumberOfSections);
    ui->SectionNum->setText(nt_section_num);

    // 2. ����
    tm t1 = {};
    time_t t2 = pNt->FileHeader.TimeDateStamp;
    gmtime_s(&t1, &t2);
    swprintf_s(nt_date, MAX_PATH, L"ʱ�䣺%d��%d�� %dʱ%d��%d��\n", t1.tm_mon + 1, t1.tm_mday, t1.tm_hour + 1, t1.tm_min, t1.tm_sec);
    ui->DateTime->setText(QString::fromWCharArray(nt_date));

    // 3.��չͷ��С/��־��
    sprintf_s(nt_exdSize, MAX_PATH, "0x%X\n", pNt->FileHeader.SizeOfOptionalHeader);
    ui->ExdSize->setText(nt_exdSize);

    sprintf_s(nt_flagnum, MAX_PATH, "0x%X\n", pNt->OptionalHeader.Magic);
    ui->FlagNum->setText(nt_flagnum);

    // 4.���ݻ�ַ
    sprintf_s(nt_data_base, MAX_PATH, "0x%X\n", pNt->OptionalHeader.BaseOfData);
    ui->DataBaseAddr->setText(nt_data_base);

    // 5.�����ַ
    sprintf_s(nt_code_base, MAX_PATH, "0x%X\n", pNt->OptionalHeader.BaseOfCode);
    ui->CodeBaseAddr->setText(nt_code_base);

    // 6.�����С
    sprintf_s(nt_img_size, MAX_PATH, "0x%X\n", pNt->OptionalHeader.SizeOfImage);
    ui->ImgSize->setText(nt_img_size);

    // 7.�����ַ
    sprintf_s(nt_img_base, MAX_PATH, "0x%X\n", pNt->OptionalHeader.ImageBase);
    ui->ImgBase->setText(nt_img_base);

    // 8.��ڵ�
    sprintf_s(nt_oep, MAX_PATH, "0x%X\n", pNt->OptionalHeader.AddressOfEntryPoint);
    ui->EntryPoint->setText(nt_oep);
}

/* ���α� */
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

/* Ŀ¼�� */

void CMyPE::
ShowDataDir(PIMAGE_NT_HEADERS pNt) {
    const char* DataDir[16] = {
        u8"������", u8"�����", u8"��Դ", u8"�쳣",
        u8"��ȫ", u8"�ض�λ", u8"����", u8"��Ȩ",
        u8"ȫ��ָ��", u8"TLS��", u8"��������", u8"���뷶Χ",
        u8"IAT", u8"�ӳ�����", u8"COM", u8"����"
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

/* �����ַת�ļ�ƫ�� */
DWORD CMyPE::
VirtualAddr2FileAddr(PIMAGE_NT_HEADERS pNt, DWORD rva) {
    PIMAGE_SECTION_HEADER pSec = IMAGE_FIRST_SECTION(pNt);

    for (size_t i = 0; i < pNt->FileHeader.NumberOfSections; i++) {
        // �����ĸ�������  ���������ַ С�� �����ַ+�������ļ��еĴ�С
        if (rva >= pSec[i].VirtualAddress && rva <= pSec[i].VirtualAddress + pSec[i].SizeOfRawData) {
            if (pSec[i].SizeOfRawData == 0) {
                return 0;
            }
            // rva���ڴ��е�����ƫ�� + �������ļ��еĻ�ַ
            return rva - pSec[i].VirtualAddress + pSec[i].PointerToRawData;
        }
    }

    return 0;
}

// ������
void CMyPE::
ShowExport(PBYTE buf) {
    // ���ҵ�������λ��
    // ��λ������Ŀ¼��
    IMAGE_DOS_HEADER *pDos = (PIMAGE_DOS_HEADER)buf;
    IMAGE_NT_HEADERS *pNt = (PIMAGE_NT_HEADERS)(buf + pDos->e_lfanew);
    PIMAGE_DATA_DIRECTORY pExportDir = &pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];

    DWORD dwFOA = VirtualAddr2FileAddr(pNt, pExportDir->VirtualAddress);
    PIMAGE_EXPORT_DIRECTORY pExportData = (PIMAGE_EXPORT_DIRECTORY)(buf + dwFOA);

    // ������ַ���ַ ת�ļ�ƫ�� + buf �õ��������ļ��е�λ��
    PDWORD pAddrTab = (PDWORD)(buf + VirtualAddr2FileAddr(pNt, pExportData->AddressOfFunctions));
    // ���Ʊ��ַ
    PDWORD pFunNameTab = (PDWORD)(buf + VirtualAddr2FileAddr(pNt, pExportData->AddressOfNames));
    // ��ű��ַ
    PWORD pOrdTab = (PWORD)(buf + VirtualAddr2FileAddr(pNt, pExportData->AddressOfNameOrdinals));

    ui->tab_export->clearContents();
    ui->tab_export->setRowCount(pExportData->NumberOfFunctions);

    for (size_t i = 0; i < pExportData->NumberOfFunctions; i++) {
        char t_export_name[64] = {};
        char t_export_ordinal[20] = {};
        char t_export_rva[20] = {};

        // �������
        sprintf_s(t_export_ordinal, 20, "%d", i + pExportData->Base);
        // ��ַ
        sprintf_s(t_export_rva, 20, "0x%08X ", pAddrTab[i]);
        // ����
        bool bFind = false;

        for (size_t j = 0; j < pExportData->NumberOfNames; j++) {
            if (pOrdTab[j] == i) {
                // ˵���������������
                // ����ƫ��
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

/* ����� */
void CMyPE::
ShowImport(PBYTE buf) {
    // ���ҵ����������λ��
    // ��λ������Ŀ¼��
    IMAGE_DOS_HEADER *pDos = (PIMAGE_DOS_HEADER)buf;
    IMAGE_NT_HEADERS *pNt = (PIMAGE_NT_HEADERS)(buf + pDos->e_lfanew);
    PIMAGE_DATA_DIRECTORY pImportDir = &pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];

    DWORD dwFOA = VirtualAddr2FileAddr(pNt, pImportDir->VirtualAddress);
    PIMAGE_IMPORT_DESCRIPTOR pImportData = (PIMAGE_IMPORT_DESCRIPTOR)(buf + dwFOA);

    ui->Import_Table->clear();

    while (pImportData->OriginalFirstThunk) {
        QTreeWidgetItem* item = new QTreeWidgetItem;
        wchar_t dll_name[128] = {};
        // DLL���ƣ�
        //_snwprintf_s(dll_name, 128, L"%s", (wchar_t*)(buf + VirtualAddr2FileAddr(pNt, pImportData->Name)));
        MultiByteToWideChar(CP_ACP, NULL,
                            (char*)(buf + VirtualAddr2FileAddr(pNt, pImportData->Name)),
                            -1, dll_name,
                            strlen((char*)(buf + VirtualAddr2FileAddr(pNt, pImportData->Name))));
        item->setText( 0 , QString::fromWCharArray(dll_name));

        // �������ƻ����
        // �����׵�ַ
        PIMAGE_THUNK_DATA pDataTab = (PIMAGE_THUNK_DATA)(buf + VirtualAddr2FileAddr(pNt, pImportData->OriginalFirstThunk));

        while (pDataTab->u1.Ordinal) {
            QTreeWidgetItem* subItem = new QTreeWidgetItem;
            wchar_t import_method[128] = {};
            wchar_t import_hint[10] = {};
            wchar_t import_fun_name[128] = {};
            // �����ƻ�����ŵ���
            if (IMAGE_SNAP_BY_ORDINAL(pDataTab->u1.Ordinal)) {
                _snwprintf_s(import_method, 128, L"��ŵ���->[%d]", pDataTab->u1.Ordinal & 0xffff);
                _snwprintf_s(import_hint, 10, L"---");
                _snwprintf_s(import_fun_name, 10, L"---");
            } else {
                // ���Ƶ���
                PIMAGE_IMPORT_BY_NAME pHintName =
                    (PIMAGE_IMPORT_BY_NAME)(buf + VirtualAddr2FileAddr(pNt, pDataTab->u1.AddressOfData));
                _snwprintf_s(import_method, 128, L"���Ƶ���");
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

/* �ض�λ�� */
void CMyPE::
ShowRelocInfo(PBYTE buf) {
    // ���ҵ��ض�λ��Ϣ����ʼ��ַ
    // �ض�λ������Ŀ¼��
    IMAGE_DOS_HEADER *pDos = (PIMAGE_DOS_HEADER)buf;
    IMAGE_NT_HEADERS *pNt = (PIMAGE_NT_HEADERS)(buf + pDos->e_lfanew);
    PIMAGE_DATA_DIRECTORY pDataDir = &pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];

    // �ض�λ��Ϣλ��
    PIMAGE_BASE_RELOCATION pReloc = (PIMAGE_BASE_RELOCATION)(VirtualAddr2FileAddr(pNt, pDataDir->VirtualAddress) + buf);

    ui->treeWidget_Relocation->clear();

    DWORD dwCount = 0;
    while (dwCount <= pDataDir->Size && pReloc->VirtualAddress) {
        wchar_t relocation_start_rva[128] = {};
        wchar_t relocation_block_size[128] = {};
        QTreeWidgetItem* item = new QTreeWidgetItem;
        // ���ÿһ����Ҫ�ض�λ���ƫ��
        _snwprintf_s(relocation_start_rva, 128, L"0x%X", pReloc->VirtualAddress);
        item->setText( 0 , QString::fromWCharArray(relocation_start_rva));

        // ����SizeOfBlock�������ͷ�ṹ��Ĵ�С����ˣ���Ҫ��ȥ�����⣬���ͷָʾ�������ڣ�ʣ�µģ�16��Byteһ��
        DWORD dwSubCount = (pReloc->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);
        _snwprintf_s(relocation_block_size, 128, L"%d", dwSubCount);
        item->setText( 1 , QString::fromWCharArray(relocation_block_size));

        typedef struct TypeOffset {
            WORD Offset : 12;
            WORD Type : 4;
        } TYPEOFFSET;

        // �ṹ�������׵�ַ
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
                // �ҵ���Ҫ�ض�λ�����ݵ�RVA
                DWORD dwRVA = pReloc->VirtualAddress + pInfo[i].Offset;
                _snwprintf_s(relocation_sub_rva, 128, L"0x%X", dwRVA);

                // ��Ҫ�ض�λ�����ݵĵ�ַ
                PDWORD pAddr = (PDWORD)(buf + VirtualAddr2FileAddr(pNt, dwRVA));
                _snwprintf_s(relocation_sub_fileoffset, 128, L"0x%X", VirtualAddr2FileAddr(pNt, dwRVA));
                _snwprintf_s(relocation_va, 128, L"0x%X", *pAddr);

                // ���һ��VA�����ԣ���Ҫ��ת��ΪRVA�Ժ��ٲ���
                // �ҵ���Ҫ�ض�λ������ʵ��ָ�������
                // �����ַתRVA
                (*pAddr - pNt->OptionalHeader.ImageBase);
                //��ת�ļ�ƫ��
                VirtualAddr2FileAddr(pNt, *pAddr - pNt->OptionalHeader.ImageBase);
                //�ټ��ļ��׵�ַ
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
        // �ݹ�����
        pReloc = (PIMAGE_BASE_RELOCATION)((DWORD)pReloc + pReloc->SizeOfBlock);
        dwCount += pReloc->SizeOfBlock;
    }
}

/* ��Դ�� */
void CMyPE::
ShowResource(PBYTE buf) {
    // ��׼��Դָ������
    const wchar_t* szRes[17] = {
        L"", L"���", L"λͼ", L"ͼ��",
    L"�˵�", L"�Ի���", L"�ַ����б�", L"����Ŀ¼",
    L"����", L"��ݼ�", L"�Ǹ�ʽ����Դ", L"��Ϣ�б�",
    L"�����", L"", L"ͼ����", L"", L"�汾��Ϣ" };

    // ���ҵ���Դ��ʼλ�ã���һ��Ŀ¼��ʼ��ַ
    IMAGE_DOS_HEADER *pDos = (PIMAGE_DOS_HEADER)buf;
    IMAGE_NT_HEADERS *pNt = (PIMAGE_NT_HEADERS)(buf + pDos->e_lfanew);

    PIMAGE_DATA_DIRECTORY pDataDir = &pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE];

    PIMAGE_RESOURCE_DIRECTORY pDirFst = (PIMAGE_RESOURCE_DIRECTORY)(
                VirtualAddr2FileAddr(pNt, pDataDir->VirtualAddress) + buf);

    // ��Դ������
    DWORD dwCountKind = pDirFst->NumberOfIdEntries + pDirFst->NumberOfNamedEntries;

    // ��һ��Ŀ¼�Ľṹ��������ʼ��ַ
    PIMAGE_RESOURCE_DIRECTORY_ENTRY  pDataFst = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)(pDirFst + 1);// ��Ҫ++

    ui->treeWidget_Resource->clear();

    for (unsigned int i = 0; i < dwCountKind; i++) {
        QTreeWidgetItem* item = new QTreeWidgetItem;
        wchar_t resource_kind[MAX_PATH] = {};
        // ����������������Դ������ID��������Դ
        if (pDataFst[i].NameIsString) {
            // �ҵ�����ƫ��
            PIMAGE_RESOURCE_DIR_STRING_U pNameU = (PIMAGE_RESOURCE_DIR_STRING_U)(pDataFst[i].NameOffset + (DWORD)pDirFst);
            // ������������
            WCHAR *pName = new WCHAR[pNameU->Length + 1]{};
            memcpy_s(pName, sizeof(WCHAR)*pNameU->Length, pNameU->NameString, sizeof(WCHAR)*pNameU->Length);
            //_snwprintf_s(resource_kind, MAX_PATH, L"%S", pName);
            memcpy_s(resource_kind, MAX_PATH, pNameU->NameString, sizeof(WCHAR)*pNameU->Length);

            delete[] pName;
        } else {
            // ��ID�����ı�׼��Դ��Ǳ�׼��Դ
            if ((pDataFst[i].Id >= 1 && pDataFst[i].Id <= 12) ||
                pDataFst[i].Id == 14 || pDataFst[i].Id == 16) {
                // ��׼��Դ
                //_snwprintf_s(resource_kind, MAX_PATH, L"%S", szRes[pDataFst[i].Id]);
                memcpy_s(resource_kind, MAX_PATH, szRes[pDataFst[i].Id], wcslen(szRes[pDataFst[i].Id]));
            } else {
                // �Ǳ�׼��Դֱ�����ID
                _snwprintf_s(resource_kind, MAX_PATH, L"%d", pDataFst[i].Id);
            }
        }

        item->setText( 0 , QString::fromWCharArray(resource_kind));

        // �����ǵڶ���Ŀ¼ ��ʼ��ַ
        PIMAGE_RESOURCE_DIRECTORY pDirSec = (PIMAGE_RESOURCE_DIRECTORY)(pDataFst[i].OffsetToDirectory + (DWORD)pDirFst);
        // ������Դ�ж��ٸ�
        DWORD dwCountID = pDirSec->NumberOfIdEntries + pDirSec->NumberOfNamedEntries;
        // ��Դ�������ʼ��ַ
        PIMAGE_RESOURCE_DIRECTORY_ENTRY pDataSec = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)(pDirSec + 1);
        for (unsigned int j = 0; j < dwCountID; j++) {
            QTreeWidgetItem* subItem = new QTreeWidgetItem;
            wchar_t resource_name[MAX_PATH] = {};
            // �����Դ��ID�����ַ���
            if (pDataSec[j].NameIsString) {
                // �ҵ�����ƫ��
                PIMAGE_RESOURCE_DIR_STRING_U pNameU = (PIMAGE_RESOURCE_DIR_STRING_U)(pDataSec[j].NameOffset + (DWORD)pDirFst);
                // ������������
                WCHAR *pName = new WCHAR[pNameU->Length + 1]{};
                memcpy_s(pName, sizeof(WCHAR)*pNameU->Length, pNameU->NameString, sizeof(WCHAR)*pNameU->Length);
                //_snwprintf_s(resource_name, MAX_PATH, L"%S", pName);
                memcpy_s(resource_name, MAX_PATH, pNameU->NameString, sizeof(WCHAR)*pNameU->Length);
                delete[] pName;
            } else {
                _snwprintf_s(resource_name, MAX_PATH, L"%d", pDataSec[j].Id);
            }

            subItem->setText( 1 , QString::fromWCharArray(resource_name) );

            // ������Ŀ¼
            PIMAGE_RESOURCE_DIRECTORY pDirThr = (PIMAGE_RESOURCE_DIRECTORY)((DWORD)pDirFst + pDataSec[j].OffsetToDirectory);
            // ����
            PIMAGE_RESOURCE_DIRECTORY_ENTRY pDataThr = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)(pDirThr + 1);
            // ����ҳ
            wchar_t resource_CodePage[128] = {};
            wchar_t resource_offset[128] = {};
            wchar_t resource_rva[128] = {};
            wchar_t resource_size[128] = {};

            /* ����ҳ */
            _snwprintf_s(resource_CodePage, MAX_PATH, L"%d", pDataThr->Name);
            // ��Դ������Ϣƫ��
            _snwprintf_s(resource_offset, MAX_PATH, L"%X", pDataThr->OffsetToData);

            // ������Ϣ
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
