//------------------------------------------------------------------------------
//
//    Copyright (C) Streamlet. All rights reserved.
//
//    File Name:   ZLibWrapLib.cpp
//    Author:      Streamlet
//    Create Time: 2010-09-16
//    Description:
//
//    Version history:
//
//
//
//------------------------------------------------------------------------------

// 查阅: http://www.cppblog.com/Streamlet/archive/2013/05/12/127368.html

/*  相关 API
 * 压缩相关：
 * zipOpen64   zipClose    zipOpenNewFileInZip zipCloseFileInZip   zipWriteInFileInZip
 *
 * 解压相关：
 * unzOpen64   unzClose    unzGetGlobalInfo64  unzGoToNextFile
 * unzGetCurrentFileInfo64 unzOpenCurrentFile  unzCloseCurrentFile unzReadCurrentFile
 *
 * 首先是压缩操作。使用 zipOpen64 来打开/创建一个 ZIP 文件，然后开始遍历要被放到压缩包中去的文件。
 * 针对每个文件，先调用一次 zipOpenNewFileInZip，然后开始读原始文件数据，使用 zipWriteInFileInZip 来写入到 ZIP 文件中去。
 * zipOpenNewFileInZip 的第三个参数是一个 zip_fileinfo 结构，该结构数据可全部置零，其中 dosDate 可用于填入一个时间（LastModificationTime）。
 * 它的第二个参数是 ZIP 中的文件名，若要保持目录结构，该参数中可以保留路径，如 foo/bar.txt。
 *
 * 解压操作稍微复杂一点点。打开一个 ZIP 文件后，需要先使用 unzGetGlobalInfo64 来取得该文件的一些信息，来了解这个压缩包里一共包含了多少个文件，等等。
 * 目前我们用得着的就是这个文件数目。然后开始遍历 ZIP 中的文件，初始时自动会定位在第一个文件，以后处理完一个后用 unzGoToNextFile 来跳到下一个文件。
 * 对于每个内部文件，可用 unzGetCurrentFileInfo64 来查内部文件名。这个文件名和刚才 zipOpenNewFileInZip 的第二个参数是一样的形式，所以有可能包含路径。
 * 也有可能会以路径分隔符（/）结尾，表明这是个目录项（其实压缩操作的时候也可以针对目录写入这样的内部文件，上面没有做）。所以接下来要根据情况创建（多级）目录。
 * unzGetCurrentFileInfo64 的第三个参数是 unz_file_info64 结构，其中也有一项包含了 dosDate 信息，可以还原文件时间。
 * 对于非目录的内部文件，用 unzOpenCurrentFile，打开，然后 unzReadCurrentFile 读取文件内容，写入到真实文件中。unzReadCurrentFile 返回 0 表示文件读取结束。
 *
 */


#include "ZLibWrapLib.h"
#include "Encoding.h"
#include "Loki/ScopeGuard.h"
#include "ZLib/zip.h"
#include "ZLib/unzip.h"
#include <atlstr.h>

#define ZIP_GPBF_LANGUAGE_ENCODING_FLAG 0x800

BOOL ZipAddFile(zipFile zf, LPCTSTR lpszFileNameInZip, LPCTSTR lpszFilePath, bool bUtf8 = false)
{
    DWORD dwFileAttr = GetFileAttributes(lpszFilePath);

    if (dwFileAttr == INVALID_FILE_ATTRIBUTES) {
        return false;
    }

    DWORD dwOpenAttr = (dwFileAttr & FILE_ATTRIBUTE_DIRECTORY) != 0 ? FILE_FLAG_BACKUP_SEMANTICS : 0;
    HANDLE hFile = CreateFile(lpszFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, dwOpenAttr, NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
        return FALSE;
    }

    LOKI_ON_BLOCK_EXIT(CloseHandle, hFile);

    FILETIME ftUTC, ftLocal;

    GetFileTime(hFile, NULL, NULL, &ftUTC);
    FileTimeToLocalFileTime(&ftUTC, &ftLocal);

    WORD wDate, wTime;
    FileTimeToDosDateTime(&ftLocal, &wDate, &wTime);

    zip_fileinfo FileInfo;
    ZeroMemory(&FileInfo, sizeof(FileInfo));

    FileInfo.dosDate = ((((DWORD)wDate) << 16) | (DWORD)wTime);
    FileInfo.external_fa |= dwFileAttr;

    if (bUtf8) {
        CStringA strFileNameInZipA = UCS2ToANSI(lpszFileNameInZip, CP_UTF8);

        if (zipOpenNewFileInZip4(zf, strFileNameInZipA, &FileInfo, NULL, 0, NULL, 0, NULL, Z_DEFLATED, 9,
                                 0, -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY, NULL, 0, 0, ZIP_GPBF_LANGUAGE_ENCODING_FLAG) != ZIP_OK) {
            return FALSE;
        }
    } else {
        CStringA strFileNameInZipA = UCS2ToANSI(lpszFileNameInZip);

        // 先调用一次 zipOpenNewFileInZip ，然后开始读原始文件数据
        if (zipOpenNewFileInZip(zf, strFileNameInZipA, &FileInfo, NULL, 0, NULL, 0, NULL, Z_DEFLATED, 9) != ZIP_OK) {
            return FALSE;
        }
    }

    LOKI_ON_BLOCK_EXIT(zipCloseFileInZip, zf);

    if ((dwFileAttr & FILE_ATTRIBUTE_DIRECTORY) != 0) {
        return TRUE;
    }

    const DWORD BUFFER_SIZE = 4096;
    BYTE byBuffer[BUFFER_SIZE];

    LARGE_INTEGER li = {};

    if (!GetFileSizeEx(hFile, &li)) {
        return FALSE;
    }

    while (li.QuadPart > 0) {
        DWORD dwSizeToRead = li.QuadPart > (LONGLONG)BUFFER_SIZE ? BUFFER_SIZE : (DWORD)li.LowPart;
        DWORD dwRead = 0;

        if (!ReadFile(hFile, byBuffer, dwSizeToRead, &dwRead, NULL)) {
            return FALSE;
        }

        //使用 zipWriteInFileInZip 来写入到 ZIP 文件中去
        if (zipWriteInFileInZip(zf, byBuffer, dwRead) < 0) {
            return FALSE;
        }

        li.QuadPart -= (LONGLONG)dwRead;
    }

    return TRUE;
}

BOOL ZipAddFiles(zipFile zf, LPCTSTR lpszFileNameInZip, LPCTSTR lpszFiles, bool bUtf8 = false)
{
    WIN32_FIND_DATA wfd;
    ZeroMemory(&wfd, sizeof(wfd));

    HANDLE hFind = FindFirstFile(lpszFiles, &wfd);

    if (hFind == INVALID_HANDLE_VALUE) {
        return FALSE;
    }

    LOKI_ON_BLOCK_EXIT(FindClose, hFind);

    CString strFilePath = lpszFiles;
    int nPos = strFilePath.ReverseFind('\\');

    if (nPos != -1) {
        strFilePath = strFilePath.Left(nPos + 1);
    } else {
        strFilePath.Empty();
    }

    CString strFileNameInZip = lpszFileNameInZip;

    do {
        CString strFileName = wfd.cFileName;

        if (strFileName == _T(".") || strFileName == _T("..")) {
            continue;
        }

        if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
            if (!ZipAddFile(zf, strFileNameInZip + strFileName + _T("/"), strFilePath + strFileName, bUtf8)) {
                return FALSE;
            }

            if (!ZipAddFiles(zf, strFileNameInZip + strFileName + _T("/"), strFilePath + strFileName + _T("\\*"), bUtf8)) {
                return FALSE;
            }
        } else {
            if (!ZipAddFile(zf, strFileNameInZip + strFileName, strFilePath + strFileName, bUtf8)) {
                return FALSE;
            }
        }

    } while (FindNextFile(hFind, &wfd));

    return TRUE;
}

BOOL ZipCompress(LPCTSTR lpszSourceFiles, LPCTSTR lpszDestFile, bool bUtf8 /*= false*/)
{
    CStringA strDestFile = UCS2ToANSI(lpszDestFile);

    // 压缩先使用 zipOpen64 来打开/创建一个 ZIP 文件
    zipFile zf = zipOpen64(strDestFile, 0);

    if (zf == NULL) {
        return FALSE;
    }

    LOKI_ON_BLOCK_EXIT(zipClose, zf, (const char*)NULL);

    // 遍历要被放到压缩包中去的文件
    if (!ZipAddFiles(zf, _T(""), lpszSourceFiles, bUtf8)) {
        return FALSE;
    }

    return TRUE;
}

BOOL ZipExtractCurrentFile(unzFile uf, LPCTSTR lpszDestFolder)
{
    char szFilePathA[MAX_PATH];
    unz_file_info64 FileInfo;

    if (unzGetCurrentFileInfo64(uf, &FileInfo, szFilePathA, sizeof(szFilePathA), NULL, 0, NULL, 0) != UNZ_OK) {
        return FALSE;
    }

    if (unzOpenCurrentFile(uf) != UNZ_OK) {
        return FALSE;
    }

    LOKI_ON_BLOCK_EXIT(unzCloseCurrentFile, uf);

    CString strDestPath = lpszDestFolder;
    CString strFileName;

    if ((FileInfo.flag & ZIP_GPBF_LANGUAGE_ENCODING_FLAG) != 0) {
        strFileName = ANSIToUCS2(szFilePathA, CP_UTF8);
    } else {
        strFileName = ANSIToUCS2(szFilePathA);
    }

    int nLength = strFileName.GetLength();
    LPTSTR lpszFileName = strFileName.GetBuffer();
    LPTSTR lpszCurrentFile = lpszFileName;
    LOKI_ON_BLOCK_EXIT_OBJ(strFileName, &CString::ReleaseBuffer, -1);

    for (int i = 0; i <= nLength; ++i) {
        if (lpszFileName[i] == _T('\0')) {
            strDestPath += lpszCurrentFile;
            break;
        }

        if (lpszFileName[i] == '\\' || lpszFileName[i] == '/') {
            lpszFileName[i] = '\0';

            strDestPath += lpszCurrentFile;
            strDestPath += _T("\\");

            CreateDirectory(strDestPath, NULL);

            lpszCurrentFile = lpszFileName + i + 1;
        }
    }

    if (lpszCurrentFile[0] == _T('\0')) {
        return TRUE;
    }

    HANDLE hFile = CreateFile(strDestPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
        return FALSE;
    }

    LOKI_ON_BLOCK_EXIT(CloseHandle, hFile);

    const DWORD BUFFER_SIZE = 4096;
    BYTE byBuffer[BUFFER_SIZE];

    while (true) {
        int nSize = unzReadCurrentFile(uf, byBuffer, BUFFER_SIZE);

        if (nSize < 0) {
            return FALSE;
        } else if (nSize == 0) {
            break;
        } else {
            DWORD dwWritten = 0;

            if (!WriteFile(hFile, byBuffer, (DWORD)nSize, &dwWritten, NULL) || dwWritten != (DWORD)nSize) {
                return FALSE;
            }
        }
    }

    FILETIME ftLocal, ftUTC;

    DosDateTimeToFileTime((WORD)(FileInfo.dosDate >> 16), (WORD)FileInfo.dosDate, &ftLocal);
    LocalFileTimeToFileTime(&ftLocal, &ftUTC);
    SetFileTime(hFile, &ftUTC, &ftUTC, &ftUTC);

    return TRUE;
}

BOOL ZipExtract(LPCTSTR lpszSourceFile, LPCTSTR lpszDestFolder)
{
    CStringA strSourceFileA = UCS2ToANSI(lpszSourceFile);

    // 解压先使用 zipOpen64 来打开一个 ZIP 文件
    unzFile uf = unzOpen64(strSourceFileA);

    if (uf == NULL) {
        return FALSE;
    }

    LOKI_ON_BLOCK_EXIT(unzClose, uf);

    // 需要先使用 unzGetGlobalInfo64 来取得该文件的一些信息，来了解这个压缩包里一共包含了多少个文件，等等。
    unz_global_info64 gi;

    if (unzGetGlobalInfo64(uf, &gi) != UNZ_OK) {
        return FALSE;
    }

    CString strDestFolder = lpszDestFolder; // 释放目录

    CreateDirectory(lpszDestFolder, NULL);

    if (!strDestFolder.IsEmpty() && strDestFolder[strDestFolder.GetLength() - 1] != _T('\\')) {
        strDestFolder += _T("\\");
    }

    for (int i = 0; i < gi.number_entry; ++i) {
        if (!ZipExtractCurrentFile(uf, strDestFolder)) {
            return FALSE;
        }

        if (i < gi.number_entry - 1) {
            if (unzGoToNextFile(uf) != UNZ_OK) {
                return FALSE;
            }
        }
    }

    return TRUE;
}
