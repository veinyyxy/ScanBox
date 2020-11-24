#include "CTWAINSessionSupper.h"


CTWAINSessionSupper::CTWAINSessionSupper()
{
}

CTWAINSessionSupper::~CTWAINSessionSupper()
{
}

void CTWAINSessionSupper::OnError(const TW_STATUS & twStatus)
{
}

void CTWAINSessionSupper::OnNativeTransfer(const TW_MEMREF & pData, bool & bAbort, bool & bFree)
{
}

void CTWAINSessionSupper::OnMemoryTransfer(const TW_IMAGEINFO & twInfo, const TW_IMAGEMEMXFER & twData, const TW_UINT16 & twRC, bool & bAbort)
{
}

void CTWAINSessionSupper::OnFileTransfer(const TW_SETUPFILEXFER & twFileXfer, bool & bAbort)
{
}

void CTWAINSessionSupper::OnFileMemTransfer(const TW_IMAGEMEMXFER & twData, const TW_UINT16 & twRC, bool & bAbort)
{
}
