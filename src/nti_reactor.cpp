/*!
 * This file is PART of nti56acad.arx project
 * @author hongjun.liao <docici@126.com>, @date 2020/09/15
 *
 * arx reactor
 * */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "StdAfx.h"
#include <axlock.h>
//
#include "AdskDMgr.h"
#include <cinttypes>
#include "nti_imgui.h" /*nti_wnddata*/
#include "nti_reactor.h" /**/
#include "nti_str.h"		//
#include "nti_cmn.h"	//nti_wnddata

extern nti_wnddata * g_wnddata;
extern CDbModReactor * gpDbReactor;

/////////////////////////////////////////////////////////////////////////////////////

class CDocInfo
{
public:
    CDocInfo();
    bool undoBtnAvailable;
    bool undoCmd;
};

CDocInfo::CDocInfo(){this->undoBtnAvailable=false; this->undoCmd=false;}

AsdkDataManager<CDocInfo> DocDataMgr; //MDI safe
#define docState DocDataMgr.docData()

//
// Update the content displayed in the modeless dialog.
//
void objDisplay(const TCHAR* pWhat, const AcDbObject* pDbObj)
{
    TCHAR hstr[20];
    TCHAR idstr[20];
    const TCHAR *pClsName = NULL;
    if(pDbObj)
    {
        AcDbObjectId id = pDbObj->objectId();
        AcDbHandle h;
        pClsName = pDbObj->isA()->name();
    
        pDbObj->getAcDbHandle(h);
        h.getIntoAsciiBuffer(hstr);
    
        acutPrintf(_T("\nDbModReactor: obj %s: ")
            _T("class %s, id %lx, handle %s.\n"),
            pWhat, pClsName , id, hstr);
    
        _stprintf_s(idstr, _countof(idstr), _T("0x%08")PRIx64, id.handle().operator Adesk::UInt64());
    }
    else
    {
        pClsName = _T("");
        _tcscpy_s(hstr, _countof(hstr), _T(""));
        _tcscpy_s(idstr, _countof(idstr), _T(""));
    }

	wcpy(g_wnddata->reactor.what, pWhat);
	wcpy(g_wnddata->reactor.cls, pClsName);
	wcpy(g_wnddata->reactor.obj_id, idstr);
	wcpy(g_wnddata->reactor.handle, hstr);

    // g_hwnd->SetDlgItemText(IDC_EDIT_CLASS, pClsName);
    // g_hwnd->SetDlgItemText(IDC_EDIT_OBJID, idstr);
    // g_hwnd->SetDlgItemText(IDC_EDIT_HANDLE, hstr);

    //enable UNDO dialog button only if UNDO not already underway and
    //either an entity is being generated or modified.
    if (pDbObj && pDbObj->isKindOf(AcDbEntity::desc()) && !docState.undoCmd) 
        docState.undoBtnAvailable = TRUE;
    
}

//
//	Attach the database reactor if one isn't attached already.  
//	Then, start up the dialog.
//
//
void attachDbReactor(AcDbDatabase* pDb)
{
	AcDbDatabase* pWkDb = acdbHostApplicationServices()->workingDatabase();
	assert(pWkDb == pDb);
	AcDbDatabase* pCurDb = curDoc()->database();
	assert(pCurDb == pDb);
	acDocManager->lockDocument(curDoc(), AcAp::kWrite);

	gpDbReactor = new CDbModReactor();
	pWkDb->addReactor(gpDbReactor);
	acutPrintf(_T("\nAttached CDbModReactor to the current database.\n"));

	acDocManager->unlockDocument(curDoc());
	acedPostCommandPrompt();
}
//
//
//	Detach the database reactor if the db is deactivated.  
//  Close the dialog if requested by the user.
//
//
void detachDbReactor(AcDbDatabase* pDb)
{
    acDocManager->lockDocument(curDoc(), AcAp::kWrite);
    pDb->removeReactor(gpDbReactor);
    acutPrintf(_T("\nDetached DbReactor from the current database.\n"));
    acDocManager->unlockDocument(curDoc());

    acedPostCommandPrompt();
}

/////////////////////////////////////////////////////////////////////////////////////

//
//	CDbModReactor overrides
//
//
//
void CDbModReactor::objectModified(const AcDbDatabase* pDb,
                                  const AcDbObject* pDbObj)
{
    AcAxDocLock docLock(pDbObj->ownerId(), AcAxDocLock::kNormal);	
    if(docLock.lockStatus() != Acad::eOk)
        return;
    else 
        objDisplay(_T("modified"), pDbObj);

}
//
//
//
void CDbModReactor::objectErased(const AcDbDatabase* pDb,
                                const AcDbObject* pDbObj,
                                Adesk::Boolean bErased)
{
    AcAxDocLock docLock(pDbObj->ownerId(), AcAxDocLock::kNormal);	
    if(docLock.lockStatus() != Acad::eOk)
        return;
    else
    {
        TCHAR buf[40];
        _stprintf_s(buf, _countof(buf), _T("%lserased"), (bErased ? _T("") : _T("not ")));
        objDisplay(buf, pDbObj);
    }
}

void CEdReactor::commandWillStart(const TCHAR *cmd)
{
        if (!_tcscmp(cmd,_T("UNDO")))
            docState.undoCmd = TRUE;
}

void CEdReactor::commandEnded(const TCHAR *cmd)
{
    if (docState.undoBtnAvailable)
    {
        //disable this button next command if no entity changes
        docState.undoBtnAvailable = FALSE; 
        
    }	
    else
    {
    }
    
    if (docState.undoCmd)
        docState.undoCmd = FALSE; //undo is finished
}
/////////////////////////////////////////////////////////////////////////////////////
//
// document manager reactor
//
/////////////////////////////////////////////////////////////////////////////////////
//
//
//
void CDocReactor::documentActivated(AcApDocument* pDoc)
{
    // It is possible to get a nullptr for pDoc if the New Tab page is
    // brought up in Acad.
    //
    if (pDoc) {
        //attachDbReactor(pDoc->database());
#ifndef NDEBUG
        acutPrintf(_T("\ndocumentActiveated: %s.\n"), pDoc->fileName());
        acedPostCommandPrompt();
#endif

        g_wnddata->reactor.curr_block = 0;
		while(listFirst(g_wnddata->reactor.block_list))
			listDelNode(g_wnddata->reactor.block_list, listFirst(g_wnddata->reactor.block_list));

		AcDbBlockTable *pBlockTable = 0;
		acdbHostApplicationServices()->workingDatabase()->getSymbolTable(pBlockTable, AcDb::kForRead);

		AcDbBlockTableIterator * iter = 0;
		pBlockTable->newIterator(iter);

		for (; iter && !iter->done(); iter->step()) {

			AcDbBlockTableRecord *pBlockTableRec = 0;
			ACHAR * name = 0;
			iter->getRecord(pBlockTableRec);
			if (pBlockTableRec) {
				pBlockTableRec->getName(name);
				AcDbObjectId id = pBlockTableRec->objectId();

				//AcDbBlockTableRecordIterator * riter = 0;
				//pBlockTableRec->newIterator(riter);

				//for (; riter && !riter->done(); riter->step()) {
				//	AcDbEntity * ent = 0;
				//	riter->getEntity(ent);

				//	AcDbAttributeDefinition *pAttDef = NULL;
				//	pAttDef = AcDbAttributeDefinition::cast(ent);
				//	if (pAttDef) {

				//	}

				//	ent->close();
				//	delete ent;
				//}
				//delete riter;

				char * bname = nti_newn(128, char);
				strncpy(bname, U8(WA(name)), 128);
				listAddNodeTail(g_wnddata->reactor.block_list, bname);

				pBlockTableRec->close();
			}
		}
		pBlockTable->close();
		delete iter;
    }
}
//
// We need to attach the database reactor to the newly created
// database. We need to delete the previous database reactor as well.
//

void CDocReactor::documentCreated(AcApDocument* pDoc)
{
#ifndef NDEBUG
    // It is possible to get a nullptr for pDoc if the New Tab page is
    // brought up in Acad.
    //
    if (pDoc) {
        acutPrintf(_T("\ndocumentCreated: %s.\n"), pDoc->fileName());
        acedPostCommandPrompt();
    }
#endif
    objDisplay(_T(""), NULL);
}

//
// This deactivation includes a non-Acad window is activated 
// so the Acad window is losing focus.
//
void CDocReactor::documentToBeDeactivated(AcApDocument* pDoc)
{
    // It is possible to get a nullptr for pDoc if the New Tab page is
    // brought up in Acad.
    //
    if (pDoc) {
#ifndef NDEBUG
        acutPrintf(_T("\ndocumentToBeDeactiveated: %s."), pDoc->fileName());
        acedPostCommandPrompt();
#endif
        detachDbReactor(pDoc->database());
    }
    objDisplay(_T(""), NULL);
}
//
// Obviously, we need to delete the database reactor associated
// with the destroyed doc.
//
void CDocReactor::documentToBeDestroyed(AcApDocument* pDoc)
{
    // It is possible to get a nullptr for pDoc if the New Tab page is
    // brought up in Acad.
    //
    if (pDoc) {
#ifndef NDEBUG
        acutPrintf(_T("\ndocumentToBeDestroyed: %s.\n"), pDoc->fileName());
        acedPostCommandPrompt();
#endif
        detachDbReactor(pDoc->database());
    }
    objDisplay(_T(""), NULL);
}
//
// This one can not be used because it gives the historically 
// to be activated document prior to the current action.
//
void CDocReactor::documentToBeActivated(AcApDocument* pDoc)
{
#ifndef NDEBUG
    // It is possible to get a nullptr for pDoc if the New Tab page is
    // brought up in Acad.
    //
    if (pDoc) {
        acutPrintf(_T("\ndocumentToBeActivated: %s."), pDoc->fileName());
        acedPostCommandPrompt();
    }
#endif
}


/////////////////////////////////////////////////////////////////////////////////////////
#ifndef NDEBUG
#endif //NDEBUG
