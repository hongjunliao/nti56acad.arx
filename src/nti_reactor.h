/*!
 * This file is PART of nti56acad.arx project
 * @author hongjun.liao <docici@126.com>, @date 2020/09/15
 *
 * arx reactor
 * */

#ifndef NTI_REACTOR_H
#define NTI_REACTOR_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "stdafx.h"
/////////////////////////////////////////////////////////////////////////////////////

class CDbModReactor : public AcDbDatabaseReactor
{
public:
	virtual void objectModified(const AcDbDatabase* pDb,
								const AcDbObject* pDbObj);
	virtual void objectErased(	const AcDbDatabase* pDb,
								const AcDbObject* pDbObj,
								Adesk::Boolean bErased = Adesk::kTrue);

};

class CDocReactor : public AcApDocManagerReactor
{
public:
    virtual void  documentCreated(AcApDocument* pDoc);
	virtual void  documentActivated(AcApDocument* pDoc);
    virtual void  documentToBeDestroyed(AcApDocument* pDoc);
    virtual void  documentToBeDeactivated(AcApDocument* pDoc);
    virtual void  documentToBeActivated(AcApDocument* pDoc);
};

class CEdReactor : public AcEditorReactor
{
public:
    virtual void commandEnded(const TCHAR *cmdStr);
	virtual void commandWillStart(const TCHAR *cmdStr);
};


void attachDbReactor(AcDbDatabase* pDb);
/////////////////////////////////////////////////////////////////////////////////////////
#ifndef NDEBUG
#endif //NDEBUG

#endif //NTI_REACTOR_H