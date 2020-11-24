#pragma once
#include "TWAINSession.h"
class CTWAINSessionSupper :
	public CTWAINSession
{
public:
	CTWAINSessionSupper();
	~CTWAINSessionSupper();

	// Inherited via CTWAINSession
	virtual void OnError(const TW_STATUS & twStatus) override;
	virtual void OnNativeTransfer(const TW_MEMREF & pData, bool & bAbort, bool & bFree) override;
	virtual void OnMemoryTransfer(const TW_IMAGEINFO & twInfo, const TW_IMAGEMEMXFER & twData, const TW_UINT16 & twRC, bool & bAbort) override;
	virtual void OnFileTransfer(const TW_SETUPFILEXFER & twFileXfer, bool & bAbort) override;
	virtual void OnFileMemTransfer(const TW_IMAGEMEMXFER & twData, const TW_UINT16 & twRC, bool & bAbort) override;
};

