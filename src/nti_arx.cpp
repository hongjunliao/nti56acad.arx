/*!
 * This file is PART of nti56acad.arx project
 * @author hongjun.liao <docici@126.com>, @date 2020/09/11
 *
 * dock ctrl bar
 * */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "stdafx.h"
#include "nti_arx.h" /*nti_read*/

/////////////////////////////////////////////////////////////////////////////////////

int nti_read(ACHAR const * file)
{
	// Set constructor parameter to kFalse so that the
    // database will be constructed empty.  This way only
    // what is read in will be in the database.
    //
    AcDbDatabase *pDb = new AcDbDatabase(Adesk::kFalse);
    // The AcDbDatabase::readDwgFile() function
    // automatically appends a DWG extension if it is not
    // specified in the filename parameter.
    //
    if(Acad::eOk != pDb->readDwgFile(file))
		return -1;
    // Open the model space block table record.
    //
    AcDbBlockTable *pBlkTbl;
    pDb->getSymbolTable(pBlkTbl, AcDb::kForRead);
    AcDbBlockTableRecord *pBlkTblRcd;
    pBlkTbl->getAt(ACDB_MODEL_SPACE, pBlkTblRcd,
        AcDb::kForRead);
    pBlkTbl->close();
    AcDbBlockTableRecordIterator *pBlkTblRcdItr;
    pBlkTblRcd->newIterator(pBlkTblRcdItr);
    AcDbEntity *pEnt;
    for (pBlkTblRcdItr->start(); !pBlkTblRcdItr->done();
        pBlkTblRcdItr->step())
    {
        pBlkTblRcdItr->getEntity(pEnt,
            AcDb::kForRead);
        acutPrintf(_T("classname: %s\n"),  (pEnt->isA())->name());
        pEnt->close();
    }
    pBlkTblRcd->close();
    delete pBlkTblRcdItr;
    delete pDb;

	return 0;
}

void
refEditApiExample()
{
	AcDbObjectId transId;
	AcDbDatabase* pDb;
	TCHAR *fname;
	struct resbuf *rb;
	// Get a dwg file from the user.
	//
	rb = acutNewRb(RTSTR);
	int stat = acedGetFileD(_T("Pick a drawing"), NULL, _T("dwg"),
		0, rb);
	if ((stat != RTNORM) || (rb == NULL)) {
		acutPrintf(_T("\nYou must pick a drawing file."));        return;
	}
	fname = (TCHAR*)acad_malloc((_tcslen(rb->resval.rstring) + 1) *
		sizeof(TCHAR));
	_tcscpy(fname, rb->resval.rstring);
	acutRelRb(rb);
	// Open the dwg file.
	//
	pDb = new AcDbDatabase(Adesk::kFalse);
	if (pDb->readDwgFile(fname) != Acad::eOk) {
		acutPrintf(_T("\nSorry, that drawing is probably already open."));
			return;
	}
	// Get the Block Table and then the model space record.
	//
	AcDbBlockTable *pBlockTable;
	pDb->getSymbolTable(pBlockTable, AcDb::kForRead);
	AcDbBlockTableRecord *pOtherMsBtr;
	pBlockTable->getAt(ACDB_MODEL_SPACE, pOtherMsBtr,
		AcDb::kForRead);
	pBlockTable->close();
	// Create an iterator.
	//
	AcDbBlockTableRecordIterator *pIter;
	pOtherMsBtr->newIterator(pIter);
	// Set up an object ID array.
	//
	AcDbObjectIdArray objIdArray;
	// Iterate over the model space BTR. Look specifically 
	// for lines and append their object ID to the array.
	//
	for (pIter->start(); !pIter->done(); pIter->step()) {
		AcDbEntity *pEntity;
		pIter->getEntity(pEntity, AcDb::kForRead);
		// Look for only AcDbLine objects and add them to the 
		// object ID array.
		//
		if (pEntity->isKindOf(AcDbLine::desc())) {
			objIdArray.append(pEntity->objectId());
		}
		pEntity->close();
	}
	delete pIter;
	pOtherMsBtr->close();
	if (objIdArray.isEmpty()) {
		acad_free(fname);
		acutPrintf(_T("\nYou must pick a drawing file that contains lines."));
			return;
	}
	// Now get the current database and the object ID for the
	// current database's model space BTR.
	//
	AcDbBlockTable *pThisBlockTable;
	acdbHostApplicationServices()->workingDatabase()->
		getSymbolTable(pThisBlockTable, AcDb::kForRead);
	AcDbBlockTableRecord *pThisMsBtr;
	pThisBlockTable->getAt(ACDB_MODEL_SPACE, pThisMsBtr,
		AcDb::kForWrite);
	pThisBlockTable->close();
	AcDbObjectId id = pThisMsBtr->objectId();
	pThisMsBtr->close();
	// Create the long transaction. This will check all the entities 
	// out of the external database.
	//
	AcDbIdMapping errorMap;
	acapLongTransactionManagerPtr()->checkOut(transId, objIdArray,
		id, errorMap);
	// Now modify the color of these entities.
	//
	int colorIndex;
	acedGetInt(_T("\nEnter color number to change entities to: "),
		&colorIndex);
	AcDbObject* pObj;
	if (acdbOpenObject(pObj, transId, AcDb::kForRead) == Acad::eOk) {
		// Get a pointer to the transaction.
		//
		AcDbLongTransaction* pLongTrans =
			AcDbLongTransaction::cast(pObj);
		if (pLongTrans != NULL) {
			// Get a work set iterator.
			//
			AcDbLongTransWorkSetIterator* pWorkSetIter;
			pLongTrans->newWorkSetIterator(pWorkSetIter);
			// Iterate over the entities in the work set and change
			// the color.
			for (pWorkSetIter->start(); !pWorkSetIter->done();
				pWorkSetIter->step()) {
				AcDbEntity *pEntity;
				acdbOpenAcDbEntity(pEntity, pWorkSetIter->objectId(),
					AcDb::kForWrite);
				pEntity->setColorIndex(colorIndex);
				pEntity->close();
			}
			delete pWorkSetIter;
		}
		pObj->close();
	}
	// Pause to see the change.
	//
	TCHAR str[132];
	acedGetString(0, _T("\nSee the new colors. Press return to check the object into the original database"), str);
		// Check the entities back in to the original database.
		//
		acapLongTransactionManagerPtr()->checkIn(transId, errorMap);
	// Save the original database, since we have made changes.
	//
	pDb->saveAs(fname);
	// Close/Delete the database
	//
	delete pDb;
	pDb = NULL;
	acad_free(fname);
}


/////////////////////////////////////////////////////////////////////////////////////////
#ifndef NDEBUG
int test_nti_arx_main(int argc, char ** argv)
{
	int rc; 
	rc = nti_read("dwg/block.dwg");
	assert(rc == 0);
	return rc;
}

#endif //NDEBUG