/*!
 * This file is PART of nti56acad.arx project
 * @author hongjun.liao <docici@126.com>, @date 2020/09/15
 *
 * arx
 * */

#ifndef NTI_ARX_H
#define NTI_ARX_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#ifdef NTI56_ARX
#include "hp/hp_tuple.h" //hp_tuple2_t
#include <functional>    //std::function
/////////////////////////////////////////////////////////////////////////////////////
int nti_foreach_symtbl(
	std::function<int(AcDbDatabase*& db, AcDbSymbolTable*& tbl)> on_get
	, std::function<int(AcDbSymbolTableRecord*& tblRec)> on_each);
#endif //#ifdef NTI56_ARX
/////////////////////////////////////////////////////////////////////////////////////
hp_tuple2_t(nti_prop_t, CStringList /*field*/, CStringList /*value*/);
// derived from AcDbObject
void nti_getprop(AcDbObject* obj, nti_prop_t& p);
void nti_getprop(AcDbDictionary* obj, nti_prop_t& p);
void nti_getprop(AcDbEntity* obj, nti_prop_t& p);
void nti_getprop(AcDbFilter* obj, nti_prop_t& p);
void nti_getprop(AcDbGroup* obj, nti_prop_t& p);
void nti_getprop(AcDbIndex* obj, nti_prop_t& p);
void nti_getprop(AcDbLongTransaction* obj, nti_prop_t& p);
void nti_getprop(AcDbMlineStyle* obj, nti_prop_t& p);
void nti_getprop(AcDbPlaceHolder* obj, nti_prop_t& p);
void nti_getprop(AcDbPlotSettings* obj, nti_prop_t& p);
void nti_getprop(AcDbProxyObject* obj, nti_prop_t& p);
void nti_getprop(AcDbSymbolTable* obj, nti_prop_t& p);
void nti_getprop(AcDbSymbolTableRecord* obj, nti_prop_t& p);
void nti_getprop(AcDbXrecord* obj, nti_prop_t& p);

// derived from AcDbDictionary
void nti_getprop(AcDbDictionaryWithDefault* obj, nti_prop_t& p);

// derived form AcDbEntity
void nti_getprop(AcDb3dSolid* obj, nti_prop_t& p);
void nti_getprop(AcDbBlockBegin* obj, nti_prop_t& p);
void nti_getprop(AcDbBlockEnd* obj, nti_prop_t& p);
void nti_getprop(AcDbBlockReference* obj, nti_prop_t& p);
void nti_getprop(AcDbBody* obj, nti_prop_t& p);
void nti_getprop(AcDbCurve* obj, nti_prop_t& p);
void nti_getprop(AcDbDimension* obj, nti_prop_t& p);
void nti_getprop(AcDbFace* obj, nti_prop_t& p);
void nti_getprop(AcDbFaceRecord* obj, nti_prop_t& p);
void nti_getprop(AcDbFcf* obj, nti_prop_t& p);
void nti_getprop(AcDbFrame* obj, nti_prop_t& p);
void nti_getprop(AcDbHatch* obj, nti_prop_t& p);
void nti_getprop(AcDbImage* obj, nti_prop_t& p);
void nti_getprop(AcDbMline* obj, nti_prop_t& p);
void nti_getprop(AcDbMText* obj, nti_prop_t& p);
void nti_getprop(AcDbPoint* obj, nti_prop_t& p);
void nti_getprop(AcDbPolyFaceMesh* obj, nti_prop_t& p);
void nti_getprop(AcDbPolygonMesh* obj, nti_prop_t& p);
void nti_getprop(AcDbProxyEntity* obj, nti_prop_t& p);
void nti_getprop(AcDbRegion* obj, nti_prop_t& p);
void nti_getprop(AcDbSequenceEnd* obj, nti_prop_t& p);
void nti_getprop(AcDbShape* obj, nti_prop_t& p);
void nti_getprop(AcDbSolid* obj, nti_prop_t& p);
void nti_getprop(AcDbText* obj, nti_prop_t& p);
void nti_getprop(AcDbTrace* obj, nti_prop_t& p);
void nti_getprop(AcDbVertex* obj, nti_prop_t& p);
void nti_getprop(AcDbViewport* obj, nti_prop_t& p);

// derived from AcDbBlockReference
void nti_getprop(AcDbMInsertBlock* obj, nti_prop_t& p);

// derived from AcDbCurve
void nti_getprop(AcDb2dPolyline* obj, nti_prop_t& p);
void nti_getprop(AcDb3dPolyline* obj, nti_prop_t& p);
void nti_getprop(AcDbArc* obj, nti_prop_t& p);
void nti_getprop(AcDbCircle* obj, nti_prop_t& p);
void nti_getprop(AcDbEllipse* obj, nti_prop_t& p);
void nti_getprop(AcDbLeader* obj, nti_prop_t& p);
void nti_getprop(AcDbLine* obj, nti_prop_t& p);
void nti_getprop(AcDbPolyline* obj, nti_prop_t& p);
void nti_getprop(AcDbRay* obj, nti_prop_t& p);
void nti_getprop(AcDbSpline* obj, nti_prop_t& p);
void nti_getprop(AcDbXline* obj, nti_prop_t& p);

// derived from AcDbDimension
void nti_getprop(AcDb2LineAngularDimension* obj, nti_prop_t& p);
void nti_getprop(AcDb3PointAngularDimension* obj, nti_prop_t& p);
void nti_getprop(AcDbAlignedDimension* obj, nti_prop_t& p);
void nti_getprop(AcDbDiametricDimension* obj, nti_prop_t& p);
void nti_getprop(AcDbOrdinateDimension* obj, nti_prop_t& p);
void nti_getprop(AcDbRadialDimension* obj, nti_prop_t& p);
void nti_getprop(AcDbRotatedDimension* obj, nti_prop_t& p);

// derived from AcDbFrame
void nti_getprop(AcDbOleFrame* obj, nti_prop_t& p);
void nti_getprop(AcDbOle2Frame* obj, nti_prop_t& p);

// derived from AcDbText
void nti_getprop(AcDbAttribute* obj, nti_prop_t& p);
void nti_getprop(AcDbAttributeDefinition* obj, nti_prop_t& p);

// derived from AcDbVertex
void nti_getprop(AcDb2dVertex* obj, nti_prop_t& p);
void nti_getprop(AcDb3dPolylineVertex* obj, nti_prop_t& p);
void nti_getprop(AcDbPolyFaceMeshVertex* obj, nti_prop_t& p);
void nti_getprop(AcDbPolygonMeshVertex* obj, nti_prop_t& p);

// derived from AcDbFilter
void nti_getprop(AcDbLayerFilter* obj, nti_prop_t& p);
void nti_getprop(AcDbSpatialFilter* obj, nti_prop_t& p);

// derived from AcDbIndex
void nti_getprop(AcDbLayerIndex* obj, nti_prop_t& p);
void nti_getprop(AcDbSpatialIndex* obj, nti_prop_t& p);

// derived from AcDbPlotSettings
void nti_getprop(AcDbLayout* obj, nti_prop_t& p);

// derived from AcDbSymbolTableRecord
void nti_getprop(AcDbAbstractViewTableRecord* tbl, nti_prop_t& p);
void nti_getprop(AcDbBlockTableRecord* tbl, nti_prop_t& p);
void nti_getprop(AcDbDimStyleTableRecord* tbl, nti_prop_t& p);
void nti_getprop(AcDbLayerTableRecord* tbl, nti_prop_t& p);
void nti_getprop(AcDbLinetypeTableRecord* tbl, nti_prop_t& p);
void nti_getprop(AcDbRegAppTableRecord* tbl, nti_prop_t& p);
void nti_getprop(AcDbTextStyleTableRecord* tbl, nti_prop_t& p);
void nti_getprop(AcDbUCSTableRecord* tbl, nti_prop_t& p);

// derived from AcDbAbstractViewTableRecord
void nti_getprop(AcDbViewTableRecord* tbl, nti_prop_t& p);
void nti_getprop(AcDbViewportTableRecord* tbl, nti_prop_t& p);

/////////////////////////////////////////////////////////////////////////////////////
#ifndef NDEBUG
int test_nti_arx_main(int argc, char ** argv);
#endif //NDEBUG

#endif //NTI_ARX_H
