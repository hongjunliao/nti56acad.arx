/* DockWnd.h */

#ifndef DOCKWINDOW_INCLUDED
#define DOCKWINDOW_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

typedef LRESULT WINAPI DockMsgFunc(struct _DOCKINFO *, UINT, WPARAM, LPARAM);
typedef void WINAPI DockResizeFunc(struct _DOCKINFO *, RECT *);
typedef void WINAPI DockDestroyFunc(struct _DOCKINFO *);
typedef DWORD WINAPI DockCloseFunc(struct _DOCKINFO *);

#pragma pack(1)
typedef struct {
	UINT			count;
	HWND			container;
} DOCKCOUNTPARAMS;
#pragma pack()

#pragma pack(1)
typedef struct {
	HDWP			hdwp;
	RECT			*rect;
	unsigned char	which;
} DOCKLAYOUTPARAMS;
#pragma pack()

#pragma pack(1)
typedef struct {
	HWND		container;	/* Handle to container window. */
	HWND		hwnd;		/* Handle to window which received WM_NCACTIVATE or WM_ENABLE (can
							 * be the container, or some Docking Frame window).
							 */
	WPARAM		wParam;		/* WPARAM of the WM_NCACTIVATE/WM_ENABLE message. */
	LPARAM		lParam;		/* LPARAM of the WM_NCACTIVATE/WM_ENABLE message. */
} DOCKPARAMS;
#pragma pack()

#pragma pack(1)
typedef struct _DOCKINFO
{
	/* For application's use */
	void			*userdata;

	/* Various bits, see below. */
	DWORD			dwStyle;

	/* XY position (relative to screen) of Docking Frame window when floating. */
	unsigned short	xpos;
	unsigned short	ypos;
	/* Width/Height of client area of Docking Frame window when floating. */
	unsigned short	cxFloating;
	unsigned short	cyFloating;

	/* Width or height of window when docked. */
	unsigned short	nDockedSize;

	/* HWND of the window to set the focus whenever the user completes an operation with
	 * the Docking Frame. If 0, no focus is set. This is also the window that is resized
	 * to fill the client area if no DockResize callback is specified.
	 */
	HWND			focusWindow;

	/* Callback for messages received by the Docking frame but not handled by it.
	 * 0 if not needed.
	 */
	DockMsgFunc		*DockMsg;

	/* Callback to resize the contents of the Docking Frame's client area. */
	DockResizeFunc	*DockResize;

	/* Callback to query if the app wants to allow the Docking Frame to be closed. */
	DockCloseFunc	*DockClose;

	/* Callback to notify the app that this DockInfo is no longer in service. */
	DockDestroyFunc	*DockDestroy;

	/* The following fields are initialized/used by the DockWnd library, so do not modify these. */

	/* Handle to the Docking Frame (ie, tool window). */
	HWND			hwnd;

	/* Handle to the container (ie, owner) window. */
	HWND			container;

	/* Docking Frame window width/height when floating. */
	unsigned short	nFrameWidth;
	unsigned short	nFrameHeight;

	/* Indicates to which side of the container window the Docking Frame is docked (if any). */
	char			uDockedState;
} DOCKINFO;
#pragma pack()

//	DOCKINFO dwStyle
#define DWS_ALLOW_DOCKLEFT	   0x01	// Allow the Docking Frame to be docked to the left side of container window.
#define DWS_ALLOW_DOCKRIGHT	   0x02
#define DWS_ALLOW_DOCKTOP	   0x04
#define DWS_ALLOW_DOCKBOTTOM   0x08
#define DWS_DRAWGRIPPERDOCKED	0x00000010	// Draw a gripper when docked.
#define DWS_DRAWGRIPPERFLOATING	0x00000020	// Draw a gripper when floating.
#define DWS_KEEPORIGSTATE		0x00000040	// Force the Docking Frame to always stay docked or floating as it was originally created.
#define DWS_NORESIZE			0x00000080	// Prevent user resizing the Docking Frame.
#define DWS_DONTSAVEPOS			0x00000100	// Don't save changed position/size. This is primarily set only by the docking library.
#define DWS_NODESTROY			0x00000200	// Hides the Docking Frame window instead of destroying it.
#define DWS_NODISABLE			0x00000400	// Does not disable the Docking Frame window when DockingEnable() is called, and the window is floating.
#define DWS_FREEFLOAT			0x00000800	// Docking Frame window can float behind the container.

#define DWS_HIDDEN				0x80000000	// Used only by docking library.
#define DWS_ALLOW_DOCKALL		(DWS_ALLOW_DOCKLEFT|DWS_ALLOW_DOCKBOTTOM|DWS_ALLOW_DOCKRIGHT|DWS_ALLOW_DOCKTOP)

// DOCKINFO uDockedState
#define DWS_FLOATING			0x80
#define DWS_DOCKED_LEFT			DWS_ALLOW_DOCKLEFT
#define DWS_DOCKED_RIGHT		DWS_ALLOW_DOCKRIGHT
#define DWS_DOCKED_TOP			DWS_ALLOW_DOCKTOP
#define DWS_DOCKED_BOTTOM		DWS_ALLOW_DOCKBOTTOM

// Function declarations
extern void WINAPI 			DockingUnInitialize();
extern ULONG WINAPI			DockingInitialize(void * param);
extern UINT WINAPI 			DockingCountFrames(HWND, UINT);
extern void WINAPI 			DockingArrangeWindows(HWND, HDWP, RECT *);
extern DOCKINFO * WINAPI	DockingAlloc(char);
extern void WINAPI			DockingFree(DOCKINFO *);
extern HWND WINAPI			DockingCreateFrame(DOCKINFO *, HWND, LPCTSTR szCaption);
extern void WINAPI			DockingShowFrame(DOCKINFO *);
extern LRESULT WINAPI		DockingEnable(DOCKPARAMS *);
extern LRESULT WINAPI		DockingActivate(DOCKPARAMS *);
extern void WINAPI			DockingLoadPlacement(DOCKINFO *, HKEY);
extern DWORD WINAPI			DockingSavePlacement(DOCKINFO *, HKEY);
extern void WINAPI			DockingUpdateLayout(HWND);
extern void WINAPI			DockingRedrawFrame(DOCKINFO *);
extern DOCKINFO * WINAPI		DockingIsToolWindow(HWND, HWND);
extern void WINAPI			DockingDestroyFreeFloat(HWND);

#ifdef __cplusplus
}
#endif

#endif
