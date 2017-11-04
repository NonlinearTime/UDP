#include "receivedfile.h"

ReceivedFile* ReceivedFile::pInstance = NULL;

ReceivedFile *ReceivedFile::GetInstance()
{
    if(pInstance == NULL) {
        pInstance = new ReceivedFile();
        atexit(Release);
    }
    return pInstance;
}

void ReceivedFile::Release()
{
    if (pInstance) {
        delete pInstance;
        pInstance = NULL;
    }
}

quint64 ReceivedFile::getReceivedSize()
{
    return fileSize;
}

ReceivedFile::ReceivedFile()
{
    fileSize = 0;
}
