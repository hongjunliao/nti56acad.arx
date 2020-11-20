/*!
* This file is PART of nti56acad.arx project
* @author hongjun.liao <docici@126.com>, @date 2020/09/15
*
* */
//////////////////////////////////////////////////////////////////////////////
//

#ifndef ARXDBGEDUICONTEXT_H
#define ARXDBGEDUICONTEXT_H

/**************************************************************************
**
**  CLASS nti_EdUiContextApp
**
**  **jma
**
*************************************/

class nti_EdUiContextApp : public AcEdUIContext {

public:
				nti_EdUiContextApp();
	virtual		~nti_EdUiContextApp();

    //ACRX_DECLARE_MEMBERS(nti_EdUiContextApp);

    virtual void*	getMenuContext(const AcRxClass* classtype, const AcDbObjectIdArray& selectedObjs);
    virtual void	onCommand(Adesk::UInt32 cmdId);

	bool			isValid() const { return (m_cMenu != NULL); }

private:
		// data members
    CMenu*		m_cMenu;
    HMENU		m_hMenu;
};

#endif // ARXDBGEDUICONTEXT_H