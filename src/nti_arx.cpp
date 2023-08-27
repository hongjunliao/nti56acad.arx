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
//#include "nti_cmn.h"		//nti_new
#include "nti_arx.h" /*nti_read*/
#include "ArxDbgUtils.h" /*selectEntity*/
/////////////////////////////////////////////////////////////////////////////////////

int nti_foreach_symtbl(
	std::function<int(AcDbDatabase*& db, AcDbSymbolTable*& tbl)> on_get
	, std::function<int(AcDbSymbolTableRecord*& tblRec)> on_each)
{
	if (!(on_get && on_each)) return -1;
	Acad::ErrorStatus  es;
	AcDbDatabase* db = acdbHostApplicationServices()->workingDatabase();
	AcDbSymbolTable* symTbl;
	if (on_get(db, symTbl)!= 0) return -1;
	// get an iterator over this symbol Table
	AcDbSymbolTableIterator* tblIter;
	es = symTbl->newIterator(tblIter);
	ASSERT(es == Acad::eOk);
	if (symTbl->isKindOf(AcDbLayerTable::desc()))
		static_cast<AcDbLayerTableIterator*>(tblIter)->setSkipHidden(false);
	ASSERT(tblIter != NULL);
	if (es != Acad::eOk) {
		ArxDbgUtils::rxErrorMsg(es);
		return -1;
	}
	// don't sort AcDbViewportTable names, so we know which one is current
	bool sortAlpha = (symTbl->isKindOf(AcDbViewportTable::desc())) ? false : true;

	// walk table and just collect all the names of the entries
	AcDbSymbolTableRecord* tblRec;
	AcDbObjectIdArray  m_dictObjIdList;

	for (; !tblIter->done(); tblIter->step()) {
		es = tblIter->getRecord(tblRec, AcDb::kForRead);
		if (!(es == Acad::eOk && on_each(tblRec) == 0)) {
			ArxDbgUtils::rxErrorMsg(es);
			break;
		}
	}
	delete tblIter;
	symTbl->close();
	return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////

static void
addToProp(nti_prop_t& p, LPCTSTR field, LPCTSTR value, bool addQuotes = false)
{
	CStringList &m_fieldStrList = p._1;
	CStringList &m_valueStrList = p._2;

	if (field == NULL)
		m_fieldStrList.AddTail(_T(""));
	else
		m_fieldStrList.AddTail(field);

	if (value == NULL) {
		m_valueStrList.AddTail(_T("(NULL)"));    // some Acad funcs return Null strings!
	}
	else {
		if (addQuotes) {
			CString str;
			str.Format(_T("\"%s\""), value);
			m_valueStrList.AddTail(str);
		}
		else
			m_valueStrList.AddTail(value);
	}
}

/****************************************************************************
**
**  ArxDbgUiTdcRxObjectBase::addSeperatorLine
**
**  **jma
**
*************************************/

static void
addSeperatorLine(nti_prop_t& p, LPCTSTR className)
{
	CString str;

	str.Format(_T("------ %s ------"), className);
	addToProp(p, str, _T(""));
}


// helpers
static void nti_getpropGeCurve2d(const AcGeCurve2d* curve, int edgeType, nti_prop_t& p);

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop
**
**  **jma
**
*************************************/

void nti_getprop(AcDbObject* obj, nti_prop_t& p)
{
	ASSERT(obj != NULL);

	CString str, str2;

	addSeperatorLine(p, _T("AcDbObject"));

	addToProp(p, _T("Class Name"), ArxDbgUtils::objToClassStr(obj));
	addToProp(p, _T("Entity Name"), ArxDbgUtils::objToEnameStr(obj, str));
	addToProp(p, _T("Handle"), ArxDbgUtils::objToHandleStr(obj, str));
	addToProp(p, _T("Database"), ArxDbgUtils::dbToStr(obj->database(), str));

	AcDbObject* ownerObj;
	Acad::ErrorStatus es = acdbOpenObject(ownerObj, obj->ownerId(), AcDb::kForRead);
	if (es == Acad::eOk) {
		ArxDbgUtils::objToClassAndHandleStr(ownerObj, str);
		ownerObj->close();
		addToProp(p, _T("Owner"), str);
	}
	else
		addToProp(p, _T("Owner"), ArxDbgUtils::rxErrorStr(es));

	addToProp(p, _T("Merge Style"), ArxDbgUtils::mergeStyleToStr(obj->mergeStyle(), str));
	addToProp(p, _T("Has Save Version Override"), ArxDbgUtils::booleanToStr(obj->hasSaveVersionOverride(), str));

	AcDb::AcDbDwgVersion dwgVer;
	AcDb::MaintenanceReleaseVersion maintVer;
	es = obj->getObjectBirthVersion(dwgVer, maintVer);
	if (es == Acad::eOk) {
		addToProp(p, _T("Birth DWG Version"), ArxDbgUtils::dwgVersionToStr(dwgVer, str));
		addToProp(p, _T("Birth Maintenance Version"), ArxDbgUtils::intToStr(maintVer, str));
	}
	else
		addToProp(p, _T("Birth Version"), ArxDbgUtils::rxErrorStr(es));

	// now try to get more specific with classes that we know are derived from us
	AcDbDictionary* dict;
	AcDbEntity* ent;
	AcDbFilter* filter;
	AcDbGroup* group;
	AcDbIndex* index;
	AcDbLongTransaction* longTrans;
	AcDbMlineStyle* mlineStyle;
	AcDbPlaceHolder* placeHolder;
	AcDbPlotSettings* plotSettings;
	AcDbProxyObject* proxy;
	AcDbSymbolTable* symTbl;
	AcDbSymbolTableRecord* symTblRec;
	AcDbXrecord* xrec;

	if ((dict = AcDbDictionary::cast(obj)) != NULL)
		nti_getprop(dict, p);
	else if ((ent = AcDbEntity::cast(obj)) != NULL)
		nti_getprop(ent, p);
	else if ((filter = AcDbFilter::cast(obj)) != NULL)
		nti_getprop(filter, p);
	else if ((group = AcDbGroup::cast(obj)) != NULL)
		nti_getprop(group, p);
	else if ((index = AcDbIndex::cast(obj)) != NULL)
		nti_getprop(index, p);
	else if ((longTrans = AcDbLongTransaction::cast(obj)) != NULL)
		nti_getprop(longTrans, p);
	else if ((mlineStyle = AcDbMlineStyle::cast(obj)) != NULL)
		nti_getprop(mlineStyle, p);
	else if ((placeHolder = AcDbPlaceHolder::cast(obj)) != NULL)
		nti_getprop(placeHolder, p);
	else if ((plotSettings = AcDbPlotSettings::cast(obj)) != NULL)
		nti_getprop(plotSettings, p);
	else if ((proxy = AcDbProxyObject::cast(obj)) != NULL)
		nti_getprop(proxy, p);
	else if ((symTbl = AcDbSymbolTable::cast(obj)) != NULL)
		nti_getprop(symTbl, p);
	else if ((symTblRec = AcDbSymbolTableRecord::cast(obj)) != NULL)
		nti_getprop(symTblRec, p);
	else if ((xrec = AcDbXrecord::cast(obj)) != NULL)
		nti_getprop(xrec, p);
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop (DICTIONARY)
**
**  **jma
**
*************************************/

void nti_getprop(AcDbDictionary* dict, nti_prop_t& p)
{
	CString str;

	ASSERT(dict != NULL);

	addSeperatorLine(p, _T("AcDbDictionary"));

	addToProp(p, _T("Treat Elements As Hard"), ArxDbgUtils::booleanToStr(dict->isTreatElementsAsHard(), str));

	AcDbDictionaryWithDefault* dictWithDef;

	if ((dictWithDef = AcDbDictionaryWithDefault::cast(dict)) != NULL)
		nti_getprop(dictWithDef, p);
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop (ENTITY)
**
**  **jma
**
*************************************/

void nti_getprop(AcDbEntity* ent, nti_prop_t& p)
{
	ASSERT(ent != NULL);

	CString str, str2;

	addSeperatorLine(p, _T("AcDbEntity"));

	ArxDbgUtils::symbolIdToName(ent->layerId(), str);
	addToProp(p, _T("Layer"), str);

	addToProp(p, _T("Color"), ArxDbgUtils::colorToStr(ent->colorIndex(), str));

	ArxDbgUtils::symbolIdToName(ent->linetypeId(), str);
	addToProp(p, _T("Linetype"), str);

	addToProp(p, _T("Lineweight"), ArxDbgUtils::lineWeightTypeToStr(ent->lineWeight(), str));
	addToProp(p, _T("Linetype Scale"), ArxDbgUtils::doubleToStr(ent->linetypeScale(), str));

	TCHAR *tmpStr = ent->plotStyleName();
	addToProp(p, _T("Plot Style"), tmpStr);
	acutDelString(tmpStr);

	addToProp(p, _T("Visible"), ArxDbgUtils::booleanToStr((ent->visibility() == AcDb::kVisible), str));

	ArxDbgUtils::symbolIdToName(ent->blockId(), str);
	addToProp(p, _T("Block"), str);

	addToProp(p, _T("Is Planar"), ArxDbgUtils::booleanToStr(ent->isPlanar(), str));
	addToProp(p, _T("Clone For Dragging"), ArxDbgUtils::booleanToStr(ent->cloneMeForDragging(), str));

	AcDb3dSolid* solid;
	AcDbBlockBegin* blkBegin;
	AcDbBlockEnd* blkEnd;
	AcDbBlockReference* blkRef;
	AcDbBody* body;
	AcDbCurve* curve;
	AcDbDimension* dim;
	AcDbFace* face;
	AcDbFaceRecord* vface;
	AcDbFcf* fcf;
	AcDbFrame* frame;
	AcDbHatch* hatch;
	AcDbImage* image;
	AcDbMline* mline;
	AcDbMText* mtext;
	AcDbPoint* point;
	AcDbPolyFaceMesh* pface;
	AcDbPolygonMesh* mesh;
	AcDbProxyEntity* proxy;
	AcDbRegion* region;
	AcDbSequenceEnd* seqEnd;
	AcDbShape* shape;
	AcDbSolid* solid2d;
	AcDbText* text;
	AcDbTrace* trace;
	AcDbVertex* vertex;
	AcDbViewport* viewport;

	if ((solid = AcDb3dSolid::cast(ent)) != NULL)
		nti_getprop(solid, p);
	else if ((blkBegin = AcDbBlockBegin::cast(ent)) != NULL)
		nti_getprop(blkBegin, p);
	else if ((blkEnd = AcDbBlockEnd::cast(ent)) != NULL)
		nti_getprop(blkEnd, p);
	else if ((blkRef = AcDbBlockReference::cast(ent)) != NULL)
		nti_getprop(blkRef, p);
	else if ((body = AcDbBody::cast(ent)) != NULL)
		nti_getprop(body, p);
	else if ((curve = AcDbCurve::cast(ent)) != NULL)
		nti_getprop(curve, p);
	else if ((dim = AcDbDimension::cast(ent)) != NULL)
		nti_getprop(dim, p);
	else if ((face = AcDbFace::cast(ent)) != NULL)
		nti_getprop(face, p);
	else if ((vface = AcDbFaceRecord::cast(ent)) != NULL)
		nti_getprop(vface, p);
	else if ((fcf = AcDbFcf::cast(ent)) != NULL)
		nti_getprop(fcf, p);
	else if ((frame = AcDbFrame::cast(ent)) != NULL)
		nti_getprop(frame, p);
	else if ((hatch = AcDbHatch::cast(ent)) != NULL)
		nti_getprop(hatch, p);
	else if ((image = AcDbImage::cast(ent)) != NULL)
		nti_getprop(image, p);
	else if ((mline = AcDbMline::cast(ent)) != NULL)
		nti_getprop(mline, p);
	else if ((mtext = AcDbMText::cast(ent)) != NULL)
		nti_getprop(mtext, p);
	else if ((point = AcDbPoint::cast(ent)) != NULL)
		nti_getprop(point, p);
	else if ((pface = AcDbPolyFaceMesh::cast(ent)) != NULL)
		nti_getprop(pface, p);
	else if ((mesh = AcDbPolygonMesh::cast(ent)) != NULL)
		nti_getprop(mesh, p);
	else if ((proxy = AcDbProxyEntity::cast(ent)) != NULL)
		nti_getprop(proxy, p);
	else if ((region = AcDbRegion::cast(ent)) != NULL)
		nti_getprop(region, p);
	else if ((seqEnd = AcDbSequenceEnd::cast(ent)) != NULL)
		nti_getprop(seqEnd, p);
	else if ((shape = AcDbShape::cast(ent)) != NULL)
		nti_getprop(shape, p);
	else if ((solid2d = AcDbSolid::cast(ent)) != NULL)
		nti_getprop(solid2d, p);
	else if ((text = AcDbText::cast(ent)) != NULL)
		nti_getprop(text, p);
	else if ((trace = AcDbTrace::cast(ent)) != NULL)
		nti_getprop(trace, p);
	else if ((vertex = AcDbVertex::cast(ent)) != NULL)
		nti_getprop(vertex, p);
	else if ((viewport = AcDbViewport::cast(ent)) != NULL)
		nti_getprop(viewport, p);
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop (FILTER)
**
**  **jma
**
*************************************/

void nti_getprop(AcDbFilter* filter, nti_prop_t& p)
{
	CString str;

	ASSERT(filter != NULL);

	addSeperatorLine(p, _T("AcDbFilter"));

	AcDbLayerFilter* layFilter;
	AcDbSpatialFilter* spatialFilter;

	if ((layFilter = AcDbLayerFilter::cast(filter)) != NULL)
		nti_getprop(layFilter, p);
	else if ((spatialFilter = AcDbSpatialFilter::cast(filter)) != NULL)
		nti_getprop(spatialFilter, p);
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop    (GROUP)
**
**  **jma
**
*************************************/

void nti_getprop(AcDbGroup* group, nti_prop_t& p)
{
	CString str;

	ASSERT(group != NULL);

	addSeperatorLine(p, _T("AcDbGroup"));
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop    (INDEX)
**
**  **jma
**
*************************************/

void nti_getprop(AcDbIndex* index, nti_prop_t& p)
{
	CString str;

	ASSERT(index != NULL);

	addSeperatorLine(p, _T("AcDbIndex"));

	// TBD: doesn't link for some reason
		//AcDbLayerIndex* layIndex;
		//AcDbSpatialIndex* spatialIndex;

		//if ((layIndex = AcDbLayerIndex::cast(index)) != NULL)
		//    nti_getprop(layIndex, p);
		//else if ((spatialIndex = AcDbSpatialIndex::cast(index)) != NULL)
		//    nti_getprop(spatialIndex, p);
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop    LONG TRANSACTION
**
**  **jma
**
*************************************/

void nti_getprop(AcDbLongTransaction* lt, nti_prop_t& p)
{
	CString str;

	ASSERT(lt != NULL);

	addSeperatorLine(p, _T("AcDbLongTransaction"));

	TCHAR* tmpName = NULL;
	Acad::ErrorStatus es = lt->getLongTransactionName(tmpName);
	if (es == Acad::eOk) {
		str = tmpName;
		acutDelString(tmpName);
	}
	else
		str = ArxDbgUtils::rxErrorStr(es);

	addToProp(p, _T("Name"), str);
	addToProp(p, _T("Type"), ArxDbgUtils::longTransactionTypeToStr(lt->type(), str));

	ArxDbgUtils::symbolIdToName(lt->originBlock(), str);
	addToProp(p, _T("Origin Block"), str);

	ArxDbgUtils::symbolIdToName(lt->destinationBlock(), str);
	addToProp(p, _T("Destination Block"), str);
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop	(MLINE STYLE)
**
**  **jma
**
*************************************/

void nti_getprop(AcDbMlineStyle* mlStyle, nti_prop_t& p)
{
	CString str;

	ASSERT(mlStyle != NULL);

	addSeperatorLine(p, _T("AcDbMlineStyle"));
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop	(PLACE HOLDER)
**
**  **jma
**
*************************************/

void nti_getprop(AcDbPlaceHolder* placeHolder, nti_prop_t& p)
{
	CString str;

	ASSERT(placeHolder != NULL);

	addSeperatorLine(p, _T("AcDbPlaceHolder"));
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop	(PLOT SETTINGS)
**
**  **jma
**
*************************************/

void nti_getprop(AcDbPlotSettings* plotSettings, nti_prop_t& p)
{
	CString str;

	ASSERT(plotSettings != NULL);

	addSeperatorLine(p, _T("AcDbPlotSettings"));

	AcDbLayout* layout;
	if ((layout = AcDbLayout::cast(plotSettings)) != NULL)
		nti_getprop(layout, p);
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop    PROXY
**
**  **jma
**
*************************************/

void nti_getprop(AcDbProxyObject* proxy, nti_prop_t& p)
{
	CString str;

	ASSERT(proxy != NULL);

	addSeperatorLine(p, _T("AcDbProxyObject"));

	addToProp(p, _T("Original Class Name"), proxy->originalClassName());
	addToProp(p, _T("Original DXF Name"), proxy->originalDxfName());
	addToProp(p, _T("Application Description"), proxy->applicationDescription());
	addToProp(p, _T("Proxy Flags"), ArxDbgUtils::intToStr(proxy->proxyFlags(), str));
	int flags = proxy->proxyFlags();
	if (flags & AcDbProxyObject::kEraseAllowed)
		addToProp(p, NULL, _T("Erase Allowed"));
	if (flags & AcDbProxyObject::kCloningAllowed)
		addToProp(p, NULL, _T("Cloning Allowed"));
	if (flags & AcDbProxyObject::kMergeIgnore)
		addToProp(p, NULL, _T("Merge Ignore"));
	if (flags & AcDbProxyObject::kMergeReplace)
		addToProp(p, NULL, _T("Merge Replace"));
	if (flags & AcDbProxyObject::kMergeMangleName)
		addToProp(p, NULL, _T("Merge Mangle Name"));

	AcDbObjectIdArray refIds;
	AcDbIntArray intIds;
	Acad::ErrorStatus es = proxy->getReferences(refIds, intIds);
	if ((es == Acad::eOk) && (refIds.isEmpty() == false)) {
		addSeperatorLine(p, _T("References"));
		int len = refIds.length();
		AcDbObject* tmpObj;
		for (int i = 0; i < len; i++) {
			if (refIds[i].isNull())
				addToProp(p, NULL, _T("AcDbObjectId::kNull"));
			else {
				es = acdbOpenObject(tmpObj, refIds[i], AcDb::kForRead);
				if (es == Acad::eOk) {
					ArxDbgUtils::objToClassAndHandleStr(tmpObj, str);
					addToProp(p, NULL, str);
					tmpObj->close();
				}
				else
					addToProp(p, NULL, ArxDbgUtils::rxErrorStr(es));
			}
		}
	}
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop	(SYMBOL TABLE)
**
**  **jma
**
*************************************/

void nti_getprop(AcDbSymbolTable* symTbl, nti_prop_t& p)
{
	CString str;

	ASSERT(symTbl != NULL);

	addSeperatorLine(p, _T("AcDbSymbolTable"));
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop	(SYMBOL TABLE RECORD)
**
**  **jma
**
*************************************/

void nti_getprop(AcDbSymbolTableRecord* tblRec, nti_prop_t& p)
{
	ASSERT(tblRec != NULL);

	CString str;

	addSeperatorLine(p, _T("AcDbSymbolTableRecord"));

	// add entries common to all symbol table records
	addToProp(p, _T("Is Dependent"), ArxDbgUtils::booleanToStr(tblRec->isDependent(), str));
	addToProp(p, _T("Is Resolved"), ArxDbgUtils::booleanToStr(tblRec->isResolved(), str));

	// dispatch the appropriate nti_getprop routine (wouldn't a virtual be nice!)
	AcDbAbstractViewTableRecord* abstractViewRecord;
	AcDbBlockTableRecord*  blkRecord;
	AcDbDimStyleTableRecord*  dimStyleRecord;
	AcDbLayerTableRecord*  layerRecord;
	AcDbLinetypeTableRecord*  linetypeRecord;
	AcDbRegAppTableRecord*  regAppRecord;
	AcDbTextStyleTableRecord*  textStyleRecord;
	AcDbUCSTableRecord*  ucsRecord;

	if ((abstractViewRecord = AcDbAbstractViewTableRecord::cast(tblRec)) != NULL)
		nti_getprop(abstractViewRecord, p);
	else if ((blkRecord = AcDbBlockTableRecord::cast(tblRec)) != NULL)
		nti_getprop(blkRecord, p);
	else if ((dimStyleRecord = AcDbDimStyleTableRecord::cast(tblRec)) != NULL)
		nti_getprop(dimStyleRecord, p);
	else if ((layerRecord = AcDbLayerTableRecord::cast(tblRec)) != NULL)
		nti_getprop(layerRecord, p);
	else if ((linetypeRecord = AcDbLinetypeTableRecord::cast(tblRec)) != NULL)
		nti_getprop(linetypeRecord, p);
	else if ((regAppRecord = AcDbRegAppTableRecord::cast(tblRec)) != NULL)
		nti_getprop(regAppRecord, p);
	else if ((textStyleRecord = AcDbTextStyleTableRecord::cast(tblRec)) != NULL)
		nti_getprop(textStyleRecord, p);
	else if ((ucsRecord = AcDbUCSTableRecord::cast(tblRec)) != NULL)
		nti_getprop(ucsRecord, p);
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop	(XRECORD)
**
**  **jma
**
*************************************/

void nti_getprop(AcDbXrecord* xrec, nti_prop_t& p)
{
	CString str;

	ASSERT(xrec != NULL);

	addSeperatorLine(p, _T("AcDbXrecord"));
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop	(DICTIONARY WITH DEFAULT)
**
**  **jma
**
*************************************/

void nti_getprop(AcDbDictionaryWithDefault* dict, nti_prop_t& p)
{
	CString str;

	ASSERT(dict != NULL);

	addSeperatorLine(p, _T("AcDbDictionaryWithDefault"));

	AcDbObject* tmpObj;
	Acad::ErrorStatus es = acdbOpenObject(tmpObj, dict->defaultId(), AcDb::kForRead);
	if (es == Acad::eOk) {
		ArxDbgUtils::objToClassAndHandleStr(tmpObj, str);
		addToProp(p, _T("Default Object"), str);
		tmpObj->close();
	}
	else
		addToProp(p, _T("Default Object"), ArxDbgUtils::rxErrorStr(es));
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop    3DSOLID
**
**  **jma
**
*************************************/

void nti_getprop(AcDb3dSolid* solid, nti_prop_t& p)
{
	CString str;
	double area, volume;
	AcGePoint3d centroid;
	double momInertia[3];
	double prodInertia[3];
	double prinMoments[3];
	AcGeVector3d prinAxes[3];
	double radiiGyration[3];
	AcDbExtents extents;
	Acad::ErrorStatus es;

	ASSERT(solid != NULL);

	addSeperatorLine(p, _T("AcDb3dSolid"));

	es = solid->getArea(area);
	if (es == Acad::eOk)
		addToProp(p, _T("Area"), ArxDbgUtils::doubleToStr(area, str));

	es = solid->getMassProp(volume, centroid, momInertia,
		prodInertia, prinMoments,
		prinAxes, radiiGyration, extents);
	if (es == Acad::eOk) {
		addToProp(p, _T("Volume"), ArxDbgUtils::doubleToStr(volume, str));
		addToProp(p, _T("Centroid"), ArxDbgUtils::ptToStr(centroid, str));
		addToProp(p, _T("Mom. Of Inertia"), ArxDbgUtils::doubleArray3dToStr(momInertia, str));
		addToProp(p, _T("Prod. Of Inertia"), ArxDbgUtils::doubleArray3dToStr(prodInertia, str));
		addToProp(p, _T("Priciple Moments"), ArxDbgUtils::doubleArray3dToStr(prinMoments, str));
		addToProp(p, _T("Radii Gyration"), ArxDbgUtils::doubleArray3dToStr(radiiGyration, str));
		addToProp(p, _T("Principle Axes:"), _T(""));
		addToProp(p, _T("   X Axis"), ArxDbgUtils::vectorToStr(prinAxes[0], str));
		addToProp(p, _T("   Y Axis"), ArxDbgUtils::vectorToStr(prinAxes[1], str));
		addToProp(p, _T("   Z Axis"), ArxDbgUtils::vectorToStr(prinAxes[2], str));
		addToProp(p, _T("Extmin"), ArxDbgUtils::ptToStr(extents.minPoint(), str));
		addToProp(p, _T("Extmax"), ArxDbgUtils::ptToStr(extents.maxPoint(), str));
	}
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop	(BLOCK BEGIN)
**
**  **jma
**
*************************************/

void nti_getprop(AcDbBlockBegin* blkBegin, nti_prop_t& p)
{
	CString str;

	ASSERT(blkBegin != NULL);

	addSeperatorLine(p, _T("AcDbBlockBegin"));
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop	(BLOCK END)
**
**  **jma
**
*************************************/

void nti_getprop(AcDbBlockEnd* blkEnd, nti_prop_t& p)
{
	CString str;

	ASSERT(blkEnd != NULL);

	addSeperatorLine(p, _T("AcDbBlockEnd"));
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop    BLOCK REFERENCE
**
**  **jma
**
*************************************/

void nti_getprop(AcDbBlockReference* blockRef, nti_prop_t& p)
{
	CString str;

	ASSERT(blockRef != NULL);

	addSeperatorLine(p, _T("AcDbBlockReference"));

	ArxDbgUtils::symbolIdToName(blockRef->blockTableRecord(), str);
	addToProp(p, _T("Block Name"), str);
	addToProp(p, _T("Position"), ArxDbgUtils::ptToStr(blockRef->position(), str));
	AcGeScale3d scales = blockRef->scaleFactors();
	addToProp(p, _T("X Scale"), ArxDbgUtils::doubleToStr(scales.sx, str));
	addToProp(p, _T("Y Scale"), ArxDbgUtils::doubleToStr(scales.sy, str));
	addToProp(p, _T("Z Scale"), ArxDbgUtils::doubleToStr(scales.sz, str));
	addToProp(p, _T("Rotation"), ArxDbgUtils::angleToStr(blockRef->rotation(), str));
	addToProp(p, _T("Normal"), ArxDbgUtils::vectorToStr(blockRef->normal(), str));
	addToProp(p, _T("Treat As BlockRef For Explode"), ArxDbgUtils::booleanToStr(blockRef->treatAsAcDbBlockRefForExplode(), str));

	AcDbMInsertBlock* minsert;
	if ((minsert = AcDbMInsertBlock::cast(blockRef)) != NULL)
		nti_getprop(minsert, p);
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop	(BODY)
**
**  **jma
**
*************************************/

void nti_getprop(AcDbBody* body, nti_prop_t& p)
{
	CString str;

	ASSERT(body != NULL);

	addSeperatorLine(p, _T("AcDbBody"));
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop    CURVE
**
**  **jma
**
*************************************/

void nti_getprop(AcDbCurve* curve, nti_prop_t& p)
{
	ASSERT(curve != NULL);

	// print out curve generic info
	AcGePlane plane;
	AcDb::Planarity planeType;
	double area;
	Acad::ErrorStatus es;
	CString str;

	addSeperatorLine(p, _T("AcDbCurve"));

	addToProp(p, _T("Is Closed"), ArxDbgUtils::booleanToStr(curve->isClosed(), str));
	addToProp(p, _T("Is Periodic"), ArxDbgUtils::booleanToStr(curve->isPeriodic(), str));

	curve->getPlane(plane, planeType);
	if (planeType == AcDb::kNonPlanar)
		str, _T("Non-Planar");
	else if (planeType == AcDb::kPlanar)
		str, _T("Planar");
	else
		str, _T("Linear");
	addToProp(p, _T("Plane Type"), str);

	es = curve->getArea(area);
	if (es != Acad::eOk)
		addToProp(p, _T("Area"), ArxDbgUtils::doubleToStr(area, str));

	AcDb2dPolyline* pline2d;
	AcDb3dPolyline* pline3d;
	AcDbArc* arc;
	AcDbCircle* circle;
	AcDbEllipse* ellipse;
	AcDbLeader* leader;
	AcDbLine* line;
	AcDbPolyline* pline;
	AcDbRay* ray;
	AcDbSpline* spline;
	AcDbXline* xline;

	if ((pline2d = AcDb2dPolyline::cast(curve)) != NULL)
		nti_getprop(pline2d, p);
	else if ((pline3d = AcDb3dPolyline::cast(curve)) != NULL)
		nti_getprop(pline3d, p);
	else if ((arc = AcDbArc::cast(curve)) != NULL)
		nti_getprop(arc, p);
	else if ((circle = AcDbCircle::cast(curve)) != NULL)
		nti_getprop(circle, p);
	else if ((ellipse = AcDbEllipse::cast(curve)) != NULL)
		nti_getprop(ellipse, p);
	else if ((leader = AcDbLeader::cast(curve)) != NULL)
		nti_getprop(leader, p);
	else if ((line = AcDbLine::cast(curve)) != NULL)
		nti_getprop(line, p);
	else if ((pline = AcDbPolyline::cast(curve)) != NULL)
		nti_getprop(pline, p);
	else if ((ray = AcDbRay::cast(curve)) != NULL)
		nti_getprop(ray, p);
	else if ((spline = AcDbSpline::cast(curve)) != NULL)
		nti_getprop(spline, p);
	else if ((xline = AcDbXline::cast(curve)) != NULL)
		nti_getprop(xline, p);
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop    DIMENSION
**
**  **jma
**
*************************************/

void nti_getprop(AcDbDimension* dim, nti_prop_t& p)
{
	CString str;

	ASSERT(dim != NULL);

	addSeperatorLine(p, _T("AcDbDimension"));

	ArxDbgUtils::symbolIdToName(dim->dimensionStyle(), str);
	addToProp(p, _T("Dimstyle"), str);
	addToProp(p, _T("Text Position"), ArxDbgUtils::ptToStr(dim->textPosition(), str));
	addToProp(p, _T("Is Def Position"), ArxDbgUtils::booleanToStr(dim->isUsingDefaultTextPosition(), str));
	ArxDbgUtils::symbolIdToName(dim->dimBlockId(), str);
	addToProp(p, _T("Block Name"), str);
	addToProp(p, _T("Block Insert Pt"), ArxDbgUtils::ptToStr(dim->dimBlockPosition(), str));

	TCHAR* tmpStr = dim->dimensionText();
	addToProp(p, _T("Dimtext"), tmpStr, true);
	acutDelString(tmpStr);

	addToProp(p, _T("Text Attachment"), ArxDbgUtils::mtextAttachmentTypeToStr(dim->textAttachment(), str));
	addToProp(p, _T("Text Line Spacing Style"), ArxDbgUtils::lineSpacingTypeToStr(dim->textLineSpacingStyle(), str));
	addToProp(p, _T("Text Line Spacing Factor"), ArxDbgUtils::doubleToStr(dim->textLineSpacingFactor(), str));

	addToProp(p, _T("Text Rotation"), ArxDbgUtils::angleToStr(dim->textRotation(), str));
	addToProp(p, _T("Horiz Rotation"), ArxDbgUtils::angleToStr(dim->horizontalRotation(), str));
	addToProp(p, _T("Elevation"), ArxDbgUtils::doubleToStr(dim->elevation(), str));
	addToProp(p, _T("Normal"), ArxDbgUtils::vectorToStr(dim->normal(), str));

	// dipatch appropriate nti_getprop routine
	AcDb2LineAngularDimension* dim2Line;
	AcDb3PointAngularDimension* dim3Pt;
	AcDbDiametricDimension* dimDiam;
	AcDbAlignedDimension* dimAligned;    AcDbOrdinateDimension* dimOrdinate;

	AcDbRadialDimension* dimRadial;
	AcDbRotatedDimension* dimRotated;

	if ((dimAligned = AcDbAlignedDimension::cast(dim)) != NULL)
		nti_getprop(dimAligned, p);
	else if ((dim3Pt = AcDb3PointAngularDimension::cast(dim)) != NULL)
		nti_getprop(dim3Pt, p);
	else if ((dim2Line = AcDb2LineAngularDimension::cast(dim)) != NULL)
		nti_getprop(dim2Line, p);
	else if ((dimDiam = AcDbDiametricDimension::cast(dim)) != NULL)
		nti_getprop(dimDiam, p);
	else if ((dimRadial = AcDbRadialDimension::cast(dim)) != NULL)
		nti_getprop(dimRadial, p);
	else if ((dimRotated = AcDbRotatedDimension::cast(dim)) != NULL)
		nti_getprop(dimRotated, p);
	else if ((dimOrdinate = AcDbOrdinateDimension::cast(dim)) != NULL)
		nti_getprop(dimOrdinate, p);
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop    FACE
**
**  **jma
**
*************************************/

void nti_getprop(AcDbFace* face, nti_prop_t& p)
{
	CString str, str2;
	AcGePoint3d pt;
	int i;
	Adesk::Boolean visible;

	ASSERT(face != NULL);

	for (i = 0; i < 4; i++) {
		if (face->getVertexAt(i, pt) == Acad::eOk) {
			str2.Format(_T("Pt #%2d"), i + 1);
			addToProp(p, str2, ArxDbgUtils::ptToStr(pt, str));
		}
	}
	for (i = 0; i < 4; i++) {
		if (face->isEdgeVisibleAt(i, visible) == Acad::eOk) {
			str2.Format(_T("Edge #%2d Visible"), i + 1);
			addToProp(p, str2, ArxDbgUtils::booleanToStr(visible, str));
		}
	}
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop    FACE RECORD
**
**  **jma
**
*************************************/

void nti_getprop(AcDbFaceRecord* vface, nti_prop_t& p)
{
	int i;
	Acad::ErrorStatus es1, es2;
	Adesk::Int16 vtxIndex;
	Adesk::Boolean visible;
	CString str, str2, str3;

	ASSERT(vface != NULL);

	addSeperatorLine(p, _T("AcDbFaceRecord"));

	for (i = 0; i < 4; i++) {
		es1 = vface->getVertexAt(i, vtxIndex);
		es2 = vface->isEdgeVisibleAt(i, visible);
		if ((es1 == Acad::eOk) && (es2 == Acad::eOk)) {
			str.Format(_T("Vface Pt #%2d"), i + 1);
			str2.Format(_T("(index=%d, visible=%s)"), vtxIndex,
				ArxDbgUtils::booleanToStr(visible, str3));
			addToProp(p, str, str2);
		}
	}
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop    FCF
**
**  **jma
**
*************************************/

void nti_getprop(AcDbFcf* fcf, nti_prop_t& p)
{
	CString str, str2;

	ASSERT(fcf != NULL);

	addSeperatorLine(p, _T("AcDbFcf"));

	addToProp(p, _T("Location"), ArxDbgUtils::ptToStr(fcf->location(), str));
	ArxDbgUtils::symbolIdToName(fcf->dimensionStyle(), str);
	addToProp(p, _T("Dimstyle"), str);

	AcGePoint3dArray ptArray;
	fcf->getBoundingPoints(ptArray);
	int i, len;
	len = ptArray.length();
	for (i = 0; i < len; i++) {
		str2.Format(_T("Bouding Pt #%2d"), i + 1);
		addToProp(p, str2, ArxDbgUtils::ptToStr(ptArray.at(i), str));
	}

	addToProp(p, _T("Direction"), ArxDbgUtils::vectorToStr(fcf->direction(), str));
	addToProp(p, _T("Normal"), ArxDbgUtils::vectorToStr(fcf->normal(), str));

	TCHAR* tmpTxt = fcf->text();
	if (tmpTxt) {
		addToProp(p, _T("Text"), tmpTxt, true);
		acutDelString(tmpTxt);
	}
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop	(FRAME)
**
**  **jma
**
*************************************/

void nti_getprop(AcDbFrame* frame, nti_prop_t& p)
{
	CString str;

	ASSERT(frame != NULL);

	addSeperatorLine(p, _T("AcDbFrame"));

	AcDbOleFrame* oleFrame;
	AcDbOle2Frame* ole2Frame;

	if ((oleFrame = AcDbOleFrame::cast(frame)) != NULL)
		nti_getprop(oleFrame, p);
	else if ((ole2Frame = AcDbOle2Frame::cast(frame)) != NULL)
		nti_getprop(ole2Frame, p);
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop	(IMAGE)
**
**  **jma
**
*************************************/

void nti_getprop(AcDbImage* image, nti_prop_t& p)
{
	CString str;

	ASSERT(image != NULL);

	addSeperatorLine(p, _T("AcDbImage"));
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop    HATCH
**
**  **jma
**
*************************************/

void nti_getprop(AcDbHatch* hatch, nti_prop_t& p)
{
	CString str, str2;
	Adesk::Int32 loopType;
	AcGeVoidPointerArray edgePtrs;
	AcGeIntArray edgeTypes;
	Acad::ErrorStatus es;

	ASSERT(hatch != NULL);

	addSeperatorLine(p, _T("AcDbHatch"));

	// basic info
	addToProp(p, _T("Elevation"), ArxDbgUtils::doubleToStr(hatch->elevation(), str));
	addToProp(p, _T("Normal"), ArxDbgUtils::vectorToStr(hatch->normal(), str));
	addToProp(p, _T("Is Associative"), ArxDbgUtils::booleanToStr(hatch->associative(), str));

	// pattern info
	addToProp(p, _T("Hatch Pattern Type"), ArxDbgUtils::hatchTypeToStr(hatch->patternType(), str));
	addToProp(p, _T("Pattern Name"), hatch->patternName(), true);
	addToProp(p, _T("Pattern Angle"), ArxDbgUtils::angleToStr(hatch->patternAngle(), str));
	addToProp(p, _T("Pattern Space"), ArxDbgUtils::doubleToStr(hatch->patternSpace(), str));
	addToProp(p, _T("Pattern Scale"), ArxDbgUtils::doubleToStr(hatch->patternScale(), str));
	addToProp(p, _T("Is Pattern Double"), ArxDbgUtils::booleanToStr(hatch->patternDouble(), str));

	// style info
	addToProp(p, _T("Hatch Style"), ArxDbgUtils::hatchStyleToStr(hatch->hatchStyle(), str));

	// loop info
	for (int i = 0; i < hatch->numLoops(); i++) {
		str2.Format(_T("Loop #%d"), i + 1);
		addSeperatorLine(p, str2);

		es = hatch->getLoopAt(i, loopType, edgePtrs, edgeTypes);
		if (es == Acad::eOk) {
			addToProp(p, _T("Loop Type"), ArxDbgUtils::hatchLoopTypeToStr(loopType, str));

			int len = edgePtrs.length();
			for (int j = 0; j < len; j++) {
				str2.Format(_T("Edge #%d"), j + 1);
				addSeperatorLine(p, str2);
				nti_getpropGeCurve2d(static_cast<AcGeCurve2d*>(edgePtrs[j]), edgeTypes[j], p);
			}

			edgePtrs.setLogicalLength(0);
			edgeTypes.setLogicalLength(0);
		}
		else
			addToProp(p, _T("ERROR"), _T("Could not get loop"));
	}
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop    MLINE
**
**  **jma
**
*************************************/

void nti_getprop(AcDbMline* mline, nti_prop_t& p)
{
	CString str, str2;

	ASSERT(mline != NULL);

	addSeperatorLine(p, _T("AcDbMline"));

	ArxDbgUtils::mlineStyleIdToName(mline->style(), str);
	addToProp(p, _T("Style"), str, true);
	Mline::MlineJustification justif = mline->justification();
	if (justif == Mline::kTop)
		str = _T("Top");
	else if (justif == Mline::kZero)
		str = _T("Zero");
	else if (justif == Mline::kBottom)
		str = _T("Bottom");
	else {
		ASSERT(0);
	}
	addToProp(p, _T("Justification"), str);
	addToProp(p, _T("Scale"), ArxDbgUtils::doubleToStr(mline->scale(), str));
	addToProp(p, _T("Is Closed"), ArxDbgUtils::booleanToStr(mline->closedMline(), str));
	addToProp(p, _T("Suppress Start Caps"), ArxDbgUtils::booleanToStr(mline->supressStartCaps(), str));
	addToProp(p, _T("Suppress End Caps"), ArxDbgUtils::booleanToStr(mline->supressEndCaps(), str));
	int len = mline->numVertices();
	for (int i = 0; i < len; i++) {
		str2.Format(_T("Vertex #%2d"), i + 1);
		addToProp(p, str2, ArxDbgUtils::ptToStr(mline->vertexAt(i), str));
	}
	addToProp(p, _T("Normal"), ArxDbgUtils::vectorToStr(mline->normal(), str));
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop    MTEXT
**
**  **jma
**
*************************************/

void nti_getprop(AcDbMText* mtext, nti_prop_t& p)
{
	CString str, str2;

	ASSERT(mtext != NULL);

	addSeperatorLine(p, _T("AcDbMText"));

	addToProp(p, _T("Location"), ArxDbgUtils::ptToStr(mtext->location(), str));
	addToProp(p, _T("Rotation"), ArxDbgUtils::angleToStr(mtext->rotation(), str));
	addToProp(p, _T("Width"), ArxDbgUtils::doubleToStr(mtext->width(), str));
	addToProp(p, _T("Text Height"), ArxDbgUtils::doubleToStr(mtext->textHeight(), str));
	ArxDbgUtils::symbolIdToName(mtext->textStyle(), str);
	addToProp(p, _T("Text Style"), str);

	addToProp(p, _T("Attachment"), ArxDbgUtils::mtextAttachmentTypeToStr(mtext->attachment(), str));
	addToProp(p, _T("Flow Dir"), ArxDbgUtils::mtextFlowDirTypeToStr(mtext->flowDirection(), str));

	AcGePoint3dArray ptArray;
	mtext->getBoundingPoints(ptArray);
	int i, len;
	len = ptArray.length();
	for (i = 0; i < len; i++) {
		str2.Format(_T("Bounding Pt #%2d"), i + 1);
		addToProp(p, str2, ArxDbgUtils::ptToStr(ptArray.at(i), str));
	}

	addToProp(p, _T("Actual Height"), ArxDbgUtils::doubleToStr(mtext->actualHeight(), str));
	addToProp(p, _T("Actual Width"), ArxDbgUtils::doubleToStr(mtext->actualWidth(), str));
	addToProp(p, _T("Contents"), mtext->contents());
	addToProp(p, _T("Line Spacing Style"), ArxDbgUtils::lineSpacingTypeToStr(mtext->lineSpacingStyle(), str));
	addToProp(p, _T("Line Spacing Factor"), ArxDbgUtils::doubleToStr(mtext->lineSpacingFactor(), str));

	addToProp(p, _T("Direction"), ArxDbgUtils::vectorToStr(mtext->direction(), str));
	addToProp(p, _T("Normal"), ArxDbgUtils::vectorToStr(mtext->normal(), str));
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop    POINT
**
**  **jma
**
*************************************/

void nti_getprop(AcDbPoint* point, nti_prop_t& p)
{
	AcGePoint3d pt;
	CString str;

	ASSERT(point != NULL);

	addSeperatorLine(p, _T("AcDbPoint"));

	addToProp(p, _T("Position"), ArxDbgUtils::ptToStr(point->position(), str));
	addToProp(p, _T("Thickness"), ArxDbgUtils::doubleToStr(point->thickness(), str));
	addToProp(p, _T("ECS Rotation"), ArxDbgUtils::angleToStr(point->ecsRotation(), str));
	addToProp(p, _T("Normal"), ArxDbgUtils::vectorToStr(point->normal(), str));
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop    PFACE MESH
**
**  **jma
**
*************************************/

void nti_getprop(AcDbPolyFaceMesh* pface, nti_prop_t& p)
{
	CString str;

	ASSERT(pface != NULL);

	addSeperatorLine(p, _T("AcDbPolyFaceMesh"));

	addToProp(p, _T("Vertex Count"), ArxDbgUtils::intToStr(pface->numVertices(), str));
	addToProp(p, _T("Face Count"), ArxDbgUtils::intToStr(pface->numFaces(), str));
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop    POLYGON MESH
**
**  **jma
**
*************************************/

void nti_getprop(AcDbPolygonMesh* mesh, nti_prop_t& p)
{
	CString str;

	ASSERT(mesh != NULL);

	addSeperatorLine(p, _T("AcDbPolygonMesh"));

	AcDb::PolyMeshType pmeshType = mesh->polyMeshType();
	if (pmeshType == AcDb::kSimpleMesh)
		str = _T("Simple Mesh");
	else if (pmeshType == AcDb::kQuadSurfaceMesh)
		str = _T("Quad Surface Mesh");
	else if (pmeshType == AcDb::kCubicSurfaceMesh)
		str = _T("Cubic Surface Mesh");
	else if (pmeshType == AcDb::kBezierSurfaceMesh)
		str = _T("Bezier Surface Mesh");
	else {
		ASSERT(0);
		str = _T("*Unknown*");
	}

	addToProp(p, _T("Pmesh Type"), str);
	addToProp(p, _T("M Size"), ArxDbgUtils::intToStr(mesh->mSize(), str));
	addToProp(p, _T("N Size"), ArxDbgUtils::intToStr(mesh->nSize(), str));
	addToProp(p, _T("M Is Closed"), ArxDbgUtils::booleanToStr(mesh->isMClosed(), str));
	addToProp(p, _T("N Is Closed"), ArxDbgUtils::booleanToStr(mesh->isNClosed(), str));
	addToProp(p, _T("M Surface Density"), ArxDbgUtils::intToStr(mesh->mSurfaceDensity(), str));
	addToProp(p, _T("N Surface Density"), ArxDbgUtils::intToStr(mesh->nSurfaceDensity(), str));
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop    PROXY
**
**  **jma
**
*************************************/

void nti_getprop(AcDbProxyEntity* proxy, nti_prop_t& p)
{
	CString str;

	ASSERT(proxy != NULL);

	addSeperatorLine(p, _T("AcDbProxyEntity"));

	addToProp(p, _T("Original Class Name"), proxy->originalClassName());
	addToProp(p, _T("Original DXF Name"), proxy->originalDxfName());
	addToProp(p, _T("Application Description"), proxy->applicationDescription());
	addToProp(p, _T("Has Graphics Metafile"), ArxDbgUtils::proxyMetafileTypeToStr(proxy->graphicsMetafileType(), str));
	addToProp(p, _T("Proxy Flags"), ArxDbgUtils::intToStr(proxy->proxyFlags(), str));
	int flags = proxy->proxyFlags();
	if (flags & AcDbProxyEntity::kEraseAllowed)
		addToProp(p, _T(""), _T("Erase Allowed"));
	if (flags & AcDbProxyEntity::kTransformAllowed)
		addToProp(p, _T(""), _T("Transform Allowed"));
	if (flags & AcDbProxyEntity::kColorChangeAllowed)
		addToProp(p, _T(""), _T("Color Change Allowed"));
	if (flags & AcDbProxyEntity::kLayerChangeAllowed)
		addToProp(p, _T(""), _T("Layer Change Allowed"));
	if (flags & AcDbProxyEntity::kLinetypeChangeAllowed)
		addToProp(p, _T(""), _T("Linetype Change Allowed"));
	if (flags & AcDbProxyEntity::kLinetypeScaleChangeAllowed)
		addToProp(p, _T(""), _T("Linetype Scale Change Allowed"));
	if (flags & AcDbProxyEntity::kLineWeightChangeAllowed)
		addToProp(p, _T(""), _T("Line Weight Change Allowed"));
	if (flags & AcDbProxyEntity::kPlotStyleNameChangeAllowed)
		addToProp(p, _T(""), _T("Plot Style Name Change Allowed"));
	if (flags & AcDbProxyEntity::kVisibilityChangeAllowed)
		addToProp(p, _T(""), _T("Visibility Change Allowed"));
	if (flags & AcDbProxyEntity::kCloningAllowed)
		addToProp(p, _T(""), _T("Cloning Allowed"));

	AcDbObjectIdArray refIds;
	AcDbIntArray intIds;
	Acad::ErrorStatus es = proxy->getReferences(refIds, intIds);
	if ((es == Acad::eOk) && (refIds.isEmpty() == false)) {
		addSeperatorLine(p, _T("References"));
		int len = refIds.length();
		AcDbObject* tmpObj;
		for (int i = 0; i < len; i++) {
			if (refIds[i].isNull())
				addToProp(p, _T(""), _T("AcDbObjectId::kNull"));
			else {
				es = acdbOpenObject(tmpObj, refIds[i], AcDb::kForRead);
				if (es == Acad::eOk) {
					ArxDbgUtils::objToClassAndHandleStr(tmpObj, str);
					addToProp(p, _T(""), str);
					tmpObj->close();
				}
				else
					addToProp(p, _T(""), ArxDbgUtils::rxErrorStr(es));
			}
		}
	}
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop    REGION
**
**  **jma
**
*************************************/

void nti_getprop(AcDbRegion* pRegion, nti_prop_t& p)
{
	CString str;
	double perimeter, area;
	Acad::ErrorStatus es;
	AcGePoint3d origin;
	AcGeVector3d xAxis, yAxis;
	AcGePoint2d centroid;
	double momInertia[2], prodInertia, prinMoments[2], radiiGyration[2];
	AcGeVector2d prinAxes[2];
	AcGePoint2d extentsLow, extentsHigh;

	ASSERT(pRegion != NULL);

	addSeperatorLine(p, _T("AcDbRegion"));

	es = pRegion->getPerimeter(perimeter);
	if (es == Acad::eOk)
		addToProp(p, _T("Perimeter"), ArxDbgUtils::doubleToStr(perimeter, str));

	es = pRegion->getArea(area);
	if (es == Acad::eOk)
		addToProp(p, _T("Area"), ArxDbgUtils::doubleToStr(area, str));

	// the following 3 lines of code are added to
	// get the proper info (for getAreaProp()) from the plane it is in
	// see solution #44949 for more explanation
	AcGePlane plane;
	pRegion->getPlane(plane);
	plane.getCoordSystem(origin, xAxis, yAxis);

	es = pRegion->getAreaProp(origin, xAxis, yAxis, perimeter, area, centroid, momInertia,
		prodInertia, prinMoments,
		prinAxes, radiiGyration, extentsLow, extentsHigh);
	if (es == Acad::eOk) {
		addToProp(p, _T("Origin"), ArxDbgUtils::ptToStr(origin, str));
		addToProp(p, _T("X Axis"), ArxDbgUtils::vectorToStr(xAxis, str));
		addToProp(p, _T("Y Axis"), ArxDbgUtils::vectorToStr(yAxis, str));
		addToProp(p, _T("Perimeter"), ArxDbgUtils::doubleToStr(perimeter, str));
		addToProp(p, _T("Area"), ArxDbgUtils::doubleToStr(area, str));
		addToProp(p, _T("Centroid"), ArxDbgUtils::ptToStr(centroid, str));
		addToProp(p, _T("Mom. Of Inertia"), ArxDbgUtils::doubleArray2dToStr(momInertia, str));
		addToProp(p, _T("Prod. Of Inertia"), ArxDbgUtils::doubleToStr(prodInertia, str));
		addToProp(p, _T("Principle Moments"), ArxDbgUtils::doubleArray2dToStr(prinMoments, str));
		addToProp(p, _T("Radii Gyration"), ArxDbgUtils::doubleArray2dToStr(radiiGyration, str));
		addToProp(p, _T("Principle Axes:"), _T(""));
		addToProp(p, _T("   X Axis"), ArxDbgUtils::vectorToStr(prinAxes[0], str));
		addToProp(p, _T("   Y Axis"), ArxDbgUtils::vectorToStr(prinAxes[1], str));
		addToProp(p, _T("Extents Low"), ArxDbgUtils::ptToStr(extentsLow, str));
		addToProp(p, _T("Extents High"), ArxDbgUtils::ptToStr(extentsHigh, str));
	}
	else {
		ArxDbgUtils::stopAlertBox(_T("ERROR calling AcDbRegion::getAreaProp()"));
		ArxDbgUtils::rxErrorAlert(es);
	}
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop    SEQUENCE END
**
**  **jma
**
*************************************/

void nti_getprop(AcDbSequenceEnd* seqEnd, nti_prop_t& p)
{
	ASSERT(seqEnd != NULL);

	addSeperatorLine(p, _T("AcDbSequenceEnd"));
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop    SHAPE
**
**  **jma
**
*************************************/

void nti_getprop(AcDbShape* shape, nti_prop_t& p)
{
	CString str;

	ASSERT(shape != NULL);

	addSeperatorLine(p, _T("AcDbShape"));

	addToProp(p, _T("Position"), ArxDbgUtils::ptToStr(shape->position(), str));
	addToProp(p, _T("Size"), ArxDbgUtils::doubleToStr(shape->size(), str));
	addToProp(p, _T("Name"), shape->name(), true);
	addToProp(p, _T("Rotation"), ArxDbgUtils::angleToStr(shape->rotation(), str));
	addToProp(p, _T("Width Factor"), ArxDbgUtils::doubleToStr(shape->widthFactor(), str));
	addToProp(p, _T("Oblique"), ArxDbgUtils::angleToStr(shape->oblique(), str));
	addToProp(p, _T("Thickness:"), ArxDbgUtils::doubleToStr(shape->thickness(), str));
	addToProp(p, _T("Normal"), ArxDbgUtils::vectorToStr(shape->normal(), str));
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop    SOLID
**
**  **jma
**
*************************************/

void nti_getprop(AcDbSolid* solid, nti_prop_t& p)
{
	AcGePoint3d pt;
	int i;
	CString str, str2;

	ASSERT(solid != NULL);

	addSeperatorLine(p, _T("AcDbSolid"));

	for (i = 0; i < 4; i++) {
		if (solid->getPointAt(i, pt) == Acad::eOk) {
			str2.Format(_T("Pt #%2d"), i + 1);
			addToProp(p, str2, ArxDbgUtils::ptToStr(pt, str));
		}
	}
	addToProp(p, _T("Thickness"), ArxDbgUtils::doubleToStr(solid->thickness(), str));
	addToProp(p, _T("Normal"), ArxDbgUtils::vectorToStr(solid->normal(), str));
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop    TEXT
**
**  **jma
**
*************************************/

void nti_getprop(AcDbText* text, nti_prop_t& p)
{
	CString str;
	CString tmpStr;

	ASSERT(text != NULL);

	addSeperatorLine(p, _T("AcDbText"));

	const TCHAR* tmpStr2 = text->textStringConst();
	addToProp(p, _T("Text"), tmpStr2, true);

	ArxDbgUtils::symbolIdToName(text->textStyle(), str);
	addToProp(p, _T("Text Style"), str);
	addToProp(p, _T("Position"), ArxDbgUtils::ptToStr(text->position(), str));
	addToProp(p, _T("Is Def Alignment"), ArxDbgUtils::booleanToStr(text->isDefaultAlignment(), str));
	if (text->isDefaultAlignment() == Adesk::kFalse)
		addToProp(p, _T("Alignment Pt"), ArxDbgUtils::ptToStr(text->alignmentPoint(), str));

	addToProp(p, _T("Thickness"), ArxDbgUtils::doubleToStr(text->thickness(), str));
	addToProp(p, _T("Oblique Angle"), ArxDbgUtils::angleToStr(text->oblique(), str));
	addToProp(p, _T("Rotation"), ArxDbgUtils::angleToStr(text->rotation(), str));
	addToProp(p, _T("Height"), ArxDbgUtils::doubleToStr(text->height(), str));
	addToProp(p, _T("Width Factor"), ArxDbgUtils::doubleToStr(text->widthFactor(), str));
	addToProp(p, _T("Is Mirrored In X"), ArxDbgUtils::booleanToStr(text->isMirroredInX(), str));
	addToProp(p, _T("Is Mirrored In Y"), ArxDbgUtils::booleanToStr(text->isMirroredInY(), str));
	addToProp(p, _T("Horizontal Mode"), ArxDbgUtils::textModeToStr(text->horizontalMode(), str));
	addToProp(p, _T("Vertical Mode"), ArxDbgUtils::textModeToStr(text->verticalMode(), str));
	addToProp(p, _T("Normal"), ArxDbgUtils::vectorToStr(text->normal(), str));

	AcDbAttribute* attrib;
	AcDbAttributeDefinition* attdef;
	if ((attrib = AcDbAttribute::cast(text)) != NULL)
		nti_getprop(attrib, p);
	else if ((attdef = AcDbAttributeDefinition::cast(text)) != NULL)
		nti_getprop(attdef, p);
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop    TRACE
**
**  **jma
**
*************************************/

void nti_getprop(AcDbTrace* trace, nti_prop_t& p)
{
	AcGePoint3d pt;
	int i;
	CString str, str2;

	ASSERT(trace != NULL);

	addSeperatorLine(p, _T("AcDbTrace"));

	for (i = 0; i < 4; i++) {
		if (trace->getPointAt(i, pt) == Acad::eOk) {
			str2.Format(_T("Pt #%2d"), i + 1);
			addToProp(p, str2, ArxDbgUtils::ptToStr(pt, str));
		}
	}
	addToProp(p, _T("Thickness"), ArxDbgUtils::doubleToStr(trace->thickness(), str));
	addToProp(p, _T("Normal"), ArxDbgUtils::vectorToStr(trace->normal(), str));
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop    VERTEX
**
**  **jma
**
*************************************/

void nti_getprop(AcDbVertex* vertex, nti_prop_t& p)
{
	ASSERT(vertex != NULL);

	addSeperatorLine(p, _T("AcDbVertex"));

	AcDb2dVertex* vertex2d;
	AcDb3dPolylineVertex* vertex3d;
	AcDbPolyFaceMeshVertex* pface;
	AcDbPolygonMeshVertex* mesh;

	if ((vertex2d = AcDb2dVertex::cast(vertex)) != NULL)
		nti_getprop(vertex2d, p);
	else if ((vertex3d = AcDb3dPolylineVertex::cast(vertex)) != NULL)
		nti_getprop(vertex3d, p);
	else if ((pface = AcDbPolyFaceMeshVertex::cast(vertex)) != NULL)
		nti_getprop(pface, p);
	else if ((mesh = AcDbPolygonMeshVertex::cast(vertex)) != NULL)
		nti_getprop(mesh, p);
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop    VIEWPORT
**
**  **jma
**
*************************************/

void nti_getprop(AcDbViewport* viewport, nti_prop_t& p)
{
	CString str;

	ASSERT(viewport != NULL);

	addSeperatorLine(p, _T("AcDbViewport"));

	//if (viewport->database() != acdbHostApplicationServices()->workingDatabase()) {
	//    ArxDbgUtils::stopAlertBox(_T("This AcDbViewport belongs to a database not currently loaded in the AutoCAD editor.  AutoCAD does not like this, request for info ignored to prevent crash!"));
	//    return;
	//}

	addToProp(p, _T("Height"), ArxDbgUtils::doubleToStr(viewport->height(), str));
	addToProp(p, _T("Width"), ArxDbgUtils::doubleToStr(viewport->width(), str));
	addToProp(p, _T("Center Pt"), ArxDbgUtils::ptToStr(viewport->centerPoint(), str));
	addToProp(p, _T("Number"), ArxDbgUtils::intToStr(viewport->number(), str));
	addToProp(p, _T("Is On"), ArxDbgUtils::booleanToStr(viewport->isOn(), str));
	addToProp(p, _T("View Target"), ArxDbgUtils::ptToStr(viewport->viewTarget(), str));
	addToProp(p, _T("View Dir"), ArxDbgUtils::vectorToStr(viewport->viewDirection(), str));
	addToProp(p, _T("View Height"), ArxDbgUtils::doubleToStr(viewport->viewHeight(), str));
	addToProp(p, _T("View Center"), ArxDbgUtils::ptToStr(viewport->viewCenter(), str));
	addToProp(p, _T("Twist Angle"), ArxDbgUtils::angleToStr(viewport->twistAngle(), str));
	addToProp(p, _T("Lens Length"), ArxDbgUtils::doubleToStr(viewport->lensLength(), str));
	addToProp(p, _T("Front Clip On"), ArxDbgUtils::booleanToStr(viewport->isFrontClipOn(), str));
	addToProp(p, _T("Back Clip On"), ArxDbgUtils::booleanToStr(viewport->isBackClipOn(), str));
	addToProp(p, _T("Front Clip At Eye"), ArxDbgUtils::booleanToStr(viewport->isFrontClipAtEyeOn(), str));
	addToProp(p, _T("Front Clip Dist"), ArxDbgUtils::doubleToStr(viewport->frontClipDistance(), str));
	addToProp(p, _T("Back Clip Dist"), ArxDbgUtils::doubleToStr(viewport->backClipDistance(), str));
	addToProp(p, _T("Perspective On"), ArxDbgUtils::booleanToStr(viewport->isPerspectiveOn(), str));
	addToProp(p, _T("UCS Follow On"), ArxDbgUtils::booleanToStr(viewport->isUcsFollowModeOn(), str));
	addToProp(p, _T("UCS Icon Visible"), ArxDbgUtils::booleanToStr(viewport->isUcsIconVisible(), str));
	addToProp(p, _T("UCS Icon At Origin"), ArxDbgUtils::booleanToStr(viewport->isUcsIconAtOrigin(), str));
	addToProp(p, _T("Fast Zoom On"), ArxDbgUtils::booleanToStr(viewport->isFastZoomOn(), str));
	addToProp(p, _T("Circle Sides"), ArxDbgUtils::intToStr(viewport->circleSides(), str));
	addToProp(p, _T("Snap On"), ArxDbgUtils::booleanToStr(viewport->isSnapOn(), str));
	addToProp(p, _T("Snap Isometric"), ArxDbgUtils::booleanToStr(viewport->isSnapIsometric(), str));
	addToProp(p, _T("Snap Angle"), ArxDbgUtils::angleToStr(viewport->snapAngle(), str));
	addToProp(p, _T("Snap Base Pt"), ArxDbgUtils::ptToStr(viewport->snapBasePoint(), str));
	addToProp(p, _T("Snap Increment"), ArxDbgUtils::vectorToStr(viewport->snapIncrement(), str));
	addToProp(p, _T("Snap Iso Pair"), ArxDbgUtils::intToStr(viewport->snapIsoPair(), str));
	addToProp(p, _T("Grid On"), ArxDbgUtils::booleanToStr(viewport->isGridOn(), str));
	addToProp(p, _T("Grid Increment"), ArxDbgUtils::vectorToStr(viewport->gridIncrement(), str));
	addToProp(p, _T("Hidden Lines"), ArxDbgUtils::booleanToStr(viewport->hiddenLinesRemoved(), str));
	addToProp(p, _T("Is Locked"), ArxDbgUtils::booleanToStr(viewport->isLocked(), str));
	addToProp(p, _T("Is Transparent"), ArxDbgUtils::booleanToStr(viewport->isTransparent(), str));
	addToProp(p, _T("Custom Scale"), ArxDbgUtils::doubleToStr(viewport->customScale(), str));
	addToProp(p, _T("Standard Scale Type"), ArxDbgUtils::standardScaleTypeToStr(viewport->standardScale(), str));

	const TCHAR* tmpStr = NULL;
	Acad::ErrorStatus es = viewport->plotStyleSheet(tmpStr);
	if (es == Acad::eOk)
		addToProp(p, _T("Plot Style Sheet"), tmpStr, true);
	else
		addToProp(p, _T("Plot Style Sheet"), ArxDbgUtils::rxErrorStr(es));

	es = viewport->effectivePlotStyleSheet(tmpStr);
	if (es == Acad::eOk)
		addToProp(p, _T("Effective Plot Style Sheet"), tmpStr, true);
	else
		addToProp(p, _T("Effective Plot Style Sheet"), ArxDbgUtils::rxErrorStr(es));

	addToProp(p, _T("Non-Rect Clip On"), ArxDbgUtils::booleanToStr(viewport->isNonRectClipOn(), str));

	AcDbObject* tmpObj;
	es = acdbOpenObject(tmpObj, viewport->nonRectClipEntityId(), AcDb::kForRead);
	if (es == Acad::eOk) {
		addToProp(p, _T("Non-Rect Clip Entity"), ArxDbgUtils::objToClassAndHandleStr(tmpObj, str));
		tmpObj->close();
	}
	else
		addToProp(p, _T("Non-Rect Clip Entity"), ArxDbgUtils::rxErrorStr(es));

	AcGePoint3d origin;
	AcGeVector3d xAxis, yAxis;
	es = viewport->getUcs(origin, xAxis, yAxis);
	if (es == Acad::eOk) {
		addToProp(p, _T("UCS Origin"), ArxDbgUtils::ptToStr(origin, str));
		addToProp(p, _T("UCS X Axis"), ArxDbgUtils::vectorToStr(xAxis, str));
		addToProp(p, _T("UCS Y Axis"), ArxDbgUtils::vectorToStr(yAxis, str));
	}
	else
		addToProp(p, _T("UCS Info"), ArxDbgUtils::rxErrorStr(es));

	AcDb::OrthographicView orthoView;
	bool ucsIsOrtho = viewport->isUcsOrthographic(orthoView);
	addToProp(p, _T("Is UCS Orthographic"), ArxDbgUtils::booleanToStr(ucsIsOrtho, str));
	if (ucsIsOrtho)
		addToProp(p, _T("UCS Orthographic View"), ArxDbgUtils::orthoViewToStr(orthoView, str));

	ArxDbgUtils::symbolIdToName(viewport->ucsName(), str);
	addToProp(p, _T("UCS Name"), str);
	addToProp(p, _T("Is UCS Saved With Viewport"), ArxDbgUtils::booleanToStr(viewport->isUcsSavedWithViewport(), str));
	addToProp(p, _T("Elevation"), ArxDbgUtils::doubleToStr(viewport->elevation(), str));

	bool viewIsOrtho = viewport->isViewOrthographic(orthoView);
	addToProp(p, _T("Is View Orthographic"), ArxDbgUtils::booleanToStr(viewIsOrtho, str));
	if (viewIsOrtho)
		addToProp(p, _T("Orthographic View"), ArxDbgUtils::orthoViewToStr(orthoView, str));

	AcDbObjectIdArray frozenLayers;
	es = viewport->getFrozenLayerList(frozenLayers);
	if ((es == Acad::eOk) && (frozenLayers.isEmpty() == Adesk::kFalse)) {
		addSeperatorLine(p, _T("Frozen Layers"));
		int len = frozenLayers.length();
		for (int i = 0; i < len; i++) {
			ArxDbgUtils::symbolIdToName(frozenLayers[i], str);
			addToProp(p, _T(""), str);
		}
	}
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop:    MINSERT BLOCK
**
**  **jma
**
*************************************/

void nti_getprop(AcDbMInsertBlock* mInsert, nti_prop_t& p)
{
	CString str;

	ASSERT(mInsert != NULL);

	addSeperatorLine(p, _T("AcDbMInsertBlock"));

	addToProp(p, _T("Columns"), ArxDbgUtils::intToStr(mInsert->columns(), str));
	addToProp(p, _T("Rows"), ArxDbgUtils::intToStr(mInsert->rows(), str));
	addToProp(p, _T("Column Spacing"), ArxDbgUtils::doubleToStr(mInsert->columnSpacing(), str));
	addToProp(p, _T("Row Spacing"), ArxDbgUtils::doubleToStr(mInsert->rowSpacing(), str));
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop    2D POLYLINE
**
**  **jma
**
*************************************/

void nti_getprop(AcDb2dPolyline* pline, nti_prop_t& p)
{
	CString str;

	ASSERT(pline != NULL);

	addSeperatorLine(p, _T("AcDb2dPolyline"));

	AcDb::Poly2dType ptype = pline->polyType();
	if (ptype == AcDb::k2dSimplePoly)
		str = _T("Simple");
	else if (ptype == AcDb::k2dFitCurvePoly)
		str = _T("Fit Curve");
	else if (ptype == AcDb::k2dQuadSplinePoly)
		str = _T("Quadratic Spline");
	else if (ptype == AcDb::k2dCubicSplinePoly)
		str = _T("Cubic Spline");
	else {
		ASSERT(0);
		str = _T("*Unknown*");
	}

	addToProp(p, _T("2D Pline Type"), str);
	addToProp(p, _T("Def Start Width"), ArxDbgUtils::doubleToStr(pline->defaultStartWidth(), str));
	addToProp(p, _T("Def End Width"), ArxDbgUtils::doubleToStr(pline->defaultEndWidth(), str));
	addToProp(p, _T("Linetype Gen On"), ArxDbgUtils::doubleToStr(pline->isLinetypeGenerationOn(), str));
	addToProp(p, _T("Elevation"), ArxDbgUtils::doubleToStr(pline->elevation(), str));
	addToProp(p, _T("Thickness"), ArxDbgUtils::doubleToStr(pline->thickness(), str));
	addToProp(p, _T("Normal"), ArxDbgUtils::vectorToStr(pline->normal(), str));
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop    3D POLYLINE
**
**  **jma
**
*************************************/

void nti_getprop(AcDb3dPolyline* pline, nti_prop_t& p)
{
	CString str;

	ASSERT(pline != NULL);

	addSeperatorLine(p, _T("AcDb3dPolyline"));

	AcDb::Poly3dType ptype = pline->polyType();
	if (ptype == AcDb::k3dSimplePoly)
		str = _T("Simple");
	else if (ptype == AcDb::k3dQuadSplinePoly)
		str = _T("Quadratic Spline");
	else if (ptype == AcDb::k3dCubicSplinePoly)
		str = _T("Cubic Spline");
	else {
		ASSERT(0);
		str = _T("*Unknown*");
	}

	addToProp(p, _T("3D Pline Type"), str);
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop    ARC
**
**  **jma
**
*************************************/

void nti_getprop(AcDbArc* arc, nti_prop_t& p)
{
	CString str;

	ASSERT(arc != NULL);

	addSeperatorLine(p, _T("AcDbArc"));

	addToProp(p, _T("Center Pt"), ArxDbgUtils::ptToStr(arc->center(), str));
	addToProp(p, _T("Radius"), ArxDbgUtils::doubleToStr(arc->radius(), str));
	addToProp(p, _T("Start Angle"), ArxDbgUtils::angleToStr(arc->startAngle(), str));
	addToProp(p, _T("End Angle"), ArxDbgUtils::angleToStr(arc->endAngle(), str));
	addToProp(p, _T("Thickness"), ArxDbgUtils::doubleToStr(arc->thickness(), str));
	addToProp(p, _T("Normal"), ArxDbgUtils::vectorToStr(arc->normal(), str));
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop    CIRCLE
**
**  **jma
**
*************************************/

void nti_getprop(AcDbCircle* circle, nti_prop_t& p)
{
	CString str;

	ASSERT(circle != NULL);

	addSeperatorLine(p, _T("AcDbCircle"));

	addToProp(p, _T("Center Pt"), ArxDbgUtils::ptToStr(circle->center(), str));
	addToProp(p, _T("Radius"), ArxDbgUtils::doubleToStr(circle->radius(), str));
	addToProp(p, _T("Thickness"), ArxDbgUtils::doubleToStr(circle->thickness(), str));
	addToProp(p, _T("Normal"), ArxDbgUtils::vectorToStr(circle->normal(), str));
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop    ELLIPSE
**
**  **jma
**
*************************************/

void nti_getprop(AcDbEllipse* ellipse, nti_prop_t& p)
{
	CString str;

	ASSERT(ellipse != NULL);

	addSeperatorLine(p, _T("AcDbEllipse"));

	addToProp(p, _T("Center Pt"), ArxDbgUtils::ptToStr(ellipse->center(), str));
	addToProp(p, _T("Major Axis"), ArxDbgUtils::vectorToStr(ellipse->majorAxis(), str));
	addToProp(p, _T("Minor Axis"), ArxDbgUtils::vectorToStr(ellipse->minorAxis(), str));
	addToProp(p, _T("Radius Ratio"), ArxDbgUtils::doubleToStr(ellipse->radiusRatio(), str));
	addToProp(p, _T("Start Angle"), ArxDbgUtils::angleToStr(ellipse->startAngle(), str));
	addToProp(p, _T("End Angle"), ArxDbgUtils::angleToStr(ellipse->endAngle(), str));
	addToProp(p, _T("Normal"), ArxDbgUtils::vectorToStr(ellipse->normal(), str));
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop    LEADER
**
**  **jma
**
*************************************/

void nti_getprop(AcDbLeader* leader, nti_prop_t& p)
{
	CString str, str2;

	ASSERT(leader != NULL);

	addSeperatorLine(p, _T("AcDbLeader"));

	int numVerts = leader->numVertices();
	for (int i = 0; i < numVerts; i++) {
		str2.Format(_T("Vertex #%2d"), i + 1);
		addToProp(p, str2, ArxDbgUtils::ptToStr(leader->vertexAt(i), str));
	}
	ArxDbgUtils::symbolIdToName(leader->dimensionStyle(), str);
	addToProp(p, _T("Dimstyle"), str);
	addToProp(p, _T("Has Arrow Head"), ArxDbgUtils::booleanToStr(leader->hasArrowHead(), str));
	addToProp(p, _T("Has Hook Line"), ArxDbgUtils::booleanToStr(leader->hasHookLine(), str));
	addToProp(p, _T("Is Splined"), ArxDbgUtils::booleanToStr(leader->isSplined(), str));
	addToProp(p, _T("Normal"), ArxDbgUtils::vectorToStr(leader->normal(), str));

	AcDbLeader::AnnoType type = leader->annoType();
	if (type == AcDbLeader::kMText)
		str = _T("Mtext");
	else if (type == AcDbLeader::kFcf)
		str = _T("Fcf");
	else if (type == AcDbLeader::kBlockRef)
		str = _T("Block Ref");
	else if (type == AcDbLeader::kNoAnno)
		str = _T("None");
	else {
		ASSERT(0);
		str = _T("*Unknown*");
	}

	addToProp(p, _T("Annotation"), str);

	if (type != AcDbLeader::kNoAnno) {
		AcDbObject* annoObj;
		Acad::ErrorStatus es = acdbOpenObject(annoObj, leader->annotationObjId(), AcDb::kForRead);
		if (es == Acad::eOk) {
			addToProp(p, _T("Annotation"), ArxDbgUtils::objToClassAndHandleStr(annoObj, str));
			annoObj->close();
		}
		else
			addToProp(p, _T("Annotation"), ArxDbgUtils::rxErrorStr(es));

		addToProp(p, _T("Annotation Offset"), ArxDbgUtils::vectorToStr(leader->annotationOffset(), str));
		addToProp(p, _T("Annotation Width"), ArxDbgUtils::doubleToStr(leader->annoWidth(), str));
		addToProp(p, _T("Annotation Height"), ArxDbgUtils::doubleToStr(leader->annoHeight(), str));
	}

	// TBD: still need the dimvars to be complete!
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop    POLYLINE
**
**  **jma
**
*************************************/

void nti_getprop(AcDbPolyline* pline, nti_prop_t& p)
{
	CString str, str2;
	double tmpDouble;
	AcGePoint3d tmpPt;
	Acad::ErrorStatus es;

	ASSERT(pline != NULL);

	addSeperatorLine(p, _T("AcDbPolyline"));

	addToProp(p, _T("Is Only Lines"), ArxDbgUtils::booleanToStr(pline->isOnlyLines(), str));
	addToProp(p, _T("Is Closed"), ArxDbgUtils::booleanToStr(pline->isClosed(), str));
	addToProp(p, _T("Has Pline Gen"), ArxDbgUtils::booleanToStr(pline->hasPlinegen(), str));
	addToProp(p, _T("Elevation"), ArxDbgUtils::doubleToStr(pline->elevation(), str));
	addToProp(p, _T("Thickness"), ArxDbgUtils::doubleToStr(pline->thickness(), str));
	if (pline->getConstantWidth(tmpDouble) == Acad::eOk)
		addToProp(p, _T("Constant Width"), ArxDbgUtils::doubleToStr(tmpDouble, str));
	else
		addToProp(p, _T("Constant Width"), _T("False"));
	addToProp(p, _T("Normal"), ArxDbgUtils::vectorToStr(pline->normal(), str));

	addSeperatorLine(p, _T("Vertices"));
	unsigned int i;
	for (i = 0; i < pline->numVerts(); i++) {
		es = pline->getPointAt(i, tmpPt);
		if (es == Acad::eOk) {
			str2.Format(_T("Vertex #%d"), i + 1);
			addToProp(p, str2, ArxDbgUtils::ptToStr(tmpPt, str));
		}
	}
	AcDbPolyline::SegType segType;
	double bulge, startWidth, endWidth;

	unsigned int numSegs;
	if (pline->isClosed())
		numSegs = pline->numVerts();
	else
		numSegs = pline->numVerts() - 1;

	for (i = 0; i < numSegs; i++) {
		str2.Format(_T("Segment #%d"), i + 1);
		addSeperatorLine(p, str2);

		segType = pline->segType(i);
		if (segType == AcDbPolyline::kLine) {
			addToProp(p, _T("Type"), _T("Line"));
			AcGeLineSeg3d line;
			es = pline->getLineSegAt(i, line);
			if (es == Acad::eOk) {
				addToProp(p, _T("Start Point"), ArxDbgUtils::ptToStr(line.startPoint(), str));
				addToProp(p, _T("End Point"), ArxDbgUtils::ptToStr(line.endPoint(), str));
			}
			else
				addToProp(p, _T("ERROR"), _T("Could not get Line segment"));
		}
		else if (segType == AcDbPolyline::kArc) {
			addToProp(p, _T("Type"), _T("Arc"));
			AcGeCircArc3d arc;
			es = pline->getArcSegAt(i, arc);
			if (es == Acad::eOk) {
				addToProp(p, _T("Center Point"), ArxDbgUtils::ptToStr(arc.center(), str));
				addToProp(p, _T("Start Point"), ArxDbgUtils::ptToStr(arc.startPoint(), str));
				addToProp(p, _T("End Point"), ArxDbgUtils::ptToStr(arc.endPoint(), str));
				addToProp(p, _T("Radius"), ArxDbgUtils::doubleToStr(arc.radius(), str));
				addToProp(p, _T("Start Angle"), ArxDbgUtils::angleToStr(arc.startAng(), str));
				addToProp(p, _T("End Angle"), ArxDbgUtils::angleToStr(arc.endAng(), str));
				addToProp(p, _T("Normal"), ArxDbgUtils::vectorToStr(arc.normal(), str));
			}
			else
				addToProp(p, _T("ERROR"), _T("Could not get Line segment"));
		}
		else if (segType == AcDbPolyline::kCoincident) {
			addToProp(p, _T("Type"), _T("Coincident"));

		}
		else if (segType == AcDbPolyline::kPoint) {
			addToProp(p, _T("Type"), _T("Point"));
			if (pline->getPointAt(i, tmpPt) == Acad::eOk)
				addToProp(p, _T("Point"), ArxDbgUtils::ptToStr(tmpPt, str));
			else
				addToProp(p, _T("ERROR"), _T("Could not get point"));
		}
		else if (segType == AcDbPolyline::kEmpty) {
			addToProp(p, _T("Type"), _T("Empty"));
		}
		else {
			ASSERT(0);
			addToProp(p, _T("Type"), _T("*Unknown"));
		}

		es = pline->getBulgeAt(i, bulge);
		if (es == Acad::eOk)
			addToProp(p, _T("Bulge"), ArxDbgUtils::doubleToStr(bulge, str));
		else
			addToProp(p, _T("ERROR"), _T("Could not get bulge"));

		es = pline->getWidthsAt(i, startWidth, endWidth);
		if (es == Acad::eOk) {
			addToProp(p, _T("Start Width"), ArxDbgUtils::doubleToStr(startWidth, str));
			addToProp(p, _T("End Width"), ArxDbgUtils::doubleToStr(endWidth, str));
		}
		else
			addToProp(p, _T("ERROR"), _T("Could not get widths"));
	}
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop    LINE
**
**  **jma
**
*************************************/

void nti_getprop(AcDbLine* line, nti_prop_t& p)
{
	CString str;

	ASSERT(line != NULL);

	addSeperatorLine(p, _T("AcDbLine"));

	addToProp(p, _T("Start Pt"), ArxDbgUtils::ptToStr(line->startPoint(), str));
	addToProp(p, _T("End Pt"), ArxDbgUtils::ptToStr(line->endPoint(), str));
	addToProp(p, _T("Thickness"), ArxDbgUtils::doubleToStr(line->thickness(), str));
	addToProp(p, _T("Normal"), ArxDbgUtils::vectorToStr(line->normal(), str));
}


/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop    RAY
**
**  **jma
**
*************************************/

void nti_getprop(AcDbRay* ray, nti_prop_t& p)
{
	CString str;

	ASSERT(ray != NULL);

	addSeperatorLine(p, _T("AcDbRay"));

	addToProp(p, _T("Base Pt"), ArxDbgUtils::ptToStr(ray->basePoint(), str));
	addToProp(p, _T("Unit Dir"), ArxDbgUtils::vectorToStr(ray->unitDir(), str));
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop    SPLINE
**
**  **jma
**
*************************************/

void nti_getprop(AcDbSpline* spline, nti_prop_t& p)
{
	CString str, str2;
	AcGePoint3d pt;
	AcGeVector3d startTan, endTan;
	int i, count;

	ASSERT(spline != NULL);

	addSeperatorLine(p, _T("AcDbSpline"));

	int degree;
	Adesk::Boolean rational, closed, periodic;
	AcGePoint3dArray controlPoints;
	AcGeDoubleArray  knots;
	AcGeDoubleArray  weights;
	double controlPtTol, knotTol;
	spline->getNurbsData(degree, rational, closed, periodic, controlPoints,
		knots, weights, controlPtTol, knotTol);


	addToProp(p, _T("Is Rational"), ArxDbgUtils::booleanToStr(rational, str));
	addToProp(p, _T("Degree"), ArxDbgUtils::intToStr(degree, str));

	count = controlPoints.length();
	for (i = 0; i < count; i++) {
		str2.Format(_T("Control Pt #%2d"), i + 1);
		addToProp(p, str2, ArxDbgUtils::ptToStr(controlPoints[i], str));
	}

	count = knots.length();
	for (i = 0; i < count; i++) {
		str2.Format(_T("Knot #%2d"), i + 1);
		addToProp(p, str2, ArxDbgUtils::doubleToStr(knots[i], str));
	}

	AcGePoint3dArray  fitPoints;
	Adesk::Boolean    tangentsExist;
	AcGeVector3d      startTangent;
	AcGeVector3d      endTangent;
	AcGe::KnotParameterization  knotParam;
	double            fitTolerance;

	if (!spline->hasFitData())
		return;

	spline->getFitData(fitPoints, tangentsExist, startTangent, endTangent, knotParam, degree, fitTolerance);

	count = fitPoints.length();
	for (i = 0; i < count; i++) {
		str2.Format(_T("Fit Pt #%2d"), i + 1);
		addToProp(p, str2, ArxDbgUtils::ptToStr(fitPoints[i], str));
	}
	addToProp(p, _T("Fit Tolerance"), ArxDbgUtils::doubleToStr(fitTolerance, str));
	addToProp(p, _T("Start Tangent"), ArxDbgUtils::vectorToStr(startTan, str));
	addToProp(p, _T("End Tangent"), ArxDbgUtils::vectorToStr(endTan, str));
	addToProp(p, _T("Knot Param"), ArxDbgUtils::intToStr(knotParam, str));
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop    XLINE
**
**  **jma
**
*************************************/

void nti_getprop(AcDbXline* xline, nti_prop_t& p)
{
	CString str;

	ASSERT(xline != NULL);

	addSeperatorLine(p, _T("AcDbXline"));

	addToProp(p, _T("Base Pt"), ArxDbgUtils::ptToStr(xline->basePoint(), str));
	addToProp(p, _T("Unit Dir"), ArxDbgUtils::vectorToStr(xline->unitDir(), str));
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop    2 LINE ANGULAR DIMENSION
**
**  **jma
**
*************************************/

void nti_getprop(AcDb2LineAngularDimension* dim, nti_prop_t& p)
{
	CString str;

	ASSERT(dim != NULL);

	addSeperatorLine(p, _T("AcDb2LineAngularDimension"));

	addToProp(p, _T("Arc Pt"), ArxDbgUtils::ptToStr(dim->arcPoint(), str));
	addToProp(p, _T("Xline1 Start"), ArxDbgUtils::ptToStr(dim->xLine1Start(), str));
	addToProp(p, _T("Xline1 End"), ArxDbgUtils::ptToStr(dim->xLine1End(), str));
	addToProp(p, _T("Xline2 Start"), ArxDbgUtils::ptToStr(dim->xLine2Start(), str));
	addToProp(p, _T("Xline2 End"), ArxDbgUtils::ptToStr(dim->xLine2End(), str));
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop    3 POINT ANGULAR DIMENSION
**
**  **jma
**
*************************************/

void nti_getprop(AcDb3PointAngularDimension* dim, nti_prop_t& p)
{
	CString str;

	ASSERT(dim != NULL);

	addSeperatorLine(p, _T("AcDb3PointAngularDimension"));

	addToProp(p, _T("Arc Pt"), ArxDbgUtils::ptToStr(dim->arcPoint(), str));
	addToProp(p, _T("Xline Pt1"), ArxDbgUtils::ptToStr(dim->xLine1Point(), str));
	addToProp(p, _T("Xline Pt2"), ArxDbgUtils::ptToStr(dim->xLine2Point(), str));
	addToProp(p, _T("Center Pt"), ArxDbgUtils::ptToStr(dim->centerPoint(), str));
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop    ALIGNED DIMENSION
**
**  **jma
**
*************************************/

void nti_getprop(AcDbAlignedDimension* dim, nti_prop_t& p)
{
	CString str;

	ASSERT(dim != NULL);

	addSeperatorLine(p, _T("AcDbAlignedDimension"));

	addToProp(p, _T("Xline Pt1"), ArxDbgUtils::ptToStr(dim->xLine1Point(), str));
	addToProp(p, _T("Xline Pt2"), ArxDbgUtils::ptToStr(dim->xLine2Point(), str));
	addToProp(p, _T("Dimline Pt"), ArxDbgUtils::ptToStr(dim->dimLinePoint(), str));
	addToProp(p, _T("Oblique Angle"), ArxDbgUtils::angleToStr(dim->oblique(), str));
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop    DIAMETRIC DIMENSION
**
**  **jma
**
*************************************/

void nti_getprop(AcDbDiametricDimension* dim, nti_prop_t& p)
{
	CString str;

	ASSERT(dim != NULL);

	addSeperatorLine(p, _T("AcDbDiametricDimension"));

	addToProp(p, _T("Leader Length"), ArxDbgUtils::doubleToStr(dim->leaderLength(), str));
	addToProp(p, _T("Chord Pt"), ArxDbgUtils::ptToStr(dim->chordPoint(), str));
	addToProp(p, _T("Far Chord Pt"), ArxDbgUtils::ptToStr(dim->farChordPoint(), str));
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop    ORDINATE DIMENION
**
**  **jma
**
*************************************/

void nti_getprop(AcDbOrdinateDimension* dim, nti_prop_t& p)
{
	CString str;

	ASSERT(dim != NULL);

	addSeperatorLine(p, _T("AcDbOrdinateDimension"));

	addToProp(p, _T("Is Using X Axis"), ArxDbgUtils::booleanToStr(dim->isUsingXAxis(), str));
	addToProp(p, _T("Is Using Y Axis"), ArxDbgUtils::booleanToStr(dim->isUsingYAxis(), str));
	addToProp(p, _T("Origin Pt"), ArxDbgUtils::ptToStr(dim->origin(), str));
	addToProp(p, _T("Defining Pt"), ArxDbgUtils::ptToStr(dim->definingPoint(), str));
	addToProp(p, _T("Leader End Pt"), ArxDbgUtils::ptToStr(dim->leaderEndPoint(), str));
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop    RADIAL DIMENSION
**
**  **jma
**
*************************************/

void nti_getprop(AcDbRadialDimension* dim, nti_prop_t& p)
{
	CString str;

	ASSERT(dim != NULL);

	addSeperatorLine(p, _T("AcDbRadialDimension"));

	addToProp(p, _T("Leader Length"), ArxDbgUtils::doubleToStr(dim->leaderLength(), str));
	addToProp(p, _T("Center"), ArxDbgUtils::ptToStr(dim->center(), str));
	addToProp(p, _T("Chord Pt"), ArxDbgUtils::ptToStr(dim->chordPoint(), str));
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop    ROTATED DIMENSION
**
**  **jma
**
*************************************/

void nti_getprop(AcDbRotatedDimension* dim, nti_prop_t& p)
{
	CString str;

	ASSERT(dim != NULL);

	addSeperatorLine(p, _T("AcDbRotatedDimension"));

	addToProp(p, _T("Xline Pt1"), ArxDbgUtils::ptToStr(dim->xLine1Point(), str));
	addToProp(p, _T("Xline Pt2"), ArxDbgUtils::ptToStr(dim->xLine2Point(), str));
	addToProp(p, _T("Dimline Pt"), ArxDbgUtils::ptToStr(dim->dimLinePoint(), str));
	addToProp(p, _T("Oblique Angle"), ArxDbgUtils::angleToStr(dim->oblique(), str));
	addToProp(p, _T("Rotation"), ArxDbgUtils::angleToStr(dim->rotation(), str));
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop	(OLE FRAME)
**
**  **jma
**
*************************************/

void nti_getprop(AcDbOleFrame* oleFrame, nti_prop_t& p)
{
	CString str;

	ASSERT(oleFrame != NULL);

	addSeperatorLine(p, _T("AcDbOleFrame"));
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop	(OLE 2 FRAME)
**
**  **jma
**
*************************************/

void nti_getprop(AcDbOle2Frame* ole2Frame, nti_prop_t& p)
{
	CString str;

	ASSERT(ole2Frame != NULL);

	addSeperatorLine(p, _T("AcDbOle2Frame"));
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop    ATTRIBUTE
**
**  **jma
**
*************************************/

void nti_getprop(AcDbAttribute* attrib, nti_prop_t& p)
{
	CString str;

	ASSERT(attrib != NULL);

	addSeperatorLine(p, _T("AcDbAttribute"));

	const TCHAR* tmpStr = attrib->tagConst();
	addToProp(p, _T("Tag"), tmpStr, true);

	addToProp(p, _T("Invisible"), ArxDbgUtils::booleanToStr(attrib->isInvisible(), str));
	addToProp(p, _T("Constant"), ArxDbgUtils::booleanToStr(attrib->isConstant(), str));
	addToProp(p, _T("Verifiable"), ArxDbgUtils::booleanToStr(attrib->isVerifiable(), str));
	addToProp(p, _T("Preset"), ArxDbgUtils::booleanToStr(attrib->isPreset(), str));
	addToProp(p, _T("Field Length"), ArxDbgUtils::intToStr(attrib->fieldLength(), str));
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop    ATTDEF
**
**  **jma
**
*************************************/

void nti_getprop(AcDbAttributeDefinition* attdef, nti_prop_t& p)
{
	CString str;

	ASSERT(attdef != NULL);

	addSeperatorLine(p, _T("AcDbAttributeDefinition"));

	const TCHAR* tmpStr = attdef->promptConst();
	addToProp(p, _T("Prompt"), tmpStr, true);

	tmpStr = attdef->tagConst();
	addToProp(p, _T("Tag"), tmpStr, true);

	addToProp(p, _T("Invisible"), ArxDbgUtils::booleanToStr(attdef->isInvisible(), str));
	addToProp(p, _T("Constant"), ArxDbgUtils::booleanToStr(attdef->isConstant(), str));
	addToProp(p, _T("Verifiable"), ArxDbgUtils::booleanToStr(attdef->isVerifiable(), str));
	addToProp(p, _T("Preset"), ArxDbgUtils::booleanToStr(attdef->isPreset(), str));
	addToProp(p, _T("Field Length"), ArxDbgUtils::intToStr(attdef->fieldLength(), str));
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop    2D VERTEX
**
**  **jma
**
*************************************/

void nti_getprop(AcDb2dVertex* vertex, nti_prop_t& p)
{
	CString str;

	ASSERT(vertex != NULL);

	addSeperatorLine(p, _T("AcDb2dVertex"));

	AcDb::Vertex2dType vertexType = vertex->vertexType();
	if (vertexType == AcDb::k2dVertex)
		str = _T("2D Vertex");
	else if (vertexType == AcDb::k2dSplineCtlVertex)
		str = _T("Spline Control Vertex");
	else if (vertexType == AcDb::k2dSplineFitVertex)
		str = _T("Spline Fit Vertex");
	else if (vertexType == AcDb::k2dCurveFitVertex)
		str = _T("Curve Fit Vertex");
	else
		str = _T("");

	addToProp(p, _T("Vertex Type"), str);
	addToProp(p, _T("Position"), ArxDbgUtils::ptToStr(vertex->position(), str));
	addToProp(p, _T("Start Width"), ArxDbgUtils::doubleToStr(vertex->startWidth(), str));
	addToProp(p, _T("End Width"), ArxDbgUtils::doubleToStr(vertex->endWidth(), str));
	addToProp(p, _T("Bulge"), ArxDbgUtils::doubleToStr(vertex->bulge(), str));
	addToProp(p, _T("Is Tangent Used"), ArxDbgUtils::booleanToStr(vertex->isTangentUsed(), str));
	if (vertex->isTangentUsed())
		addToProp(p, _T("Tangent"), ArxDbgUtils::doubleToStr(vertex->tangent(), str));
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop    3D POLYLINE VERTEX
**
**  **jma
**
*************************************/

void nti_getprop(AcDb3dPolylineVertex* vertex, nti_prop_t& p)
{
	CString str;

	ASSERT(vertex != NULL);

	addSeperatorLine(p, _T("AcDb3dPolylineVertex"));

	addToProp(p, _T("Vertex Type"), ArxDbgUtils::vertexTypeToStr(vertex->vertexType(), str));
	addToProp(p, _T("Position"), ArxDbgUtils::ptToStr(vertex->position(), str));
}


/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop    POLYFACE MESH VERTEX
**
**  **jma
**
*************************************/

void nti_getprop(AcDbPolyFaceMeshVertex* vertex, nti_prop_t& p)
{
	CString str;

	ASSERT(vertex != NULL);

	addSeperatorLine(p, _T("AcDbPolyFaceMeshVertex"));

	addToProp(p, _T("Position"), ArxDbgUtils::ptToStr(vertex->position(), str));
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop    POLYGON MESH VERTEX
**
**  **jma
**
*************************************/

void nti_getprop(AcDbPolygonMeshVertex* vertex, nti_prop_t& p)
{
	CString str;

	ASSERT(vertex != NULL);

	addSeperatorLine(p, _T("AcDbPolygonMeshVertex"));

	addToProp(p, _T("Vertex Type"), ArxDbgUtils::vertexTypeToStr(vertex->vertexType(), str));
	addToProp(p, _T("Position"), ArxDbgUtils::ptToStr(vertex->position(), str));
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop	(LAYER FILTER)
**
**  **jma
**
*************************************/

void nti_getprop(AcDbLayerFilter* filter, nti_prop_t& p)
{
	CString str;

	ASSERT(filter != NULL);

	addSeperatorLine(p, _T("AcDbLayerFilter"));
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop	(SPATIAL FILTER)
**
**  **jma
**
*************************************/

void nti_getprop(AcDbSpatialFilter* filter, nti_prop_t& p)
{
	CString str;

	ASSERT(filter != NULL);

	addSeperatorLine(p, _T("AcDbSpatialFilter"));
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop	(LAYER INDEX)
**
**  **jma
**
*************************************/

void nti_getprop(AcDbLayerIndex* index, nti_prop_t& p)
{
	CString str;

	ASSERT(index != NULL);

	addSeperatorLine(p, _T("AcDbLayerIndex"));
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop	(SPATIAL INDEX)
**
**  **jma
**
*************************************/

void nti_getprop(AcDbSpatialIndex* index, nti_prop_t& p)
{
	CString str;

	ASSERT(index != NULL);

	addSeperatorLine(p, _T("AcDbSpatialIndex"));
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop	(LAYOUT)
**
**  **jma
**
*************************************/

void nti_getprop(AcDbLayout* layout, nti_prop_t& p)
{
	CString str;

	ASSERT(layout != NULL);

	addSeperatorLine(p, _T("AcDbLayout"));
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop
**
**  **jma
**
*************************************/

void nti_getprop(AcDbAbstractViewTableRecord* view, nti_prop_t& p)
{
	CString str;

	addSeperatorLine(p, _T("AcDbAbstractViewTableRecord"));

	addToProp(p, _T("Center Pt"), ArxDbgUtils::ptToStr(view->centerPoint(), str));
	addToProp(p, _T("Height"), ArxDbgUtils::doubleToStr(view->height(), str));
	addToProp(p, _T("Width"), ArxDbgUtils::doubleToStr(view->width(), str));
	addToProp(p, _T("Target"), ArxDbgUtils::ptToStr(view->target(), str));
	addToProp(p, _T("View Dir"), ArxDbgUtils::vectorToStr(view->viewDirection(), str));
	addToProp(p, _T("View Twist"), ArxDbgUtils::doubleToStr(view->viewTwist(), str));
	addToProp(p, _T("Lens Length"), ArxDbgUtils::doubleToStr(view->lensLength(), str));
	addToProp(p, _T("Front Clip Dist"), ArxDbgUtils::doubleToStr(view->frontClipDistance(), str));
	addToProp(p, _T("Back Clip Dist"), ArxDbgUtils::doubleToStr(view->backClipDistance(), str));
	addToProp(p, _T("Perspective On"), ArxDbgUtils::booleanToStr(view->perspectiveEnabled(), str));
	addToProp(p, _T("Front Clip On"), ArxDbgUtils::booleanToStr(view->frontClipEnabled(), str));
	addToProp(p, _T("Back Clip On"), ArxDbgUtils::booleanToStr(view->backClipEnabled(), str));
	addToProp(p, _T("Front Clip At Eye"), ArxDbgUtils::booleanToStr(view->frontClipAtEye(), str));

	AcGePoint3d origin;
	AcGeVector3d xAxis, yAxis;
	Acad::ErrorStatus es = view->getUcs(origin, xAxis, yAxis);
	if (es == Acad::eOk) {
		addToProp(p, _T("UCS Origin"), ArxDbgUtils::ptToStr(origin, str));
		addToProp(p, _T("UCS X Axis"), ArxDbgUtils::vectorToStr(xAxis, str));
		addToProp(p, _T("UCS Y Axis"), ArxDbgUtils::vectorToStr(yAxis, str));
	}
	else
		addToProp(p, _T("UCS Info"), ArxDbgUtils::rxErrorStr(es));

	AcDb::OrthographicView orthoView;
	bool ucsIsOrtho = view->isUcsOrthographic(orthoView);
	addToProp(p, _T("Is UCS Orthographic"), ArxDbgUtils::booleanToStr(ucsIsOrtho, str));
	if (ucsIsOrtho)
		addToProp(p, _T("UCS Orthographic View"), ArxDbgUtils::orthoViewToStr(orthoView, str));

	ArxDbgUtils::symbolIdToName(view->ucsName(), str);
	addToProp(p, _T("UCS Name"), str);
	addToProp(p, _T("Elevation"), ArxDbgUtils::doubleToStr(view->elevation(), str));

	bool viewIsOrtho = view->isViewOrthographic(orthoView);
	addToProp(p, _T("Is View Orthographic"), ArxDbgUtils::booleanToStr(viewIsOrtho, str));
	if (viewIsOrtho)
		addToProp(p, _T("Orthographic View"), ArxDbgUtils::orthoViewToStr(orthoView, str));

	AcDbViewportTableRecord* vportRec;
	AcDbViewTableRecord* viewRec;

	if ((vportRec = AcDbViewportTableRecord::cast(view)) != NULL)
		nti_getprop(vportRec, p);
	else if ((viewRec = AcDbViewTableRecord::cast(view)) != NULL)
		nti_getprop(viewRec, p);
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop        (BLOCK)
**
**  **jma
**
*************************************/

void nti_getprop(AcDbBlockTableRecord* blk, nti_prop_t& p)
{
	CString str;
	const TCHAR* locName;
	Acad::ErrorStatus es;

	addSeperatorLine(p, _T("AcDbBlockTableRecord"));

	es = blk->pathName(locName);
	if (es == Acad::eOk) {
		addToProp(p, _T("Path Name"), locName, true);
	}

	addToProp(p, _T("Origin"), ArxDbgUtils::ptToStr(blk->origin(), str));
	addToProp(p, _T("Has Attribute Defs"), ArxDbgUtils::booleanToStr(blk->hasAttributeDefinitions(), str));
	addToProp(p, _T("Has Preview Icon"), ArxDbgUtils::booleanToStr(blk->hasPreviewIcon(), str));
	addToProp(p, _T("Is Anonymous"), ArxDbgUtils::booleanToStr(blk->isAnonymous(), str));
	addToProp(p, _T("Is From Xref"), ArxDbgUtils::booleanToStr(blk->isFromExternalReference(), str));
	addToProp(p, _T("Is From Overlay Ref"), ArxDbgUtils::booleanToStr(blk->isFromOverlayReference(), str));
	addToProp(p, _T("Is Layout"), ArxDbgUtils::booleanToStr(blk->isLayout(), str));

	AcDbObject* tmpObj;
	es = acdbOpenObject(tmpObj, blk->getLayoutId(), AcDb::kForRead);
	if (es == Acad::eOk) {
		addToProp(p, _T("Layout Object"), ArxDbgUtils::objToClassAndHandleStr(tmpObj, str));
		tmpObj->close();
	}
	else
		addToProp(p, _T("Layout Object"), ArxDbgUtils::rxErrorStr(es));

	addToProp(p, _T("Xref Database"), ArxDbgUtils::ptrToStr(blk->xrefDatabase(), str));
	addToProp(p, _T("Is Unloaded"), ArxDbgUtils::booleanToStr(blk->isUnloaded(), str));
	addToProp(p, _T("Xref Status"), ArxDbgUtils::xrefStatusToStr(blk->xrefStatus(), str));

	const TCHAR* tmpStr;
	es = blk->comments(tmpStr);
	if (es == Acad::eOk)
		str = tmpStr;
	else
		ArxDbgUtils::rxErrorStr(es);

	addToProp(p, _T("Comments"), str, true);
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop        (DIMSTYLE)
**
**  **jma
**
*************************************/

void nti_getprop(AcDbDimStyleTableRecord* dimStyle, nti_prop_t& p)
{
	CString str;

	addSeperatorLine(p, _T("AcDbDimStyleTableRecord"));

	ArxDbgUtils::symbolIdToName(dimStyle->arrowId(AcDb::kFirstArrow), str);
	addToProp(p, _T("First Arrow"), str);

	ArxDbgUtils::symbolIdToName(dimStyle->arrowId(AcDb::kSecondArrow), str);
	addToProp(p, _T("Second Arrow"), str);

	addToProp(p, _T("DIMADEC"), ArxDbgUtils::intToStr(dimStyle->dimadec(), str));
	addToProp(p, _T("DIMALT"), ArxDbgUtils::booleanToStr(dimStyle->dimalt(), str));
	addToProp(p, _T("DIMALTD"), ArxDbgUtils::intToStr(dimStyle->dimaltd(), str));
	addToProp(p, _T("DIMALTF"), ArxDbgUtils::doubleToStr(dimStyle->dimaltf(), str));
	addToProp(p, _T("DIMALTRND"), ArxDbgUtils::doubleToStr(dimStyle->dimaltrnd(), str));
	addToProp(p, _T("DIMALTTD"), ArxDbgUtils::intToStr(dimStyle->dimalttd(), str));
	addToProp(p, _T("DIMALTU"), ArxDbgUtils::intToStr(dimStyle->dimaltu(), str));
	addToProp(p, _T("DIMALTTZ"), ArxDbgUtils::intToStr(dimStyle->dimalttz(), str));
	addToProp(p, _T("DIMALTZ"), ArxDbgUtils::intToStr(dimStyle->dimaltz(), str));
	addToProp(p, _T("DIMAPOST"), dimStyle->dimapost(), true);
	addToProp(p, _T("DIMASZ"), ArxDbgUtils::doubleToStr(dimStyle->dimasz(), str));
	addToProp(p, _T("DIMAUNIT"), ArxDbgUtils::intToStr(dimStyle->dimaunit(), str));
	addToProp(p, _T("DIMAZIN"), ArxDbgUtils::intToStr(dimStyle->dimazin(), str));

	ArxDbgUtils::symbolIdToName(dimStyle->dimblk(), str);
	addToProp(p, _T("DIMBLK"), str);

	ArxDbgUtils::symbolIdToName(dimStyle->dimblk1(), str);
	addToProp(p, _T("DIMBLK1"), str);

	ArxDbgUtils::symbolIdToName(dimStyle->dimblk2(), str);
	addToProp(p, _T("DIMBLK2"), str);

	addToProp(p, _T("DIMCEN"), ArxDbgUtils::doubleToStr(dimStyle->dimcen(), str));
	addToProp(p, _T("DIMCLRD"), ArxDbgUtils::colorToStr(dimStyle->dimclrd().colorIndex(), str, true));
	addToProp(p, _T("DIMCLRE"), ArxDbgUtils::colorToStr(dimStyle->dimclre().colorIndex(), str, true));
	addToProp(p, _T("DIMCLRT"), ArxDbgUtils::colorToStr(dimStyle->dimclrt().colorIndex(), str, true));
	addToProp(p, _T("DIMDEC"), ArxDbgUtils::intToStr(dimStyle->dimdec(), str));
	addToProp(p, _T("DIMDLE"), ArxDbgUtils::doubleToStr(dimStyle->dimdle(), str));
	addToProp(p, _T("DIMDLI"), ArxDbgUtils::doubleToStr(dimStyle->dimdli(), str));
	addToProp(p, _T("DIMDSEP"), ArxDbgUtils::intToStr(dimStyle->dimdsep(), str));
	addToProp(p, _T("DIMEXE"), ArxDbgUtils::doubleToStr(dimStyle->dimexe(), str));
	addToProp(p, _T("DIMEXO"), ArxDbgUtils::doubleToStr(dimStyle->dimexo(), str));
	addToProp(p, _T("DIMATFIT"), ArxDbgUtils::intToStr(dimStyle->dimatfit(), str));
	addToProp(p, _T("DIMFRAC"), ArxDbgUtils::intToStr(dimStyle->dimfrac(), str));
	addToProp(p, _T("DIMGAP"), ArxDbgUtils::doubleToStr(dimStyle->dimgap(), str));
	addToProp(p, _T("DIMJUST"), ArxDbgUtils::intToStr(dimStyle->dimjust(), str));

	ArxDbgUtils::symbolIdToName(dimStyle->dimldrblk(), str);
	addToProp(p, _T("DIMLDRBLK"), str);

	addToProp(p, _T("DIMLFAC"), ArxDbgUtils::doubleToStr(dimStyle->dimlfac(), str));
	addToProp(p, _T("DIMLIM"), ArxDbgUtils::booleanToStr(dimStyle->dimlim(), str));
	addToProp(p, _T("DIMLUNIT"), ArxDbgUtils::intToStr(dimStyle->dimlunit(), str));
	addToProp(p, _T("DIMLWD"), ArxDbgUtils::lineWeightTypeToStr(dimStyle->dimlwd(), str));
	addToProp(p, _T("DIMLWE"), ArxDbgUtils::lineWeightTypeToStr(dimStyle->dimlwe(), str));
	addToProp(p, _T("DIMPOST"), dimStyle->dimpost(), true);
	addToProp(p, _T("DIMRND"), ArxDbgUtils::doubleToStr(dimStyle->dimrnd(), str));
	addToProp(p, _T("DIMSAH"), ArxDbgUtils::booleanToStr(dimStyle->dimsah(), str));
	addToProp(p, _T("DIMSCALE"), ArxDbgUtils::doubleToStr(dimStyle->dimscale(), str));
	addToProp(p, _T("DIMSD1"), ArxDbgUtils::booleanToStr(dimStyle->dimsd1(), str));
	addToProp(p, _T("DIMSD2"), ArxDbgUtils::booleanToStr(dimStyle->dimsd2(), str));
	addToProp(p, _T("DIMSE1"), ArxDbgUtils::booleanToStr(dimStyle->dimse1(), str));
	addToProp(p, _T("DIMSE2"), ArxDbgUtils::booleanToStr(dimStyle->dimse2(), str));
	addToProp(p, _T("DIMSOXD"), ArxDbgUtils::booleanToStr(dimStyle->dimsoxd(), str));
	addToProp(p, _T("DIMTAD"), ArxDbgUtils::intToStr(dimStyle->dimtad(), str));
	addToProp(p, _T("DIMTDEC"), ArxDbgUtils::intToStr(dimStyle->dimtdec(), str));
	addToProp(p, _T("DIMTFAC"), ArxDbgUtils::doubleToStr(dimStyle->dimtfac(), str));
	addToProp(p, _T("DIMTIH"), ArxDbgUtils::booleanToStr(dimStyle->dimtih(), str));
	addToProp(p, _T("DIMTIX"), ArxDbgUtils::booleanToStr(dimStyle->dimtix(), str));
	addToProp(p, _T("DIMTM"), ArxDbgUtils::doubleToStr(dimStyle->dimtm(), str));
	addToProp(p, _T("DIMTMOVE"), ArxDbgUtils::intToStr(dimStyle->dimtmove(), str));
	addToProp(p, _T("DIMTOFL"), ArxDbgUtils::booleanToStr(dimStyle->dimtofl(), str));
	addToProp(p, _T("DIMTOH"), ArxDbgUtils::booleanToStr(dimStyle->dimtoh(), str));
	addToProp(p, _T("DIMTOL"), ArxDbgUtils::booleanToStr(dimStyle->dimtol(), str));
	addToProp(p, _T("DIMTOLJ"), ArxDbgUtils::intToStr(dimStyle->dimtolj(), str));
	addToProp(p, _T("DIMTP"), ArxDbgUtils::doubleToStr(dimStyle->dimtp(), str));
	addToProp(p, _T("DIMTSZ"), ArxDbgUtils::doubleToStr(dimStyle->dimtsz(), str));
	addToProp(p, _T("DIMTVP"), ArxDbgUtils::doubleToStr(dimStyle->dimtvp(), str));

	ArxDbgUtils::symbolIdToName(dimStyle->dimtxsty(), str);
	addToProp(p, _T("DIMTXSTY"), str);

	addToProp(p, _T("DIMTXT"), ArxDbgUtils::doubleToStr(dimStyle->dimtxt(), str));
	addToProp(p, _T("DIMTZIN"), ArxDbgUtils::intToStr(dimStyle->dimtzin(), str));
	addToProp(p, _T("DIMUPT"), ArxDbgUtils::booleanToStr(dimStyle->dimupt(), str));
	addToProp(p, _T("DIMZIN"), ArxDbgUtils::intToStr(dimStyle->dimzin(), str));

	addToProp(p, _T("DIMFIT (Deprecated)"), ArxDbgUtils::intToStr(dimStyle->dimfit(), str));
	addToProp(p, _T("DIMUNIT (Deprecated)"), ArxDbgUtils::intToStr(dimStyle->dimunit(), str));
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop        (LAYER)
**
**  **jma
**
*************************************/

void nti_getprop(AcDbLayerTableRecord* layer, nti_prop_t& p)
{
	CString str;

	addSeperatorLine(p, _T("AcDbLayerTableRecord"));
	addToProp(p, _T("Is Hidden"), ArxDbgUtils::booleanToStr(layer->isHidden(), str));
	addToProp(p, _T("Is Frozen"), ArxDbgUtils::booleanToStr(layer->isFrozen(), str));
	addToProp(p, _T("Is Off"), ArxDbgUtils::booleanToStr(layer->isOff(), str));
	addToProp(p, _T("Is Locked"), ArxDbgUtils::booleanToStr(layer->isLocked(), str));
	addToProp(p, _T("Is VP Default"), ArxDbgUtils::booleanToStr(layer->VPDFLT(), str));
	addToProp(p, _T("Color"), ArxDbgUtils::intToStr(layer->color().colorIndex(), str));

	ArxDbgUtils::symbolIdToName(layer->linetypeObjectId(), str);
	addToProp(p, _T("Linetype"), str);

	addToProp(p, _T("Lineweight"), ArxDbgUtils::lineWeightTypeToStr(layer->lineWeight(), str));
	addToProp(p, _T("Is Plottable"), ArxDbgUtils::booleanToStr(layer->isPlottable(), str));

	TCHAR* name = layer->plotStyleName();
	addToProp(p, _T("Plot Style Name"), name);
	acutDelString(name);
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop        (LINETYPE)
**
**  **jma
**
*************************************/

void nti_getprop(AcDbLinetypeTableRecord* linetype, nti_prop_t& p)
{
	CString str;
	const TCHAR* locName;
	Acad::ErrorStatus es;

	addSeperatorLine(p, _T("AcDbLinetypeTableRecord"));

	es = linetype->asciiDescription(locName);
	if (es == Acad::eOk) {
		str = locName;
	}
	else
		str = ArxDbgUtils::rxErrorStr(es);
	addToProp(p, _T("Description"), str);

	addToProp(p, _T("Pattern Length"), ArxDbgUtils::doubleToStr(linetype->patternLength(), str));

	AcDbObjectId shapeStyleId;
	const TCHAR* tmpTxt;
	int i, len;
	len = linetype->numDashes();
	for (i = 0; i < len; i++) {
		str.Format(_T("--- DASH #%2d ---"), i + 1);
		addToProp(p, str, _T(""));
		addToProp(p, _T("Length"), ArxDbgUtils::doubleToStr(linetype->dashLengthAt(i), str));

		shapeStyleId = linetype->shapeStyleAt(i);
		if (shapeStyleId != AcDbObjectId::kNull) {
			if (ArxDbgUtils::symbolIdToName(shapeStyleId, str) != Acad::eOk)
				str = _T("*ERROR*");
			addToProp(p, _T("Shape Style"), str);
			addToProp(p, _T("Shape Number"), ArxDbgUtils::intToStr(linetype->shapeNumberAt(i), str));
			addToProp(p, _T("Shape Offset"), ArxDbgUtils::vectorToStr(linetype->shapeOffsetAt(i), str));
			addToProp(p, _T("Shape Scale"), ArxDbgUtils::doubleToStr(linetype->shapeScaleAt(i), str));
			addToProp(p, _T("Shape UCS Oriented"), ArxDbgUtils::booleanToStr(linetype->shapeIsUcsOrientedAt(i), str));
			addToProp(p, _T("Shape Rotation"), ArxDbgUtils::angleToStr(linetype->shapeRotationAt(i), str));
			es = linetype->textAt(i, tmpTxt);
			if (es == Acad::eOk)
				addToProp(p, _T("Text"), tmpTxt, true);
			else
				addToProp(p, _T("Text"), ArxDbgUtils::rxErrorStr(es));
		}
	}

	addToProp(p, _T("Is Scaled To Fit"), ArxDbgUtils::booleanToStr(linetype->isScaledToFit(), str));

	const TCHAR* tmpStr;
	es = linetype->comments(tmpStr);
	if (es == Acad::eOk)
		str = tmpStr;
	else
		ArxDbgUtils::rxErrorStr(es);

	addToProp(p, _T("Comments"), str, true);
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop        (REGAPP)
**
**  **jma
**
*************************************/

void nti_getprop(AcDbRegAppTableRecord* regApp, nti_prop_t& p)
{
	addSeperatorLine(p, _T("AcDbRegAppTableRecord"));
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop        (TextStyle)
**
**  **jma
**
*************************************/

void nti_getprop(AcDbTextStyleTableRecord* textStyle, nti_prop_t& p)
{
	CString str;
	const TCHAR* locName;
	Acad::ErrorStatus es;

	addSeperatorLine(p, _T("AcDbTextStyleTableRecord"));

	es = textStyle->fileName(locName);
	if (es == Acad::eOk)
		str = locName;
	else
		str = ArxDbgUtils::rxErrorStr(es);
	addToProp(p, _T("File Name"), str, true);

	es = textStyle->bigFontFileName(locName);
	if (es == Acad::eOk)
		str = locName;
	else
		str = ArxDbgUtils::rxErrorStr(es);
	addToProp(p, _T("Big Font File"), str, true);

	TCHAR* typeFace;
	Adesk::Boolean isBold, isItalic;
	Charset charset;
	using namespace Autodesk::AutoCAD::PAL;
	FontUtils::FontPitch  pitch = FontUtils::FontPitch::kDefault;
	FontUtils::FontFamily family = FontUtils::FontFamily::kDoNotCare;
	es = textStyle->font(typeFace, isBold, isItalic, charset, pitch, family);
	if (es == Acad::eOk) {
		addToProp(p, _T("Font Typeface"), typeFace, true);
		addToProp(p, _T("Font Is Bold"), ArxDbgUtils::booleanToStr(isBold, str));
		addToProp(p, _T("Font Is Italic"), ArxDbgUtils::booleanToStr(isItalic, str));
		addToProp(p, _T("Font Charset"), ArxDbgUtils::intToStr(charset, str));
		int pitchAndFamily = 0;
		pitchAndFamily |= (int)pitch;
		pitchAndFamily |= (int)family;
		addToProp(p, _T("Font Pitch/Family"), ArxDbgUtils::intToStr(pitchAndFamily, str));
		acutDelString(typeFace);
	}
	else
		addToProp(p, _T("Font"), ArxDbgUtils::rxErrorStr(es));

	addToProp(p, _T("Is Shape File"), ArxDbgUtils::booleanToStr(textStyle->isShapeFile(), str));
	addToProp(p, _T("Is Vertical"), ArxDbgUtils::booleanToStr(textStyle->isVertical(), str));
	addToProp(p, _T("Text Size"), ArxDbgUtils::doubleToStr(textStyle->textSize(), str));
	addToProp(p, _T("Prior Size"), ArxDbgUtils::doubleToStr(textStyle->priorSize(), str));
	addToProp(p, _T("X Scale"), ArxDbgUtils::doubleToStr(textStyle->xScale(), str));
	addToProp(p, _T("Oblique Angle"), ArxDbgUtils::angleToStr(textStyle->obliquingAngle(), str));
	addToProp(p, _T("Flag Bits"), ArxDbgUtils::intToStr(textStyle->flagBits(), str));
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop        (UCS)
**
**  **jma
**
*************************************/

void nti_getprop(AcDbUCSTableRecord* ucs, nti_prop_t& p)
{
	CString str;

	addSeperatorLine(p, _T("AcDbUCSTableRecord"));

	addToProp(p, _T("Origin"), ArxDbgUtils::ptToStr(ucs->origin(), str));
	addToProp(p, _T("X Axis"), ArxDbgUtils::vectorToStr(ucs->xAxis(), str));
	addToProp(p, _T("Y Axis"), ArxDbgUtils::vectorToStr(ucs->yAxis(), str));

	// TBD: not sure how to call ucsBaseOrigin()
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop        (VIEW)
**
**  **jma
**
*************************************/

void nti_getprop(AcDbViewTableRecord* view, nti_prop_t& p)
{
	CString str;

	addSeperatorLine(p, _T("AcDbViewTableRecord"));

	addToProp(p, _T("Is PaperSpace View"), ArxDbgUtils::booleanToStr(view->isPaperspaceView(), str));
	addToProp(p, _T("Is UCS Associated View"), ArxDbgUtils::booleanToStr(view->isUcsAssociatedToView(), str));
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::nti_getprop        (VIEWPORT)
**
**  **jma
**
*************************************/

void nti_getprop(AcDbViewportTableRecord* viewport, nti_prop_t& p)
{
	CString str;

	addSeperatorLine(p, _T("AcDbViewportTableRecord"));

	addToProp(p, _T("Lower Left"), ArxDbgUtils::ptToStr(viewport->lowerLeftCorner(), str));
	addToProp(p, _T("Upper Right"), ArxDbgUtils::ptToStr(viewport->upperRightCorner(), str));
	addToProp(p, _T("UCS Follow"), ArxDbgUtils::booleanToStr(viewport->ucsFollowMode(), str));
	addToProp(p, _T("Circle Sides"), ArxDbgUtils::intToStr(viewport->circleSides(), str));
	addToProp(p, _T("Fast Zooms"), ArxDbgUtils::booleanToStr(viewport->fastZoomsEnabled(), str));
	addToProp(p, _T("Icon Enabled"), ArxDbgUtils::booleanToStr(viewport->iconEnabled(), str));
	addToProp(p, _T("Icon At Origin"), ArxDbgUtils::booleanToStr(viewport->iconAtOrigin(), str));
	addToProp(p, _T("Grid On"), ArxDbgUtils::booleanToStr(viewport->gridEnabled(), str));
	addToProp(p, _T("Grid Incr"), ArxDbgUtils::ptToStr(viewport->gridIncrements(), str));
	addToProp(p, _T("Snap On"), ArxDbgUtils::booleanToStr(viewport->snapEnabled(), str));
	addToProp(p, _T("ISO Snap On"), ArxDbgUtils::booleanToStr(viewport->isometricSnapEnabled(), str));
	addToProp(p, _T("Snap Pair"), ArxDbgUtils::intToStr(viewport->snapPair(), str));
	addToProp(p, _T("Snap Angle"), ArxDbgUtils::angleToStr(viewport->snapAngle(), str));
	addToProp(p, _T("Snap Base"), ArxDbgUtils::ptToStr(viewport->snapBase(), str));
	addToProp(p, _T("Snap Incr"), ArxDbgUtils::ptToStr(viewport->snapIncrements(), str));
	addToProp(p, _T("Is UCS Saved WIth Viewport"), ArxDbgUtils::booleanToStr(viewport->isUcsSavedWithViewport(), str));
}

/****************************************************************************
**
**  ArxDbgUiTdcDbObjectBase::displayGeCurve3d
**
**  **jma
**
*************************************/

static void nti_getpropGeCurve2d(const AcGeCurve2d* curve, int edgeType, nti_prop_t& p)
{
	CString curveStr = _T("Curve Type");
	CString str, str2;

	if (curve->isKindOf(AcGe::kLineSeg2d)) {
		ASSERT(edgeType == AcDbHatch::kLine);
		const AcGeLineSeg2d* line = static_cast<const AcGeLineSeg2d*>(curve);
		addToProp(p, curveStr, _T("Line"));
		addToProp(p, _T("Start Point"), ArxDbgUtils::ptToStr(line->startPoint(), str));
		addToProp(p, _T("End Point"), ArxDbgUtils::ptToStr(line->endPoint(), str));
	}
	else if (curve->isKindOf(AcGe::kCircArc2d)) {
		ASSERT(edgeType == AcDbHatch::kCirArc);
		const AcGeCircArc2d* arc = static_cast<const AcGeCircArc2d*>(curve);
		if (arc->isClosed()) {
			addToProp(p, curveStr, _T("Circle"));
			addToProp(p, _T("Center"), ArxDbgUtils::ptToStr(arc->center(), str));
			addToProp(p, _T("Radius"), ArxDbgUtils::doubleToStr(arc->radius(), str));

		}
		else {
			addToProp(p, curveStr, _T("Arc"));
			addToProp(p, _T("Center"), ArxDbgUtils::ptToStr(arc->center(), str));
			addToProp(p, _T("Radius"), ArxDbgUtils::doubleToStr(arc->radius(), str));
			addToProp(p, _T("Start Angle"), ArxDbgUtils::angleToStr(arc->startAng(), str));
			addToProp(p, _T("End Angle"), ArxDbgUtils::angleToStr(arc->endAng(), str));
		}
	}
	else if (curve->isKindOf(AcGe::kEllipArc2d)) {
		ASSERT(edgeType == AcDbHatch::kEllArc);
		const AcGeEllipArc2d* ellip = static_cast<const AcGeEllipArc2d*>(curve);
		if (ellip->isClosed()) {
			addToProp(p, curveStr, _T("Ellipse"));
			addToProp(p, _T("Center"), ArxDbgUtils::ptToStr(ellip->center(), str));
			addToProp(p, _T("Minor Radius"), ArxDbgUtils::doubleToStr(ellip->minorRadius(), str));
			addToProp(p, _T("Major Radius"), ArxDbgUtils::doubleToStr(ellip->majorRadius(), str));
			addToProp(p, _T("Minor Axis"), ArxDbgUtils::vectorToStr(ellip->minorAxis(), str));
			addToProp(p, _T("Major Axis"), ArxDbgUtils::vectorToStr(ellip->majorAxis(), str));
		}
		else {
			addToProp(p, curveStr, _T("Elliptical Arc"));
			addToProp(p, _T("Center"), ArxDbgUtils::ptToStr(ellip->center(), str));
			addToProp(p, _T("Minor Radius"), ArxDbgUtils::doubleToStr(ellip->minorRadius(), str));
			addToProp(p, _T("Major Radius"), ArxDbgUtils::doubleToStr(ellip->majorRadius(), str));
			addToProp(p, _T("Minor Axis"), ArxDbgUtils::vectorToStr(ellip->minorAxis(), str));
			addToProp(p, _T("Major Axis"), ArxDbgUtils::vectorToStr(ellip->majorAxis(), str));
			addToProp(p, _T("Start Angle"), ArxDbgUtils::angleToStr(ellip->startAng(), str));
			addToProp(p, _T("End Angle"), ArxDbgUtils::angleToStr(ellip->endAng(), str));
			addToProp(p, _T("Start Point"), ArxDbgUtils::ptToStr(ellip->startPoint(), str));
			addToProp(p, _T("End Point"), ArxDbgUtils::ptToStr(ellip->endPoint(), str));
		}
	}
	else if (curve->isKindOf(AcGe::kSplineEnt2d)) {
		ASSERT(edgeType == AcDbHatch::kSpline);
		const AcGeSplineEnt2d* spline = static_cast<const AcGeSplineEnt2d*>(curve);
		addToProp(p, curveStr, _T("Spline"));
		addToProp(p, _T("Is Rational"), ArxDbgUtils::booleanToStr(spline->isRational(), str));
		addToProp(p, _T("Has Fit Data"), ArxDbgUtils::booleanToStr(spline->hasFitData(), str));
		addToProp(p, _T("Degree"), ArxDbgUtils::intToStr(spline->degree(), str));
		addToProp(p, _T("Order"), ArxDbgUtils::intToStr(spline->order(), str));
		addSeperatorLine(p, _T("Knots"));
		for (int i = 0; i < spline->numKnots(); i++) {
			str2.Format(_T("Knot #%d"), i + 1);
			addToProp(p, str2, ArxDbgUtils::doubleToStr(spline->knotAt(i), str));
		}
		addSeperatorLine(p, _T("Control Points"));
		for (int ii = 0; ii < spline->numControlPoints(); ii++) {
			str2.Format(_T("Control Pt #%d"), ii + 1);
			addToProp(p, str2, ArxDbgUtils::ptToStr(spline->controlPointAt(ii), str));
		}
	}
	else {
		addToProp(p, _T("Curve Type"), _T("*Unknown*"));
	}
}



/////////////////////////////////////////////////////////////////////////////////////////
//int nti_read(ACHAR const * file)
//{
//	// Set constructor parameter to kFalse so that the
//    // database will be constructed empty.  This way only
//    // what is read in will be in the database.
//    //
//    AcDbDatabase *pDb = new AcDbDatabase(Adesk::kFalse);
//    // The AcDbDatabase::readDwgFile() function
//    // automatically appends a DWG extension if it is not
//    // specified in the filename parameter.
//    //
//    if(Acad::eOk != pDb->readDwgFile(file))
//		return -1;
//    // Open the model space block table record.
//    //
//    AcDbBlockTable *pBlkTbl;
//    pDb->getSymbolTable(pBlkTbl, AcDb::kForRead);
//    AcDbBlockTableRecord *pBlkTblRcd;
//    pBlkTbl->getAt(ACDB_MODEL_SPACE, pBlkTblRcd,
//        AcDb::kForRead);
//    pBlkTbl->close();
//    AcDbBlockTableRecordIterator *pBlkTblRcdItr;
//    pBlkTblRcd->newIterator(pBlkTblRcdItr);
//    AcDbEntity *pEnt;
//    for (pBlkTblRcdItr->start(); !pBlkTblRcdItr->done();
//        pBlkTblRcdItr->step())
//    {
//        pBlkTblRcdItr->getEntity(pEnt,
//            AcDb::kForRead);
//        acutPrintf(_T("classname: %s\n"),  (pEnt->isA())->name());
//        pEnt->close();
//    }
//    pBlkTblRcd->close();
//    delete pBlkTblRcdItr;
//    delete pDb;
//	return 0;
//}
//
//void
//refEditApiExample()
//{
//	AcDbObjectId transId;
//	AcDbDatabase* pDb;
//	TCHAR *fname;
//	struct resbuf *rb;
//	// Get a dwg file from the user.
//	//
//	rb = acutNewRb(RTSTR);
//	int stat = acedGetFileD(_T("Pick a drawing"), NULL, _T("dwg"),
//		0, rb);
//	if ((stat != RTNORM) || (rb == NULL)) {
//		acutPrintf(_T("\nYou must pick a drawing file."));        return;
//	}
//	fname = (TCHAR*)acad_malloc((_tcslen(rb->resval.rstring) + 1) *
//		sizeof(TCHAR));
//	_tcscpy(fname, rb->resval.rstring);
//	acutRelRb(rb);
//	// Open the dwg file.
//	//
//	pDb = new AcDbDatabase(Adesk::kFalse);
//	if (pDb->readDwgFile(fname) != Acad::eOk) {
//		acutPrintf(_T("\nSorry, that drawing is probably already open."));
//			return;
//	}
//	// Get the Block Table and then the model space record.
//	//
//	AcDbBlockTable *pBlockTable;
//	pDb->getSymbolTable(pBlockTable, AcDb::kForRead);
//	AcDbBlockTableRecord *pOtherMsBtr;
//	pBlockTable->getAt(ACDB_MODEL_SPACE, pOtherMsBtr,
//		AcDb::kForRead);
//	pBlockTable->close();
//	// Create an iterator.
//	//
//	AcDbBlockTableRecordIterator *pIter;
//	pOtherMsBtr->newIterator(pIter);
//	// Set up an object ID array.
//	//
//	AcDbObjectIdArray objIdArray;
//	// Iterate over the model space BTR. Look specifically
//	// for lines and append their object ID to the array.
//	//
//	for (pIter->start(); !pIter->done(); pIter->step()) {
//		AcDbEntity *pEntity;
//		pIter->getEntity(pEntity, AcDb::kForRead);
//		// Look for only AcDbLine objects and add them to the
//		// object ID array.
//		//
//		if (pEntity->isKindOf(AcDbLine::desc())) {
//			objIdArray.append(pEntity->objectId());
//		}
//		pEntity->close();
//	}
//	delete pIter;
//	pOtherMsBtr->close();
//	if (objIdArray.isEmpty()) {
//		acad_free(fname);
//		acutPrintf(_T("\nYou must pick a drawing file that contains lines."));
//			return;
//	}
//	// Now get the current database and the object ID for the
//	// current database's model space BTR.
//	//
//	AcDbBlockTable *pThisBlockTable;
//	acdbHostApplicationServices()->workingDatabase()->
//		getSymbolTable(pThisBlockTable, AcDb::kForRead);
//	AcDbBlockTableRecord *pThisMsBtr;
//	pThisBlockTable->getAt(ACDB_MODEL_SPACE, pThisMsBtr,
//		AcDb::kForWrite);
//	pThisBlockTable->close();
//	AcDbObjectId id = pThisMsBtr->objectId();
//	pThisMsBtr->close();
//	// Create the long transaction. This will check all the entities
//	// out of the external database.
//	//
//	AcDbIdMapping errorMap;
//	acapLongTransactionManagerPtr()->checkOut(transId, objIdArray,
//		id, errorMap);
//	// Now modify the color of these entities.
//	//
//	int colorIndex;
//	acedGetInt(_T("\nEnter color number to change entities to: "),
//		&colorIndex);
//	AcDbObject* pObj;
//	if (acdbOpenObject(pObj, transId, AcDb::kForRead) == Acad::eOk) {
//		// Get a pointer to the transaction.
//		//
//		AcDbLongTransaction* pLongTrans =
//			AcDbLongTransaction::cast(pObj);
//		if (pLongTrans != NULL) {
//			// Get a work set iterator.
//			//
//			AcDbLongTransWorkSetIterator* pWorkSetIter;
//			pLongTrans->newWorkSetIterator(pWorkSetIter);
//			// Iterate over the entities in the work set and change
//			// the color.
//			for (pWorkSetIter->start(); !pWorkSetIter->done();
//				pWorkSetIter->step()) {
//				AcDbEntity *pEntity;
//				acdbOpenAcDbEntity(pEntity, pWorkSetIter->objectId(),
//					AcDb::kForWrite);
//				pEntity->setColorIndex(colorIndex);
//				pEntity->close();
//			}
//			delete pWorkSetIter;
//		}
//		pObj->close();
//	}
//	// Pause to see the change.
//	//
//	TCHAR str[132];
//	acedGetString(0, _T("\nSee the new colors. Press return to check the object into the original database"), str);
//		// Check the entities back in to the original database.
//		//
//		acapLongTransactionManagerPtr()->checkIn(transId, errorMap);
//	// Save the original database, since we have made changes.
//	//
//	pDb->saveAs(fname);
//	// Close/Delete the database
//	//
//	delete pDb;
//	pDb = NULL;
//	acad_free(fname);
//}
////
////////////////////////////////////////////////////////////////////////////////
//
//void printAll();
//
//void makeABlock();
//
//void createPolyline();
//
//void addBlockWithAttributes();
//
//Acad::ErrorStatus addToModelSpace(AcDbObjectId&,
//    AcDbEntity*);
//
//void defineBlockWithAttributes(AcDbObjectId&,
//    const AcGePoint3d&, double, double);
//// THE FOLLOWING CODE APPEARS IN THE SDK DOCUMENT.
//
//void makeABlock()
//{
//     // Create and name a new block table record.
//     //
//     AcDbBlockTableRecord *pBlockTableRec
//         = new AcDbBlockTableRecord();
//     pBlockTableRec->setName(_T("ASDK-NO-ATTR"));
//     // Get the block table.
//     //
//     AcDbBlockTable *pBlockTable = NULL;
//     acdbHostApplicationServices()->workingDatabase()
//        ->getSymbolTable(pBlockTable, AcDb::kForWrite);
//     // Add the new block table record to the block table.
//     //
//     AcDbObjectId blockTableRecordId;
//     pBlockTable->add(blockTableRecordId, pBlockTableRec);
//     pBlockTable->close();
//     // Create and add a line entity to the component's
//     // block record.
//     //
//     AcDbLine *pLine = new AcDbLine();
//     AcDbObjectId lineId;
//     pLine->setStartPoint(AcGePoint3d(3, 3, 0));
//     pLine->setEndPoint(AcGePoint3d(6, 6, 0));
//     pLine->setColorIndex(3);
//     pBlockTableRec->appendAcDbEntity(lineId, pLine);
//     pLine->close();
//     pBlockTableRec->close();
//}
//
//void defineBlockWithAttributes(
//    AcDbObjectId& blockId, // This is a returned value.
//    const AcGePoint3d& basePoint,
//    double textHeight,
//    double textAngle)
//{
//    AcDbBlockTable *pBlockTable = NULL;
//    AcDbBlockTableRecord* pBlockRecord
//       = new AcDbBlockTableRecord;
//    AcDbObjectId entityId;
//    // Step 1: Set the block name and base point of the block definition
//    //
//    pBlockRecord->setName(_T("ASDK-BLOCK-WITH-ATTR"));
//    pBlockRecord->setOrigin(basePoint);
//    // Open the block table for write.
//    //
//    acdbHostApplicationServices()->workingDatabase()
//        ->getSymbolTable(pBlockTable, AcDb::kForWrite);
//    // Step 2: Add the block table record to block table.
//    //
//    if (pBlockTable->add(blockId, pBlockRecord) != Acad::eOk)
//    {
//        delete pBlockRecord;
//        pBlockTable->close();
//        return;
//    }
//    // Step 3: Create a circle entity.
//    //
//    AcDbCircle *pCircle = new AcDbCircle;
//    pCircle->setCenter(basePoint);
//    pCircle->setRadius(textHeight * 4.0);
//    pCircle->setColorIndex(3);
//    // Append the circle entity to the block record.
//    //
//    pBlockRecord->appendAcDbEntity(entityId, pCircle);
//    pCircle->close();
//    // Step 4: Create an attribute definition entity.
//    //
//    AcDbAttributeDefinition *pAttdef
//        = new AcDbAttributeDefinition;
//    // Set the attribute definition values.
//    //
//    pAttdef->setPosition(basePoint);
//    pAttdef->setHeight(textHeight);
//    pAttdef->setRotation(textAngle);
//    // For horizontal modes other than AcDb::kTextLeft
//    // and vertical modes other than AcDb::kTextBase,
//    // you may need to call setAlignmentPoint(). See the
//    // AcDbText::setAlignmentPoint() documentation for details.
//    pAttdef->setHorizontalMode(AcDb::kTextLeft);
//    pAttdef->setVerticalMode(AcDb::kTextBase);
//    pAttdef->setPrompt(_T("Prompt"));
//    pAttdef->setTextString(_T("DEFAULT"));
//    pAttdef->setTag(_T("Tag"));
//    pAttdef->setInvisible(Adesk::kFalse);
//    pAttdef->setVerifiable(Adesk::kFalse);
//    pAttdef->setPreset(Adesk::kFalse);
//    pAttdef->setConstant(Adesk::kFalse);
//    pAttdef->setFieldLength(25);
//    // Append the attribute definition to the block.
//    //
//    pBlockRecord->appendAcDbEntity(entityId, pAttdef);
//    // The second attribute definition is a little easier
//    // because we are cloning the first one.
//    //
//    AcDbAttributeDefinition *pAttdef2
//        = AcDbAttributeDefinition::cast(pAttdef->clone());
//    // Set the values which are specific to the
//    // second attribute definition.
//    //
//    AcGePoint3d tempPt(basePoint);
//    tempPt.y -= pAttdef2->height();
//    pAttdef2->setPosition(tempPt);
//    pAttdef2->setColorIndex(1); // Red
//    pAttdef2->setConstant(Adesk::kTrue);
//    // Append the second attribute definition to the block.
//    //
//    pBlockRecord->appendAcDbEntity(entityId, pAttdef2);
//    pAttdef->close();
//    pAttdef2->close();
//    pBlockRecord->close();
//    pBlockTable->close();
//    return;
//}
//
//void addBlockWithAttributes()
//{
//    // Get an insertion point for the block reference,
//    // definition, and attribute definition.
//    //
//    AcGePoint3d basePoint;
//    if (acedGetPoint(NULL, _T("\nEnter insertion point: "),
//        asDblArray(basePoint)) != RTNORM)
//        return;
//    // Get the rotation angle for the attribute definition.
//    //
//    double textAngle;
//    if (acedGetAngle(asDblArray(basePoint),
//        _T("\nEnter rotation angle: "), &textAngle) != RTNORM)
//        return;
//    // Define the height used for the attribute definition text.
//    //
//    double textHeight;
//    if (acedGetDist(asDblArray(basePoint),
//        _T("\nEnter text height: "), &textHeight) != RTNORM)
//        return;
//    // Build the block definition to be inserted.
//    //
//    AcDbObjectId blockId;
//    defineBlockWithAttributes(blockId, basePoint, textHeight, textAngle);
//    if (blockId.isNull()) {
//        return;
//    }
//    // Step 1: Allocate a block reference object.
//    //
//    AcDbBlockReference *pBlkRef = new AcDbBlockReference;
//    // Step 2: Set up the block reference to the newly
//    // created block definition.
//    //
//    pBlkRef->setBlockTableRecord(blockId);
//    // Give it the current UCS normal.
//    //
//    resbuf to, from;
//    from.restype = RTSHORT;
//    from.resval.rint = 1; // UCS
//    to.restype = RTSHORT;
//    to.resval.rint = 0; // WCS
//    AcGeVector3d normal(0.0, 0.0, 1.0);
//    acedTrans(&(normal.x), &from, &to, Adesk::kTrue,
//        &(normal.x));
//    // Set the insertion point for the block reference.
//    //
//    pBlkRef->setPosition(basePoint);
//    // Indicate the LCS 0.0 angle, not necessarily the UCS 0.0 angle.
//    //
//    pBlkRef->setRotation(0.0);
//    pBlkRef->setNormal(normal);
//    // Step 3: Open the current database's model space
//    // block Table Record.
//    //
//    AcDbBlockTable *pBlockTable;
//    acdbHostApplicationServices()->workingDatabase()
//        ->getSymbolTable(pBlockTable, AcDb::kForRead);
//    AcDbBlockTableRecord *pBlockTableRecord;
//    pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord,
//        AcDb::kForWrite);
//    pBlockTable->close();
//    // Append the block reference to the model space
//    // block Table Record.
//    //
//    AcDbObjectId newEntId;
//    pBlockTableRecord->appendAcDbEntity(newEntId, pBlkRef);
//    pBlockTableRecord->close();
//    // Step 4: Open the block definition for read.
//    //
//    AcDbBlockTableRecord *pBlockDef;
//    acdbOpenObject(pBlockDef, blockId, AcDb::kForRead);
//    // Set up a block table record iterator to iterate
//    // over the attribute definitions.
//    //
//    AcDbBlockTableRecordIterator *pIterator;
//    pBlockDef->newIterator(pIterator);
//    AcDbEntity *pEnt;
//    AcDbAttributeDefinition *pAttdef;
//    for (pIterator->start(); !pIterator->done();
//        pIterator->step())
//    {
//        // Get the next entity.
//        //
//        pIterator->getEntity(pEnt, AcDb::kForRead);
//        // Make sure the entity is an attribute definition
//        // and not a constant.
//        //
//        pAttdef = AcDbAttributeDefinition::cast(pEnt);
//        if (pAttdef != NULL && !pAttdef->isConstant()) {
//            // We have a non-constant attribute definition,
//            // so build an attribute entity.
//            //
//            AcDbAttribute *pAtt = new AcDbAttribute();
//            pAtt->setPropertiesFrom(pAttdef);
//            pAtt->setInvisible(pAttdef->isInvisible());
//            // Translate the attribute by block reference.
//            // To be really correct, the entire block
//            // reference transform should be applied here.
//            //
//            basePoint = pAttdef->position();
//            basePoint += pBlkRef->position().asVector();
//            pAtt->setPosition(basePoint);
//            pAtt->setHeight(pAttdef->height());
//            pAtt->setRotation(pAttdef->rotation());
//            pAtt->setTag(_T("Tag"));
//            pAtt->setFieldLength(25);
//            const TCHAR *pStr = pAttdef->tagConst();
//            pAtt->setTag(pStr);
//            pAtt->setFieldLength(pAttdef->fieldLength());
//            // The database column value should be displayed.
//            // INSERT prompts for this.
//            //
//            pAtt->setTextString(_T("Assigned Attribute Value"), p);
//            AcDbObjectId attId;
//            pBlkRef->appendAttribute(attId, pAtt);
//            pAtt->close();
//        }
//        pEnt->close(); // use pEnt... pAttdef might be NULL
//    }
//    delete pIterator;
//    pBlockDef->close();
//    pBlkRef->close();
//}
//
//void
//
//printAll()
//{
//    int rc;
//    TCHAR blkName[50];
//    *blkName = _T('\0');
//    rc = acedGetString(Adesk::kTrue,
//        _T("Enter Block Name <hit <ENTER> for current space>: "),
//        blkName);
//    if (rc != RTNORM)
//        return;
//    if (blkName[0] == _T('\0')) {
//        if (acdbHostApplicationServices()->workingDatabase()->tilemode()
//            == Adesk::kFalse) {
//            resbuf rb;
//            acedGetVar(_T("cvport"), &rb);
//            if (rb.resval.rint == 1) {
//                _tcscpy(blkName, ACDB_PAPER_SPACE);
//            } else {
//                _tcscpy(blkName, ACDB_MODEL_SPACE);
//            }
//        } else {
//            _tcscpy(blkName, ACDB_MODEL_SPACE);
//        }
//    }
//    AcDbBlockTable *pBlockTable;
//    acdbHostApplicationServices()->workingDatabase()
//        ->getSymbolTable(pBlockTable, AcDb::kForRead);
//    AcDbBlockTableRecord *pBlockTableRecord;
//    Acad::ErrorStatus es = pBlockTable->getAt(blkName, pBlockTableRecord,
//        AcDb::kForRead);
//    pBlockTable->close();
//    if (es != Acad::eOk)
//        return;
//    AcDbBlockTableRecordIterator *pBlockIterator;
//    pBlockTableRecord->newIterator(pBlockIterator);
//    for (; !pBlockIterator->done();
//        pBlockIterator->step())
//    {
//        AcDbEntity *pEntity;
//        pBlockIterator->getEntity(pEntity, AcDb::kForRead);
//        AcDbHandle objHandle;
//        pEntity->getAcDbHandle(objHandle);
//        TCHAR handleStr[20];
//        objHandle.getIntoAsciiBuffer(handleStr);
//        const TCHAR *pCname = pEntity->isA()->name();
//        acutPrintf(_T("Object Id %lx, handle %s, class %s.\n"),
//            pEntity->objectId().asOldId(), handleStr, pCname);
//        pEntity->close();
//    }
//    delete pBlockIterator;
//    pBlockTableRecord->close();
//    acutPrintf(_T("\n"));
//}
//
//void
//
//createPolyline()
//{
//    // Set four vertex locations for the pline.
//    //
//    AcGePoint3dArray ptArr;
//    ptArr.setLogicalLength(4);
//    for (int i = 0; i < 4; i++) {
//        ptArr[i].set((double)(i/2), (double)(i%2), 0.0);
//    }
//    // Dynamically allocate an AcDb2dPolyline object,
//    // given four vertex elements whose locations are supplied
//    // in ptArr.  The polyline has no elevation, and is
//    // explicitly set as closed.  The polyline is simple;
//    // that is, not curve fit or a spline.  By default, the
//    // widths are all 0.0 and there are no bulge factors.
//    //
//    AcDb2dPolyline *pNewPline = new AcDb2dPolyline(
//        AcDb::k2dSimplePoly, ptArr, 0.0, Adesk::kTrue);
//    pNewPline->setColorIndex(3);
//    // Get a pointer to a Block Table object.
//    //
//    AcDbBlockTable *pBlockTable;
//    acdbHostApplicationServices()->workingDatabase()
//        ->getSymbolTable(pBlockTable, AcDb::kForRead);
//    // Get a pointer to the MODEL_SPACE BlockTableRecord.
//    //
//    AcDbBlockTableRecord *pBlockTableRecord;
//    pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord,
//        AcDb::kForWrite);
//    pBlockTable->close();
//    // Append the pline object to the database and
//    // obtain its Object ID.
//    //
//    AcDbObjectId plineObjId;
//    pBlockTableRecord->appendAcDbEntity(plineObjId,
//        pNewPline);
//    pBlockTableRecord->close();
//    // Make the pline object reside on layer "0".
//    //
//    pNewPline->setLayer(_T("0"));
//    pNewPline->close();
//}
//// END CODE APPEARING IN SDK DOCUMENT.
//
//Acad::ErrorStatus addToModelSpace(AcDbObjectId &objId, AcDbEntity* pEntity)
//{
//    Acad::ErrorStatus     es = Acad::eOk;
//    AcDbBlockTable        *pBlockTable;
//    AcDbBlockTableRecord  *pSpaceRecord;
//    acdbHostApplicationServices()->workingDatabase()
//        ->getSymbolTable(pBlockTable, AcDb::kForRead);
//    es = pBlockTable->getAt(ACDB_MODEL_SPACE, pSpaceRecord,
//        AcDb::kForWrite);
//	if (!pSpaceRecord) {
//		pBlockTable->close();
//		return es;
//	}
//    pSpaceRecord->appendAcDbEntity(objId, pEntity);
//    pBlockTable->close();
//    pEntity->close();
//    pSpaceRecord->close();
//    return es;
//}
//
///////////////////////////////////////////////////////////////////////////////////////////
//
//// Create a table template from a table, and create a table style to hold the template.
//void createTemplate()
//{
//	// Select a table
//	ErrorStatus es;
//	AcDbTable *pTbl = NULL;
//	if (NULL == (pTbl = AcDbTable::cast(ArxDbgUtils::selectEntity(_T("\nSelect a table: ")))))
//		acutPrintf(ACRX_T("\nSelected entity was not a table!"));
//	static ACHAR sNameOfMyTableTemplate[MAX_PATH] = ACRX_T("MyTableTemplate");
//	static ACHAR sNameOfMyTableStyle[MAX_PATH] = ACRX_T("MyTableStyle");
//	AcDbTableTemplate* pTblTpl = new AcDbTableTemplate();
//	// We skip its content here.
//	es = pTblTpl->capture(pTbl, AcDb::kTableCopySkipContent);
//	es = pTblTpl->setName(sNameOfMyTableTemplate);
//	AcDbTableStyle* pTblSty = new AcDbTableStyle();
//	AcDbDatabase *pDb = acdbHostApplicationServices()->workingDatabase();
//	assert(pDb);
//	// If a style with the name is already in the TableStyleDictionary, remove it.
//	AcDbDictionary *pDict = NULL;
//	es = acdbHostApplicationServices()->workingDatabase()->getTableStyleDictionary(pDict, AcDb::kForRead);
//	// Check if the Table Style is already there.
//	if (pDict->has(sNameOfMyTableStyle))
//	{
//		es = pDict->upgradeOpen();
//		es = pDict->remove(sNameOfMyTableStyle);
//	}
//	// Post the table style to DB and then set the table template to it.
//	AcDbObjectId idTS;
//	es = pDict->upgradeOpen();
//	if (Acad::eOk != (es = pDict->setAt(sNameOfMyTableStyle, pTblSty, idTS)))
//	{
//		pDict->close();
//		delete pTblSty;
//		delete pTblTpl;
//		es = pTbl->close();
//		acutPrintf(ACRX_T("\nUnable to add new Table Style"));
//		return;
//	}
//	es = pDict->close();
//	assert(es == Acad::eOk);
//	// Set the new template to the table.
//	AcDbObjectId id;
//	es = pTblSty->setTemplate(pTblTpl, AcDb::kMergeCellStyleNone, id);
//	if (es != Acad::eOk)
//	{
//		delete pTblTpl;
//		pTblSty->close();
//		pTbl->close();
//		acutPrintf(ACRX_T("\nError in setting data table template!"));
//		return;
//	}
//	// Clean up.
//	es = pTblSty->close();
//	es = pTblTpl->close();
//	es = pTbl->close();
//}
//
//// Create a data link and set it to the cell(2,2) of a selected table.
//void createAndSetDataLink()
//{
//	Acad::ErrorStatus es;
//	AcDbObjectId idDL;
//	AcDbDataLink *pDL = NULL;
//
//	AcApDocument* pDoc = acDocManager->curDocument();
//	if(!pDoc)
//		return;
//
//	es = acDocManager->lockDocument(pDoc, AcAp::kAutoWrite, NULL, NULL, false);
//	// Select a table
//	AcDbTable *pTbl = NULL;
//	if (NULL == (pTbl =
//		AcDbTable::cast(ArxDbgUtils::selectEntity(_T("\nSelect a table: "), AcDb::kForWrite))))
//	{
//		acutPrintf(ACRX_T("\nSelected entity was not a table!"));
//		goto ret;
//	}
//	// Get an Excel file
//
//	struct resbuf *result;
//	int rc;
//	if ((result = acutNewRb(RTSTR)) == NULL)
//	{
//		pTbl->close();
//		acutPrintf(ACRX_T("\nUnable to allocate buffer!"));
//		return;
//	}
//	result->resval.rstring = NULL;
//	rc = acedGetFileD(ACRX_T("Excel File"),	// Title
//		0/*ACRX_T("c:/temp")*/,	// Default pathname
//		ACRX_T("xls;xlsx"),	//Default extension
//		16,					// Control flags
//		result);	// The path selected by the user.
//	if (rc != RTNORM)
//	{
//		pTbl->close();
//		acutPrintf(ACRX_T("\nError in selecting an EXCEL file!"));
//		return;
//	}
//	// Retrieve the file name from the ResBuf.
//	ACHAR fileName[MAX_PATH] = _T("");
//	//_tcscpy(fileName, result->resval.rstring);
//	//rc = acutRelRb(result);
//	static ACHAR sMyDataLink[MAX_PATH] = ACRX_T("MyDataLinkTest");
//	// Get the Data Link Manager.
//	AcDbDataLinkManager* pDlMan = acdbHostApplicationServices()->workingDatabase()->getDataLinkManager();
//	assert(pDlMan);
//	// Check if a Data Link with the name already exists. If so, remove it.
//	if (pDlMan->getDataLink(sMyDataLink, pDL, AcDb::kForRead) == Acad::eOk && pDL)
//	{
//		pDL->close();
//		es = pDlMan->removeDataLink(sMyDataLink, idDL);
//		if (es != Acad::eOk)
//		{
//			pTbl->close();
//			acutPrintf(ACRX_T("\nError in removing the Data Link!"));
//			goto ret;
//		}
//	}
//
//	// Create the Data Link with the name.
//	es = pDlMan->createDataLink(ACRX_T("AcExcel"), sMyDataLink, ACRX_T("This is a test for Excel type data link."), fileName, idDL);
//	if (es != Acad::eOk)
//	{
//		pTbl->close();
//		acutPrintf(ACRX_T("\nError in creating Data Link!\nPlease check if there is a sheet named 'Sheet1' in the XLS file."));
//		goto ret;
//	}
//	// Open the Data Link.
//	es = acdbOpenObject<AcDbDataLink>(pDL, idDL, AcDb::kForWrite);
//	if (es != Acad::eOk || !pDL)
//	{
//		pTbl->close();
//		acutPrintf(ACRX_T("\nError in opening the Data Link object!"));
//		goto ret;
//	}
//	//  Set options of the Data Link
//	es = pDL->setOption(AcDb::kDataLinkOptionPersistCache);
//	es = pDL->setUpdateOption(pDL->updateOption() | AcDb::kUpdateOptionAllowSourceUpdate);
//
//	// Close the Data Link.
//	es = pDL->close();
//	// Set data link to the table object at cell(2,2).
//	es = pTbl->setDataLink(0, 0, idDL, true);
//	if (es != Acad::eOk)
//	{
//		pTbl->close();
//		acutPrintf(ACRX_T("\nError in setting Data Link to the selected table!\nPlease check if there is a sheet named 'Sheet1' in the XLS file."));
//		goto ret;
//	}
//	// Don't forget to close the table object.
//	es = pTbl->close();
//ret:
//	acDocManager->unlockDocument(pDoc);
//}
//
///////////////////////////////////////////////////////////////////////////////////////////
//int nti_arx_blocks()
//{
//	AcDbBlockTable *pBlockTable = 0;
//	AcDbBlockTableRecord *pBlkTblRcd = 0;
//
//	acdbHostApplicationServices()->workingDatabase()->getSymbolTable(pBlockTable, AcDb::kForRead);
//	pBlockTable->getAt(ACDB_MODEL_SPACE, pBlkTblRcd, AcDb::kForRead);
//
//	AcDbBlockReferenceIdIterator * iter = 0;
//	pBlkTblRcd->newBlockReferenceIdIterator(iter);
//
//	for (; !iter->done(); iter->step()) {
//		AcDbBlockReference * pBlkRef = 0;
//		iter->getBlockReference(pBlkRef, AcDb::kForRead);
//		AcDbHandle objHandle;
//		pBlkRef->getAcDbHandle(objHandle);
//		TCHAR handleStr[20];
//		objHandle.getIntoAsciiBuffer(handleStr);
//		const TCHAR *pCname = pBlkRef->isA()->name();
//		acutPrintf(_T("name='%s', Object Id %lx, handle %s, class %s.\n"),
//			  _T(""), pBlkRef->objectId().asOldId(), handleStr, pCname);
//
//	    pBlkRef->close();
//	}
//
//	return 0;
//}
//
//int nti_insert_table()
//{
//	AcDbTable* pTable = new AcDbTable;
//	pTable->setSize(5, 6);
//	pTable->setValue(0, 0, _T("hello"));
//
//	AcDbObjectId id;
//	Acad::ErrorStatus rc = addToModelSpace(id, pTable);
//
//	return 0;
//}
//
//int nti_arx_update_datalinks(list * links)
//{
//	if(!links)
//		return -1;
//
//	while (listFirst(links))
//		listDelNode(links, listFirst(links));
//
//	int i, rc;
//	Acad::ErrorStatus es;
//
//	AcDbDataLinkManager* pDlMan = acdbHostApplicationServices()->workingDatabase()->getDataLinkManager();
//	assert(pDlMan);
//	AcDbObjectIdArray datalinks;
//	rc = pDlMan->getDataLink(datalinks);
//
//	for (i = 0; i < datalinks.length(); ++i) {
//		AcDbDataLink *pDL = NULL;
//		es = acdbOpenObject<AcDbDataLink>(pDL, datalinks[i], AcDb::kForRead);
//		if (!pDL)
//			continue;
//
//		nti_datalink * link = nti_new(nti_datalink);
//		link->name = pDL->name();
//		link->desc = pDL->description();
//		link->conn = pDL->connectionString();
//
//		listAddNodeTail(links, link);
//
//		pDL->close();
//	}
//
//	return rc;
//}

#ifndef NDEBUG
int test_nti_arx_main(int argc, char ** argv)
{
	int rc; 

	rc = nti_foreach_symtbl(nullptr, nullptr); assert((rc != 0));

	return rc;
}
#endif //NDEBUG
