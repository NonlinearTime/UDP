#include "sendedfile.h"

SendedFile *SendedFile::pInstance = NULL;

SendedFile *SendedFile::GetInstance()
{
    if (pInstance == NULL) {
        pInstance = new SendedFile();
        atexit(Release);
    }
    return pInstance;
}

void SendedFile::Release()
{
    if(pInstance) {
        delete pInstance;
        pInstance = NULL;
    }
}

SendedFile::SendedFile()
{
    fileSize = 0;
}
