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
#include "nti_cmn.h"		//nti_new
#include "nti_arx.h" /*nti_read*/
#include "DbTableTemplate.h"
#include "ArxDbgUtils.h" /*selectEntity*/
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
//
//////////////////////////////////////////////////////////////////////////////

void printAll();

void makeABlock();

void createPolyline();

void addBlockWithAttributes();

Acad::ErrorStatus addToModelSpace(AcDbObjectId&,
    AcDbEntity*);

void defineBlockWithAttributes(AcDbObjectId&,
    const AcGePoint3d&, double, double);
// THE FOLLOWING CODE APPEARS IN THE SDK DOCUMENT.

void makeABlock()
{
     // Create and name a new block table record.
     //
     AcDbBlockTableRecord *pBlockTableRec
         = new AcDbBlockTableRecord();
     pBlockTableRec->setName(_T("ASDK-NO-ATTR"));
     // Get the block table.
     //
     AcDbBlockTable *pBlockTable = NULL;
     acdbHostApplicationServices()->workingDatabase()
        ->getSymbolTable(pBlockTable, AcDb::kForWrite);
     // Add the new block table record to the block table.
     //
     AcDbObjectId blockTableRecordId;
     pBlockTable->add(blockTableRecordId, pBlockTableRec);
     pBlockTable->close();
     // Create and add a line entity to the component's
     // block record.
     //
     AcDbLine *pLine = new AcDbLine();
     AcDbObjectId lineId;
     pLine->setStartPoint(AcGePoint3d(3, 3, 0));
     pLine->setEndPoint(AcGePoint3d(6, 6, 0));
     pLine->setColorIndex(3);
     pBlockTableRec->appendAcDbEntity(lineId, pLine);
     pLine->close();
     pBlockTableRec->close();
}

void defineBlockWithAttributes(
    AcDbObjectId& blockId, // This is a returned value.
    const AcGePoint3d& basePoint,
    double textHeight,
    double textAngle)
{
    AcDbBlockTable *pBlockTable = NULL;
    AcDbBlockTableRecord* pBlockRecord
       = new AcDbBlockTableRecord;
    AcDbObjectId entityId;
    // Step 1: Set the block name and base point of the block definition
    //
    pBlockRecord->setName(_T("ASDK-BLOCK-WITH-ATTR"));
    pBlockRecord->setOrigin(basePoint);
    // Open the block table for write.
    //
    acdbHostApplicationServices()->workingDatabase()
        ->getSymbolTable(pBlockTable, AcDb::kForWrite);
    // Step 2: Add the block table record to block table.
    //
    if (pBlockTable->add(blockId, pBlockRecord) != Acad::eOk)
    {
        delete pBlockRecord;
        pBlockTable->close();
        return;
    }
    // Step 3: Create a circle entity.
    //
    AcDbCircle *pCircle = new AcDbCircle;
    pCircle->setCenter(basePoint);
    pCircle->setRadius(textHeight * 4.0);
    pCircle->setColorIndex(3);
    // Append the circle entity to the block record.
    //
    pBlockRecord->appendAcDbEntity(entityId, pCircle);
    pCircle->close();
    // Step 4: Create an attribute definition entity.
    //
    AcDbAttributeDefinition *pAttdef
        = new AcDbAttributeDefinition;
    // Set the attribute definition values.
    //
    pAttdef->setPosition(basePoint);
    pAttdef->setHeight(textHeight);
    pAttdef->setRotation(textAngle);
    // For horizontal modes other than AcDb::kTextLeft
    // and vertical modes other than AcDb::kTextBase,
    // you may need to call setAlignmentPoint(). See the
    // AcDbText::setAlignmentPoint() documentation for details.
    pAttdef->setHorizontalMode(AcDb::kTextLeft);
    pAttdef->setVerticalMode(AcDb::kTextBase);
    pAttdef->setPrompt(_T("Prompt"));
    pAttdef->setTextString(_T("DEFAULT"));
    pAttdef->setTag(_T("Tag"));
    pAttdef->setInvisible(Adesk::kFalse);
    pAttdef->setVerifiable(Adesk::kFalse);
    pAttdef->setPreset(Adesk::kFalse);
    pAttdef->setConstant(Adesk::kFalse);
    pAttdef->setFieldLength(25);
    // Append the attribute definition to the block.
    //
    pBlockRecord->appendAcDbEntity(entityId, pAttdef);
    // The second attribute definition is a little easier
    // because we are cloning the first one.
    //
    AcDbAttributeDefinition *pAttdef2
        = AcDbAttributeDefinition::cast(pAttdef->clone());
    // Set the values which are specific to the
    // second attribute definition.
    //
    AcGePoint3d tempPt(basePoint);
    tempPt.y -= pAttdef2->height();
    pAttdef2->setPosition(tempPt);
    pAttdef2->setColorIndex(1); // Red
    pAttdef2->setConstant(Adesk::kTrue);
    // Append the second attribute definition to the block.
    //
    pBlockRecord->appendAcDbEntity(entityId, pAttdef2);
    pAttdef->close();
    pAttdef2->close();
    pBlockRecord->close();
    pBlockTable->close();
    return;
}

void addBlockWithAttributes()
{
    // Get an insertion point for the block reference,
    // definition, and attribute definition.
    //
    AcGePoint3d basePoint;
    if (acedGetPoint(NULL, _T("\nEnter insertion point: "),
        asDblArray(basePoint)) != RTNORM)
        return;
    // Get the rotation angle for the attribute definition.
    //
    double textAngle;
    if (acedGetAngle(asDblArray(basePoint),
        _T("\nEnter rotation angle: "), &textAngle) != RTNORM)
        return;
    // Define the height used for the attribute definition text.
    //
    double textHeight;
    if (acedGetDist(asDblArray(basePoint),
        _T("\nEnter text height: "), &textHeight) != RTNORM)
        return;
    // Build the block definition to be inserted.
    //
    AcDbObjectId blockId;
    defineBlockWithAttributes(blockId, basePoint, textHeight, textAngle);
    if (blockId.isNull()) {
        return;
    }
    // Step 1: Allocate a block reference object.
    //
    AcDbBlockReference *pBlkRef = new AcDbBlockReference;
    // Step 2: Set up the block reference to the newly
    // created block definition.
    //
    pBlkRef->setBlockTableRecord(blockId);
    // Give it the current UCS normal.
    //
    resbuf to, from;
    from.restype = RTSHORT;
    from.resval.rint = 1; // UCS
    to.restype = RTSHORT;
    to.resval.rint = 0; // WCS
    AcGeVector3d normal(0.0, 0.0, 1.0);
    acedTrans(&(normal.x), &from, &to, Adesk::kTrue,
        &(normal.x));
    // Set the insertion point for the block reference.
    //
    pBlkRef->setPosition(basePoint);
    // Indicate the LCS 0.0 angle, not necessarily the UCS 0.0 angle.
    //
    pBlkRef->setRotation(0.0);
    pBlkRef->setNormal(normal);
    // Step 3: Open the current database's model space
    // block Table Record.
    //
    AcDbBlockTable *pBlockTable;
    acdbHostApplicationServices()->workingDatabase()
        ->getSymbolTable(pBlockTable, AcDb::kForRead);
    AcDbBlockTableRecord *pBlockTableRecord;
    pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord,
        AcDb::kForWrite);
    pBlockTable->close();
    // Append the block reference to the model space
    // block Table Record.
    //
    AcDbObjectId newEntId;
    pBlockTableRecord->appendAcDbEntity(newEntId, pBlkRef);
    pBlockTableRecord->close();
    // Step 4: Open the block definition for read.
    //
    AcDbBlockTableRecord *pBlockDef;
    acdbOpenObject(pBlockDef, blockId, AcDb::kForRead);
    // Set up a block table record iterator to iterate
    // over the attribute definitions.
    //
    AcDbBlockTableRecordIterator *pIterator;
    pBlockDef->newIterator(pIterator);
    AcDbEntity *pEnt;
    AcDbAttributeDefinition *pAttdef;
    for (pIterator->start(); !pIterator->done();
        pIterator->step())
    {
        // Get the next entity.
        //
        pIterator->getEntity(pEnt, AcDb::kForRead);
        // Make sure the entity is an attribute definition
        // and not a constant.
        //
        pAttdef = AcDbAttributeDefinition::cast(pEnt);
        if (pAttdef != NULL && !pAttdef->isConstant()) {
            // We have a non-constant attribute definition,
            // so build an attribute entity.
            //
            AcDbAttribute *pAtt = new AcDbAttribute();
            pAtt->setPropertiesFrom(pAttdef);
            pAtt->setInvisible(pAttdef->isInvisible());
            // Translate the attribute by block reference.
            // To be really correct, the entire block
            // reference transform should be applied here.
            //
            basePoint = pAttdef->position();
            basePoint += pBlkRef->position().asVector();
            pAtt->setPosition(basePoint);
            pAtt->setHeight(pAttdef->height());
            pAtt->setRotation(pAttdef->rotation());
            pAtt->setTag(_T("Tag"));
            pAtt->setFieldLength(25);
            const TCHAR *pStr = pAttdef->tagConst();
            pAtt->setTag(pStr);
            pAtt->setFieldLength(pAttdef->fieldLength());
            // The database column value should be displayed.
            // INSERT prompts for this.
            //
            pAtt->setTextString(_T("Assigned Attribute Value"));
            AcDbObjectId attId;
            pBlkRef->appendAttribute(attId, pAtt);
            pAtt->close();
        }
        pEnt->close(); // use pEnt... pAttdef might be NULL
    }
    delete pIterator;
    pBlockDef->close();
    pBlkRef->close();
}

void

printAll()
{
    int rc;
    TCHAR blkName[50];
    *blkName = _T('\0');
    rc = acedGetString(Adesk::kTrue,
        _T("Enter Block Name <hit <ENTER> for current space>: "),
        blkName);
    if (rc != RTNORM)
        return;
    if (blkName[0] == _T('\0')) {
        if (acdbHostApplicationServices()->workingDatabase()->tilemode()
            == Adesk::kFalse) {
            resbuf rb;
            acedGetVar(_T("cvport"), &rb);
            if (rb.resval.rint == 1) {
                _tcscpy(blkName, ACDB_PAPER_SPACE);
            } else {
                _tcscpy(blkName, ACDB_MODEL_SPACE);
            }
        } else {
            _tcscpy(blkName, ACDB_MODEL_SPACE);
        }
    }
    AcDbBlockTable *pBlockTable;
    acdbHostApplicationServices()->workingDatabase()
        ->getSymbolTable(pBlockTable, AcDb::kForRead);
    AcDbBlockTableRecord *pBlockTableRecord;
    Acad::ErrorStatus es = pBlockTable->getAt(blkName, pBlockTableRecord,
        AcDb::kForRead);
    pBlockTable->close();
    if (es != Acad::eOk)
        return;
    AcDbBlockTableRecordIterator *pBlockIterator;
    pBlockTableRecord->newIterator(pBlockIterator);
    for (; !pBlockIterator->done();
        pBlockIterator->step())
    {
        AcDbEntity *pEntity;
        pBlockIterator->getEntity(pEntity, AcDb::kForRead);
        AcDbHandle objHandle;
        pEntity->getAcDbHandle(objHandle);
        TCHAR handleStr[20];
        objHandle.getIntoAsciiBuffer(handleStr);
        const TCHAR *pCname = pEntity->isA()->name();
        acutPrintf(_T("Object Id %lx, handle %s, class %s.\n"),
            pEntity->objectId().asOldId(), handleStr, pCname);
        pEntity->close();
    }
    delete pBlockIterator;
    pBlockTableRecord->close();
    acutPrintf(_T("\n"));
}

void

createPolyline()
{
    // Set four vertex locations for the pline.
    //
    AcGePoint3dArray ptArr;
    ptArr.setLogicalLength(4);
    for (int i = 0; i < 4; i++) {
        ptArr[i].set((double)(i/2), (double)(i%2), 0.0);
    }
    // Dynamically allocate an AcDb2dPolyline object,
    // given four vertex elements whose locations are supplied
    // in ptArr.  The polyline has no elevation, and is
    // explicitly set as closed.  The polyline is simple;
    // that is, not curve fit or a spline.  By default, the
    // widths are all 0.0 and there are no bulge factors.
    //
    AcDb2dPolyline *pNewPline = new AcDb2dPolyline(
        AcDb::k2dSimplePoly, ptArr, 0.0, Adesk::kTrue);
    pNewPline->setColorIndex(3);
    // Get a pointer to a Block Table object.
    //
    AcDbBlockTable *pBlockTable;
    acdbHostApplicationServices()->workingDatabase()
        ->getSymbolTable(pBlockTable, AcDb::kForRead);
    // Get a pointer to the MODEL_SPACE BlockTableRecord.
    //
    AcDbBlockTableRecord *pBlockTableRecord;
    pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord,
        AcDb::kForWrite);
    pBlockTable->close();
    // Append the pline object to the database and
    // obtain its Object ID.
    //
    AcDbObjectId plineObjId;
    pBlockTableRecord->appendAcDbEntity(plineObjId,
        pNewPline);
    pBlockTableRecord->close();
    // Make the pline object reside on layer "0".
    //
    pNewPline->setLayer(_T("0"));
    pNewPline->close();
}
// END CODE APPEARING IN SDK DOCUMENT.

Acad::ErrorStatus addToModelSpace(AcDbObjectId &objId, AcDbEntity* pEntity)
{
    Acad::ErrorStatus     es = Acad::eOk;
    AcDbBlockTable        *pBlockTable;
    AcDbBlockTableRecord  *pSpaceRecord;
    acdbHostApplicationServices()->workingDatabase()
        ->getSymbolTable(pBlockTable, AcDb::kForRead);
    es = pBlockTable->getAt(ACDB_MODEL_SPACE, pSpaceRecord,
        AcDb::kForWrite);
	if (!pSpaceRecord) {
		pBlockTable->close();
		return es;
	}
    pSpaceRecord->appendAcDbEntity(objId, pEntity);
    pBlockTable->close();
    pEntity->close();
    pSpaceRecord->close();
    return es;
}

/////////////////////////////////////////////////////////////////////////////////////////

// Create a table template from a table, and create a table style to hold the template.
void createTemplate()
{
	// Select a table
	ErrorStatus es;
	AcDbTable *pTbl = NULL;
	if (NULL == (pTbl = AcDbTable::cast(ArxDbgUtils::selectEntity(_T("\nSelect a table: ")))))
		acutPrintf(ACRX_T("\nSelected entity was not a table!"));
	static ACHAR sNameOfMyTableTemplate[MAX_PATH] = ACRX_T("MyTableTemplate");
	static ACHAR sNameOfMyTableStyle[MAX_PATH] = ACRX_T("MyTableStyle");
	AcDbTableTemplate* pTblTpl = new AcDbTableTemplate();
	// We skip its content here.
	es = pTblTpl->capture(pTbl, AcDb::kTableCopySkipContent);
	es = pTblTpl->setName(sNameOfMyTableTemplate);
	AcDbTableStyle* pTblSty = new AcDbTableStyle();
	AcDbDatabase *pDb = acdbHostApplicationServices()->workingDatabase();
	assert(pDb);
	// If a style with the name is already in the TableStyleDictionary, remove it.
	AcDbDictionary *pDict = NULL;
	es = acdbHostApplicationServices()->workingDatabase()->getTableStyleDictionary(pDict, AcDb::kForRead);
	// Check if the Table Style is already there.
	if (pDict->has(sNameOfMyTableStyle))
	{
		es = pDict->upgradeOpen();
		es = pDict->remove(sNameOfMyTableStyle);
	}
	// Post the table style to DB and then set the table template to it.
	AcDbObjectId idTS;
	es = pDict->upgradeOpen();
	if (Acad::eOk != (es = pDict->setAt(sNameOfMyTableStyle, pTblSty, idTS)))
	{
		pDict->close();
		delete pTblSty;
		delete pTblTpl;
		es = pTbl->close();
		acutPrintf(ACRX_T("\nUnable to add new Table Style"));
		return;
	}
	es = pDict->close();
	assert(es == Acad::eOk);
	// Set the new template to the table.
	AcDbObjectId id;
	es = pTblSty->setTemplate(pTblTpl, AcDb::kMergeCellStyleNone, id);
	if (es != Acad::eOk)
	{
		delete pTblTpl;
		pTblSty->close();
		pTbl->close();
		acutPrintf(ACRX_T("\nError in setting data table template!"));
		return;
	}
	// Clean up.
	es = pTblSty->close();
	es = pTblTpl->close();
	es = pTbl->close();
}

// Create a data link and set it to the cell(2,2) of a selected table.
void createAndSetDataLink()
{
	Acad::ErrorStatus es;
	AcDbObjectId idDL;
	AcDbDataLink *pDL = NULL;

	AcApDocument* pDoc = acDocManager->curDocument();
	if(!pDoc)
		return;

	es = acDocManager->lockDocument(pDoc, AcAp::kAutoWrite, NULL, NULL, false);
	// Select a table
	AcDbTable *pTbl = NULL;
	if (NULL == (pTbl =
		AcDbTable::cast(ArxDbgUtils::selectEntity(_T("\nSelect a table: "), AcDb::kForWrite))))
	{
		acutPrintf(ACRX_T("\nSelected entity was not a table!"));
		goto ret;
	}
	// Get an Excel file
	
	struct resbuf *result;
	int rc;
	if ((result = acutNewRb(RTSTR)) == NULL)
	{
		pTbl->close();
		acutPrintf(ACRX_T("\nUnable to allocate buffer!"));
		return;
	}
	result->resval.rstring = NULL;
	rc = acedGetFileD(ACRX_T("Excel File"),	// Title
		0/*ACRX_T("c:/temp")*/,	// Default pathname  
		ACRX_T("xls;xlsx"),	//Default extension
		16,					// Control flags
		result);	// The path selected by the user.
	if (rc != RTNORM)
	{
		pTbl->close();
		acutPrintf(ACRX_T("\nError in selecting an EXCEL file!"));
		return;
	}
	// Retrieve the file name from the ResBuf.
	ACHAR fileName[MAX_PATH] = _T("");
	//_tcscpy(fileName, result->resval.rstring);
	//rc = acutRelRb(result);
	static ACHAR sMyDataLink[MAX_PATH] = ACRX_T("MyDataLinkTest");
	// Get the Data Link Manager.
	AcDbDataLinkManager* pDlMan = acdbHostApplicationServices()->workingDatabase()->getDataLinkManager();
	assert(pDlMan);
	// Check if a Data Link with the name already exists. If so, remove it.
	if (pDlMan->getDataLink(sMyDataLink, pDL, AcDb::kForRead) == Acad::eOk && pDL)
	{
		pDL->close();
		es = pDlMan->removeDataLink(sMyDataLink, idDL);
		if (es != Acad::eOk)
		{
			pTbl->close();
			acutPrintf(ACRX_T("\nError in removing the Data Link!"));
			goto ret;
		}
	}

	// Create the Data Link with the name.
	es = pDlMan->createDataLink(ACRX_T("AcExcel"), sMyDataLink, ACRX_T("This is a test for Excel type data link."), fileName, idDL);
	if (es != Acad::eOk)
	{
		pTbl->close();
		acutPrintf(ACRX_T("\nError in creating Data Link!\nPlease check if there is a sheet named 'Sheet1' in the XLS file."));
		goto ret;
	}
	// Open the Data Link.
	es = acdbOpenObject<AcDbDataLink>(pDL, idDL, AcDb::kForWrite);
	if (es != Acad::eOk || !pDL)
	{
		pTbl->close();
		acutPrintf(ACRX_T("\nError in opening the Data Link object!"));
		goto ret;
	}
	//  Set options of the Data Link
	es = pDL->setOption(AcDb::kDataLinkOptionPersistCache);
	es = pDL->setUpdateOption(pDL->updateOption() | AcDb::kUpdateOptionAllowSourceUpdate);

	// Close the Data Link.
	es = pDL->close();
	// Set data link to the table object at cell(2,2).
	es = pTbl->setDataLink(0, 0, idDL, true);
	if (es != Acad::eOk)
	{
		pTbl->close();
		acutPrintf(ACRX_T("\nError in setting Data Link to the selected table!\nPlease check if there is a sheet named 'Sheet1' in the XLS file."));
		goto ret;
	}
	// Don't forget to close the table object.
	es = pTbl->close();
ret:
	acDocManager->unlockDocument(pDoc);
}

/////////////////////////////////////////////////////////////////////////////////////////
int nti_arx_blocks()
{
	AcDbBlockTable *pBlockTable = 0;
	AcDbBlockTableRecord *pBlkTblRcd = 0;

	acdbHostApplicationServices()->workingDatabase()->getSymbolTable(pBlockTable, AcDb::kForRead);
	pBlockTable->getAt(ACDB_MODEL_SPACE, pBlkTblRcd, AcDb::kForRead);

	AcDbBlockReferenceIdIterator * iter = 0;
	pBlkTblRcd->newBlockReferenceIdIterator(iter);

	for (; !iter->done(); iter->step()) {
		AcDbBlockReference * pBlkRef = 0;
		iter->getBlockReference(pBlkRef, AcDb::kForRead);
		AcDbHandle objHandle;
		pBlkRef->getAcDbHandle(objHandle);
		TCHAR handleStr[20];
		objHandle.getIntoAsciiBuffer(handleStr);
		const TCHAR *pCname = pBlkRef->isA()->name();
		acutPrintf(_T("name='%s', Object Id %lx, handle %s, class %s.\n"),
			  _T(""), pBlkRef->objectId().asOldId(), handleStr, pCname);

	    pBlkRef->close();
	}

	return 0;
}

int nti_insert_table()
{
	AcDbTable* pTable = new AcDbTable;
	pTable->setSize(5, 6);
	pTable->setValue(0, 0, _T("hello"));

	AcDbObjectId id;
	Acad::ErrorStatus rc = addToModelSpace(id, pTable);

	return 0;
}

int nti_arx_update_datalinks(list * links)
{
	if(!links)
		return -1;

	while (listFirst(links))
		listDelNode(links, listFirst(links));

	int i, rc;
	Acad::ErrorStatus es;

	AcDbDataLinkManager* pDlMan = acdbHostApplicationServices()->workingDatabase()->getDataLinkManager();
	assert(pDlMan);
	AcDbObjectIdArray datalinks;
	rc = pDlMan->getDataLink(datalinks);

	for (i = 0; i < datalinks.length(); ++i) {
		AcDbDataLink *pDL = NULL;
		es = acdbOpenObject<AcDbDataLink>(pDL, datalinks[i], AcDb::kForRead);
		if (!pDL)
			continue;
		
		nti_datalink * link = nti_new(nti_datalink);
		link->name = pDL->name();
		link->desc = pDL->description();
		link->conn = pDL->connectionString();

		listAddNodeTail(links, link);

		pDL->close();
	}

	return rc;
}

#ifndef NDEBUG
int test_nti_arx_main(int argc, char ** argv)
{
	printf(("%s: \n"), __FUNCTION__);
	int rc; 

	rc = nti_arx_blocks();

	rc = nti_read(_T("dwg/block.dwg"));

	return rc;
}
#endif //NDEBUG