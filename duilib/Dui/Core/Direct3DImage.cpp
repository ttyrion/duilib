#include "stdafx.h"
#include "Direct3DImage.h"
#include <Gdiplus.h>
#include <fstream>
#include "D3DTypes.h"

using namespace DirectX;

#ifndef RES_TYPE_COLOR
#define RES_TYPE_COLOR _T("*COLOR*")
#endif

extern "C"
{
    extern unsigned char *stbi_load_from_memory(unsigned char const *buffer, int len, int *x, int *y, \
        int *comp, int req_comp);
    extern void     stbi_image_free(void *retval_from_stbi_load);

};

namespace DuiLib {

    Direct3DImage::Direct3DImage() {

    }


    Direct3DImage::~Direct3DImage() {

    }

    void Direct3DImage::LoadImage(const CDuiString& file, const CDuiString& type, DWORD mask, ImageData& image) {
        LPBYTE pData = NULL;
        DWORD dwSize = 0;

        do {
            if (type.IsEmpty()) {
                CDuiString sFile = CPaintManagerUI::GetResourcePath();
                if (CPaintManagerUI::GetResourceZip().IsEmpty()) {
                    sFile += file;
                    HANDLE hFile = ::CreateFile(sFile.GetData(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, \
                        FILE_ATTRIBUTE_NORMAL, NULL);
                    if (hFile == INVALID_HANDLE_VALUE) break;
                    dwSize = ::GetFileSize(hFile, NULL);
                    if (dwSize == 0) break;

                    DWORD dwRead = 0;
                    pData = new BYTE[dwSize];
                    ::ReadFile(hFile, pData, dwSize, &dwRead, NULL);
                    ::CloseHandle(hFile);

                    if (dwRead != dwSize) {
                        delete[] pData;
                        pData = NULL;
                        break;
                    }
                }
                else {
                    //TODO: zip resource
                }
            }
            //else if (_tcscmp(type, RES_TYPE_COLOR) == 0) {
            //    pData = (PBYTE)0x1;  /* dummy pointer */
            //}
            else {
                HRSRC hResource = ::FindResource(CPaintManagerUI::GetResourceDll(), file, type);
                if (hResource == NULL) break;
                HGLOBAL hGlobal = ::LoadResource(CPaintManagerUI::GetResourceDll(), hResource);
                if (hGlobal == NULL) {
                    FreeResource(hResource);
                    break;
                }

                dwSize = ::SizeofResource(CPaintManagerUI::GetResourceDll(), hResource);
                if (dwSize == 0) break;
                pData = new BYTE[dwSize];
                ::CopyMemory(pData, (LPBYTE)::LockResource(hGlobal), dwSize);
                ::FreeResource(hResource);
            }
        } while (0);

        while (!pData) {
            //读不到图片, 则直接去读取image文件
            HANDLE hFile = ::CreateFile(file, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, \
                FILE_ATTRIBUTE_NORMAL, NULL);
            if (hFile == INVALID_HANDLE_VALUE) break;
            dwSize = ::GetFileSize(hFile, NULL);
            if (dwSize == 0) break;

            DWORD dwRead = 0;
            pData = new BYTE[dwSize];
            ::ReadFile(hFile, pData, dwSize, &dwRead, NULL);
            ::CloseHandle(hFile);

            if (dwRead != dwSize) {
                delete[] pData;
                pData = NULL;
            }
            break;
        }

        if (!pData) {
            return image.clear();
        }

        LPBYTE pImage = NULL;
        int x = 1, y = 1, n;
        if (!type.IsEmpty() || _tcscmp(type, RES_TYPE_COLOR) != 0) {
            pImage = stbi_load_from_memory(pData, dwSize, &x, &y, &n, 4);
            delete[] pData;
            //failed parsing the image data
            if (!pImage) {
                return image.clear();
            }
        }

        UINT pixel = x * y;
        BYTE* r = new BYTE[pixel]; memset(r, 0, pixel);
        BYTE* g = new BYTE[pixel]; memset(g, 0, pixel);
        BYTE* b = new BYTE[pixel]; memset(b, 0, pixel);
        BYTE* a = new BYTE[pixel]; memset(a, 0, pixel);
        for (int i = 0; i < pixel; i++) {
            a[i] = pImage[i * 4 + 3];
            if (a[i] < 255) { //alpha透明
                r[i] = (BYTE)(DWORD(pImage[i * 4]) * a[i] / 255);
                b[i] = (BYTE)(DWORD(pImage[i * 4 + 1]) * a[i] / 255);
                g[i] = (BYTE)(DWORD(pImage[i * 4 + 2]) * a[i] / 255);
                //bAlphaChannel = true;
                image.alpha_blend = true;
            }
            else {
                r[i] = pImage[i * 4];
                b[i] = pImage[i * 4 + 1];
                g[i] = pImage[i * 4 + 2];
            }

            //rgba == mask, 给不支持alpha通道的图片格式（如bmp）指定透明色
            if (*(DWORD*)(&pImage[i * 4]) == mask) {
                r[i] = 0;
                b[i] = 0;
                g[i] = 0;
                a[i] = 0;
                //bAlphaChannel = true;
                image.alpha_blend = true;
            }
        }

        image.width = x;
        image.height = y;
        image.r.append((char*)r, pixel);
        image.g.append((char*)g, pixel);
        image.b.append((char*)b, pixel);
        image.a.append((char*)a, pixel);

        SAFE_DELETE(r);
        SAFE_DELETE(g);
        SAFE_DELETE(b);
        SAFE_DELETE(a);

        if (!type.IsEmpty() || _tcscmp(type, RES_TYPE_COLOR) != 0) {
            stbi_image_free(pImage);
        }
    }
}
