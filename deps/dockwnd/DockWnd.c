/* DockWnd.c
 *
 * A Docking Window library
 *
 * Copyright J Brown 2001
 * Freeware
 *
 * Modified by Jeff Glatt -- Jan 2005
 *
 * Notes:
 *
 * Best viewed if you set your editor TAB width to 3.
 *
 * All functions called only internally begin with a small letter. All functions
 * callable by an application begine with a capital letter.
 *
 * All global variables begin with a capital letter. All local variables, or
 * arguments passed on the stack, begin with a small letter.
 */

#include <windows.h>
#include <WinUser.h>
#include <tchar.h>
#include "DockWnd.h"

// Handle to this DLL
static HINSTANCE		ThisModule;

// ATOM returned from RegisterWindowEx() for our docking frames
static ATOM				DockingFrameAtom;

// Used for dragging a Docking Frame. Since only 1 window
// can be dragged at a time, these can be global
static HHOOK			OrigDockHookProc;
static HWND				DraggingDockWnd;
static HWND				SizingDockWnd;
static RECT				DragRecPlacement;
static POINTS			MousePos;
static unsigned char	ControlKeyDown;
static unsigned char	DragRecType;
static unsigned char	MouseMoved;

static LRESULT  ( * g_wndproc)(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) = 0;

#define POPUP_STYLES   (WS_POPUP|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_SYSMENU|WS_CAPTION|WS_THICKFRAME)
#define POPUP_EXSTYLES (WS_EX_TOOLWINDOW|WS_EX_WINDOWEDGE)
#define CHILD_STYLES   (WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_DLGFRAME)
#define CHILD_EXSTYLES (0)

// ********************* Global variables that are shared among all processes *********************
#pragma data_seg("Shared")

// Used by drawDragFrame() to draw the drag rectangle
static const WORD DotPatternBmp1[] = 
{
	0x00aa, 0x0055, 0x00aa, 0x0055, 0x00aa, 0x0055, 0x00aa, 0x0055
	//0xaaaa, 0x5555, 0xaaaa, 0x5555, 0xaaaa, 0x5555, 0xaaaa, 0x5555
};
static const WORD DotPatternBmp2[] = 
{
	0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff
};

// The class name for the Docking Frame window
static const TCHAR		DockClassName[] = _T("DockWnd32");

// Name of key for the "Place" setting
static const TCHAR PlaceKey[] = _T("Place");

// Name of key for the "State" setting
static const TCHAR StateKey[] = _T("State");

#pragma data_seg()







/********************* calcFloatingSize() ********************
 * Works out how big a floating docking frame window should be,
 * taking into account the client area, and window styles.
 *
 * hwnd =	Handle of the Docking Frame window.
 * dwp =	DOCKINFO struct for the window.
 */

static void calcFloatingSize(HWND hwnd, DOCKINFO *dwp)
{
	RECT	rect;

	rect.left = rect.top = 0;
	rect.right = dwp->cxFloating + rect.left;
	rect.bottom = dwp->cyFloating + rect.top;
	AdjustWindowRectEx(&rect, POPUP_STYLES, FALSE, POPUP_EXSTYLES);

	dwp->nFrameWidth = (unsigned short)(rect.right - rect.left);
	dwp->nFrameHeight = (unsigned short)(rect.bottom - rect.top);
}





/******************** setFloatingWinPos() *******************
 * Adjusts a Docking Frame window's floating size and/or
 * position, and/or bring it to the top of the Z-order.
 */

static void setFloatingWinPos(HWND hwnd, DOCKINFO *dwp, DWORD dwSWFlags)
{
	calcFloatingSize(hwnd, dwp);
	SetWindowPos(hwnd, HWND_TOP, dwp->xpos, dwp->ypos, dwp->nFrameWidth, dwp->nFrameHeight, dwSWFlags);
}





/********************** updateLayout() **********************
 * Forces the container window to recalculate its layout (and
 * ultimately cause its docked windows to be redrawn).
 *
 * NOTE: This causes a WM_SIZE message to be sent to the
 * container window. The container's window procedure must
 * call DockingArrangeWindows() to position/size all docked
 * windows, and cause them to be repainted.
 */

static void updateLayout(HWND container)
{
	RECT	rect;

	// To tell the container window to recalculate/redraw its layout,
	// we send it a "fake" WM_SIZE message. We'll just use the current
	// width/height of the container window for the message, and tell
	// it that it has been "restored"
	GetClientRect(container, &rect);
	SendMessage(container, WM_SIZE, SIZE_RESTORED, MAKELPARAM(rect.right - rect.left, rect.bottom - rect.top));
}




void WINAPI DockingUpdateLayout(HWND container)
{
	updateLayout(container);
}





/******************* redrawDockingState() ******************
 * Redraws a Docking Frame (window) in its current (docked
 * or floating) state.
 *
 * hwnd =	Handle to Docking Frame.
 */

static void redrawDockingState(DOCKINFO *dwp)
{
	HWND	hwnd;

	hwnd = dwp->hwnd;

	// Don't allow position/size to be saved when we "hide" the window. The Windows
	// operating system "hides" a window by placing it at a position offscreen
	dwp->dwStyle |= DWS_DONTSAVEPOS;

	// Hide the Docking Frame, because we don't want it to
	// be in the wrong position when it is docked/floated
	ShowWindow(hwnd, SW_HIDE);

	// Set its window styles, and parent, per its new docking style
	if (dwp->uDockedState & DWS_FLOATING)
	{
		SetWindowLong(hwnd, GWL_STYLE, POPUP_STYLES);
		SetWindowLong(hwnd, GWL_EXSTYLE, POPUP_EXSTYLES);
		SetParent(hwnd, 0);		// Floating windows have no parent (even though they are still owned by the container window)
	}
	else
	{
		SetWindowLong(hwnd, GWL_STYLE, CHILD_STYLES);
		SetWindowLong(hwnd, GWL_EXSTYLE, CHILD_EXSTYLES);
		SetParent(hwnd, dwp->container);
	}

	// Send a WM_NCCALCSIZE message, because the frame-style has changed
	SetWindowPos(hwnd, 0, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_FRAMECHANGED);

	// Give some child window in the client area the focus, unless caller doesn't want that behavior
	if (dwp->focusWindow) SetFocus(dwp->focusWindow);

	// Allow position/size to be saved again
	dwp->dwStyle &= ~DWS_DONTSAVEPOS;

	// Have the container window resize/reposition all its docked windows
	// to accomodate this new layout
	updateLayout(dwp->container);

	// If floating, position/size/show it (because updateLayout doesn't do that for floating windows)
	if (dwp->uDockedState < 0) setFloatingWinPos(hwnd, dwp, SWP_SHOWWINDOW);

	// Show the docked frame now that it's in the right place. (It has already
	// been positioned/sized in the container window's WM_SIZE handling caused by updateLayout)
	else
		SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_SHOWWINDOW|SWP_NOMOVE|SWP_NOSIZE);
}





/********************** drawDragFrame() *********************
 * Draws the drag rectangle when a Docking Frame is being
 * dragged by the user for docking.
 *
 * NOTE: Certain globals must be set:
 *
 * DragRecPlacement = Screen position and size of drag rectangle.
 * DragRecType		= 1 if solid drag rectangle, or 0 if checkered.
 */

static void drawDragFrame(void)
{
	HDC			hdc;
	HBITMAP		hbm;
	HBRUSH		hbrush;
	HANDLE		hbrushOrig;
	int			border;

	hdc = GetDC(0);

	{
	const WORD	*bitmap;

	// Determine whether to draw a solid drag rectangle or checkered
	if (DragRecType)
	{
		border = 1;
		bitmap = &DotPatternBmp2[0];
	}
	else
	{
		border = 3;
		bitmap = &DotPatternBmp1[0];
	}

	// Create a brush with the appropriate bitmap pattern to draw our drag rectangle
	hbm = CreateBitmap(8, 8, 1, 1, bitmap);
	hbrush = CreatePatternBrush(hbm);
	}

	SetBrushOrgEx(hdc, DragRecPlacement.left, DragRecPlacement.top, 0);
	hbrushOrig = SelectObject(hdc, hbrush);

	// Draw the drag rectangle
	{
	int			width, height;

	width = DragRecPlacement.right - DragRecPlacement.left;
	height = DragRecPlacement.bottom - DragRecPlacement.top;
	PatBlt(hdc, DragRecPlacement.left + border, DragRecPlacement.top, width - border, border, PATINVERT);
	PatBlt(hdc, DragRecPlacement.left + width - border, DragRecPlacement.top + border, border, height - border, PATINVERT);
	PatBlt(hdc, DragRecPlacement.left, DragRecPlacement.top + height - border, width - border,  border, PATINVERT);
	PatBlt(hdc, DragRecPlacement.left, DragRecPlacement.top, border, height - border, PATINVERT);
	}

	SelectObject(hdc, hbrushOrig);
	DeleteObject(hbrush);
	DeleteObject(hbm);
	ReleaseDC(0, hdc);
}





/********************** dragHookProc() *********************
 * Keyboard hook used during the dragging/dropping of a
 * Docking Frame window. This hook just lets the user toggle
 * the drop mode by pressing the CTRL key, or aborting the
 * drop by pressing the ESC key.
 */

static LRESULT CALLBACK dragHookProc(int code, WPARAM wParam, LPARAM lParam)
{
	if (code >= 0)
	{
		if (wParam == VK_CONTROL)
		{
			unsigned char	newValue;

			if (lParam < 0)
				newValue = 0;
			else
				newValue = 1;

			// If user presses CRTL, toggle the drag mode by
			// flipping the state of ControlKeyDown variable
			// and sending the Docking Frame window a
			// WM_MOUSEMOVE with a delta change of 0
			if (ControlKeyDown != newValue)
			{
				ControlKeyDown = newValue;
				MouseMoved |= 0x80;
				SendMessage(DraggingDockWnd, WM_MOUSEMOVE, 0, 0);
			}
			return(-1);
		}

		// If user presses ESC, abort the drag by sending the
		// Docking Frame window a WM_CANCELMODE message
		if (wParam == VK_ESCAPE)
		{
			PostMessage(DraggingDockWnd, WM_CANCELMODE, 0, 0);
			return(0);
		}
	}

	return(CallNextHookEx(OrigDockHookProc, code, wParam, lParam));
}






/********************** drawGripper() ********************
 * Draws a Docking Frame window's gripper
 *
 * hdc =	Device DC for drawing.
 * x, y =	Upper left corner of gripper.
 * height =	Height of gripper.
 */

static void drawGripper(HDC hdc, RECT *rect)
{
	DrawEdge(hdc, rect, BDR_RAISEDINNER, BF_RECT);
	OffsetRect(rect, 3, 0);
	DrawEdge(hdc, rect, BDR_RAISEDINNER, BF_RECT);
}





/********************** eraseBkGnd() *********************
 * Draws a Docking Frame window's etched borders and gripper
 *
 * hwnd =	Handle to Docking Frame window.
 * dwp =	Pointer to DOCKINFO for hwnd.
 * hdc =	Device DC for drawing.
 *
 * RETURNS: 1 if the background is erased, or 0 if not.
 *
 * NOTE: This is called by the Docking Frame's window
 * procedure when receiving a WM_ERASEBKGND message.
 */

static void eraseBkGnd(HWND hwnd, DOCKINFO *dwp, HDC hdc)
{
	RECT	rc;

	// Get size of Docking frame client area
	GetClientRect(hwnd, &rc);

	// Erase the background
	SetBkColor(hdc, GetSysColor(COLOR_BTNFACE));
	ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rc, &DockClassName[9] /* Null string */, 0, 0);

	// Draw the gripper if caller wants that
	rc.left = 1;
	rc.right = 1 + 3;
	if (dwp->uDockedState > 0)
	{
		if (dwp->dwStyle & DWS_DRAWGRIPPERDOCKED)
		{
			rc.top = 1;
			drawGripper(hdc, &rc);
		}
	}
	else if (dwp->dwStyle & DWS_DRAWGRIPPERFLOATING)
	{
		rc.top = 3;
		--rc.bottom;
		drawGripper(hdc, &rc);
	}
}





/******************** checkDockingPos() ********************
 * Called after the drag rectangle has been moved by the user.
 * This checks if the drag rectangle has been moved to a
 * docking, or non-docking, area. It also updates the global
 * DragRecPlacement RECT to the position and size of the
 * drag rectangle.
 */

static char checkDockingPos(DOCKINFO *dwp)
{
	RECT	prc1, prc2;

	// Erase the drag rectangle by XOR drawing it at the same position
	// as before. NOTE: We assume that it has already been drawn at
	// least once
	drawDragFrame();

	// The size of the drag rectangle should now be the size of the Docking
	// Frame when it is not docked (ie, floating). The drag rectangle should
	// be centered around the current position of the mouse pointer
	DragRecPlacement.left = MousePos.x - (dwp->nFrameWidth >> 1);
	if (DragRecPlacement.left < 0) DragRecPlacement.left = 0;
	DragRecPlacement.top = MousePos.y - (dwp->nFrameHeight >> 1);
	if (DragRecPlacement.top < 0) DragRecPlacement.top = 0;
	DragRecPlacement.right = DragRecPlacement.left + dwp->nFrameWidth;
	DragRecPlacement.bottom = DragRecPlacement.top + dwp->nFrameHeight;

	// ===========================================================
	// Check if the drag rectangle has moved into a dockable area
	// ===========================================================
	{
	HWND	container;

	// Get container window's "outer" rectangle
	container = dwp->container;
	GetWindowRect(container, &prc1);
	
	// Get container window's "inner" client rectangle (relative to screen)
	GetClientRect(container, &prc2);
	MapWindowPoints(container, HWND_DESKTOP, (POINT *)&prc2, 2);
	InflateRect(&prc2, -2, -2);
	}

	{
	char	uDockSide;

	// Assume floating
	uDockSide = (char)DWS_FLOATING;

	// If outside of the container frame, then floating
	if (DragRecPlacement.left >= prc1.left && DragRecPlacement.right <= prc1.right &&
		DragRecPlacement.top >= prc1.top && DragRecPlacement.bottom <= prc1.bottom)
	{
		// Check intersection at bottom
		if (DragRecPlacement.bottom > prc2.bottom && (dwp->dwStyle & DWS_ALLOW_DOCKBOTTOM)) uDockSide = DWS_DOCKED_BOTTOM;

		// Check intersection at top
		if (DragRecPlacement.top < prc2.top && (dwp->dwStyle & DWS_ALLOW_DOCKTOP)) uDockSide = DWS_DOCKED_TOP;

		// Check intersection at left
		if (DragRecPlacement.left < prc2.left && (dwp->dwStyle & DWS_ALLOW_DOCKLEFT)) uDockSide = DWS_DOCKED_LEFT;

		// Check intersection at right
		if (DragRecPlacement.right > prc2.right && (dwp->dwStyle & DWS_ALLOW_DOCKRIGHT)) uDockSide = DWS_DOCKED_RIGHT;
	}

	DragRecType = uDockSide > 0 && !ControlKeyDown;

	return(uDockSide);
	}
}





/*********************** dockWndProc() *********************
 * Window procedure for a Docking Frame window.
 */

static LRESULT CALLBACK dockWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	register DOCKINFO *dwp;

	if(g_wndproc)
		g_wndproc(hwnd, msg, wParam, lParam);
	// Get the DOCKINFO struct that we saved in the docking frame's GWLP_USERDATA field
	dwp = (DOCKINFO *)GetWindowLong(hwnd, GWLP_USERDATA);

	switch(msg)
	{
		case WM_SIZE:{
			break;
		}
		case WM_NCDESTROY:
		{
			register HWND	container;

			// Let's get the container HWND just in case the DockDestroy callback
			// deletes the DOCKINFO
			container = dwp->container;

			// Let the app know that we don't need this DOCKINFO anymore
			(*dwp->DockDestroy)(dwp);

			// Zero out the DOCKINFO * for this window
			SetWindowLong(hwnd, GWLP_USERDATA, 0);

			// Force container window to recalc its layout
			updateLayout(container);

			goto ret0;
		}

		case WM_NCHITTEST:
		{
			// Let OS do its normal hit testing
			wParam = DefWindowProc(hwnd, WM_NCHITTEST, wParam, lParam);

			// Is it docked?
			if (dwp->uDockedState > 0)
			{
				// Is he over the border?
				if (wParam == HTBORDER)
				{
					wParam = HTCAPTION;

					// Does caller want to allow this window to be resized?
					if (!(dwp->dwStyle & DWS_NORESIZE))
					{
						RECT	rc;

						GetWindowRect(hwnd, &rc);

						switch (dwp->uDockedState & ~0x80)
						{
							// If window is docked on the left, check if he is over the
							// right border. If so return HTRIGHT
							case DWS_DOCKED_LEFT:
							{
								lParam = LOWORD(lParam);
								if (rc.right >= lParam && rc.right - 3 <= lParam) wParam = HTRIGHT;
								break;
							}

							// If window is docked on the right, check if he is over the
							// left border. If so return HTLEFT
							case DWS_DOCKED_RIGHT:
							{
								lParam = LOWORD(lParam);
								if (rc.left <= lParam && rc.left + 3 >= lParam) wParam = HTLEFT;
								break;
							}

							// If window is docked on the top, check if he is over the
							// bottom border. If so return HTBOTTOM
							case DWS_DOCKED_TOP:
							{
								lParam = HIWORD(lParam);
								if (rc.bottom >= lParam && rc.bottom - 3 <= lParam) wParam = HTBOTTOM;
								break;
							}

							// If window is docked on the bottom, check if he is over the
							// top border. If so return HTTOP
							case DWS_DOCKED_BOTTOM:
							{
								lParam = HIWORD(lParam);
								if (rc.top <= lParam && rc.top + 3 >= lParam) wParam = HTTOP;
								break;
							}
						}
					}

					return(wParam);
				}

				// Allow dragging by the client area
				if (wParam == HTCLIENT) wParam = HTCAPTION;
			}

			return(wParam);
		}

		case WM_WINDOWPOSCHANGED:
		{
			if (dwp)
			{
				WINDOWPOS	*wp;
				RECT		border;

				wp = (WINDOWPOS *)lParam;

				GetClientRect(hwnd, &border);

				// Don't save this position?
				if (!(dwp->dwStyle & DWS_DONTSAVEPOS))
				{
					// Is the window floating?
					if (dwp->uDockedState < 0)
					{
						// Update the DOCKINFO's floating XY position
						if (!(wp->flags & SWP_NOMOVE))
						{
							dwp->xpos = wp->x;
							dwp->ypos = wp->y;
						}

						// Update the DOCKINFO's floating width/height
						if (!(wp->flags & SWP_NOSIZE))
						{
							dwp->nFrameWidth = wp->cx;
							dwp->nFrameHeight = wp->cy;

							// Also update Docking Frame's client size, because we base
							// the Docking Frame's total window size (ie, with caption
							// and borders) on the client size
							dwp->cxFloating = (unsigned short)border.right;
							dwp->cyFloating = (unsigned short)border.bottom;
						}
					}

					// Were we resizing a docked window?
					else
					{
						if ((dwp->uDockedState & ~0x80) & (DWS_DOCKED_LEFT|DWS_DOCKED_RIGHT))
							border.left = wp->cx;
						else
							border.left = wp->cy;

						if (dwp->nDockedSize != border.left)
						{
							dwp->nDockedSize = (unsigned short)border.left;

							// Tell main window to update its layout
							updateLayout(dwp->container);
						}
					}

					// ======= Resize the client area

					if (dwp->uDockedState > 0)
					{
						// Let the child window fill the client area of the Docking Frame (minus
						// the below clipped areas)
						GetClientRect(hwnd, &border);

						// Allow space for the gripper if caller wants it
						if (dwp->dwStyle & DWS_DRAWGRIPPERDOCKED)
						{
							border.left += 7;
							border.right -= 7;
						}
					}
					else
					{
						border.left = border.top = 0;
						if (dwp->dwStyle & DWS_DRAWGRIPPERFLOATING) border.left = 3;
						border.right = dwp->cxFloating;
						border.bottom = dwp->cyFloating - border.left;
					}

					// If the window's size changed, make sure the entire window is
					// invalidated to force everything to be redrawn. This just
					// makes things easier
					if (!(wp->flags & SWP_NOSIZE)) InvalidateRect(hwnd, 0, TRUE);

					// Position/size the child window(s) which are inside of the
					// Docking Frame
					(*dwp->DockResize)(dwp, &border);
				}
			}

			return(0);
		}

		case WM_NCLBUTTONDBLCLK:
		{
			// User clicked on the caption area of the Docking Frame?
			if (wParam == HTCAPTION)
			{
				// Yes he did. Toggle between docking and floating
				dwp->uDockedState ^= 0x80;
				redrawDockingState(dwp);
				return(0);
			}

			break;
		}

		case WM_NCLBUTTONDOWN:
		{
			if (wParam == HTCAPTION)
			{
				// Begin a drag operation unless caller wants the original state maintained
				if (!(dwp->dwStyle & DWS_KEEPORIGSTATE))
				{
					POINT	pt;

					ControlKeyDown = 0;
					if (GetKeyState(VK_CONTROL) & 0x8000) ControlKeyDown = 1;

					// Get mouse pointer position in screen coords
					GetCursorPos(&pt);

					// Bring Docking Frame to the top
					SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);

					// Capture the mouse
					SetCapture(hwnd);

					// Indicate we're dragging now
					DraggingDockWnd = hwnd;

					// Install the keyboard hook to check for user pressing CTRL or ESC keys
					OrigDockHookProc = SetWindowsHookEx(WH_KEYBOARD, dragHookProc, GetModuleHandle(0), 0); 

					// Get the current position/size of Docking Frame and save it in a
					// global RECT. This may either be the docked size, or the floating size
					GetWindowRect(hwnd, &DragRecPlacement);

					// Set "DragRecType" based upon current state. We use this
					// variable to help drawDragFrame() figure out how to erase
					// a previously drawn drag rectangle when the user drags it
					// to a new position
					if (dwp->uDockedState > 0)
						DragRecType = 1;
					else
						DragRecType = 0;

					// Save the mouse position
					MousePos.x = (short)pt.x;
					MousePos.y = (short)pt.y;

					// Draw the drag rectangle around the current frame
					drawDragFrame();

					// Mouse has not yet moved
					MouseMoved = 0;
				}

				// Prevent the operating system from dragging the window
ret0_2:			return(0);
			}

			// Is it docked? Did he click on a border to resize the docked window?
			if (dwp->uDockedState > 0 && (wParam == HTLEFT || wParam == HTRIGHT || wParam == HTTOP || wParam == HTBOTTOM))
			{
				POINT	pt;

				// Get mouse pointer position in screen coords
				GetCursorPos(&pt);

				// Bring Docking Frame to the top
				SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);

				// Capture the mouse
				SetCapture(hwnd);

				// Indicate we're sizing now
				SizingDockWnd = hwnd;

				// Save the mouse position
				if ((dwp->uDockedState & ~0x80) & (DWS_DOCKED_LEFT|DWS_DOCKED_RIGHT)) MousePos.x = (short)pt.x;
				else MousePos.x = (short)pt.y;

				goto ret0_2;
			}

			// Otherwise, let operating system do normal behaviour for this mouse down
			break;
		}

		case WM_CANCELMODE:
		case WM_LBUTTONUP:
		{
			if (DraggingDockWnd)
			{
				// Mouse released or user cancelled. Now we need to check if we dock/undock
				// the Docking Frame window, or leave it as-is.

				// Remove the keyboard hook
				if (OrigDockHookProc)
				{
					UnhookWindowsHookEx(OrigDockHookProc); 
					OrigDockHookProc = 0;
				}

				// Indicate we're done with the dragging
				DraggingDockWnd = 0;

				// Did the user actually move the Docking Frame, and not cancel the operation?
				if (msg != WM_CANCELMODE && MouseMoved)
				{
					POINT	pt;
					char	uDockSide;

					// Get final cursor position
					GetCursorPos(&pt);
					MousePos.x = (short)pt.x;
					MousePos.y = (short)pt.y;

					// Erase the drag rectangle by redrawing it at the same position as before,
					// and check if it was moved into a dockable area
					uDockSide = checkDockingPos(dwp);

					// If the window was docked, see if it is now floating
					if (dwp->uDockedState > 0)
					{
						// If he held the control key down, or moved it to a non-dockable
						// area, then it is now floating
						if (uDockSide < 0)
						{
							// Set the XY position where the Docking Frame will float. The
							// position is where the last mouse position is
							if (DragRecPlacement.left < 0) DragRecPlacement.left = 0;
							dwp->xpos = (unsigned short)DragRecPlacement.left;
							if (DragRecPlacement.top < 0) DragRecPlacement.top = 0;
							dwp->ypos = (unsigned short)DragRecPlacement.top;

							// Set the docking mode to floating and redraw the window. Also
							// force the container window to update its layout
							dwp->uDockedState |= (char)0x80;
							redrawDockingState(dwp);
						}

						// The window is still docked (but may have changed to different
						// side of the container window)
						else
						{
							dwp->uDockedState = uDockSide;

							// Invalidate any child window so that it redraws itself, just in case
							// the frame moved
							if (dwp->focusWindow) InvalidateRect(dwp->focusWindow, 0, 1);

							// Send a WM_SIZE message to the container window so it repositions and
							// redraws everything docked inside of it
							updateLayout(dwp->container);
						}
					}

					// It was floating, so see if it is now docked
					else
					{
						// If he didn't hold the control key down, and moved it to a dockable
						// area, then it is now docked
						if (uDockSide > 0)
						{
							dwp->uDockedState = uDockSide;
							redrawDockingState(dwp);
						}

						// It is still floating. Just change the Docking Frame position to the last
						// position of the mouse
						else
							SetWindowPos(hwnd, 0, DragRecPlacement.left, DragRecPlacement.top, 0, 0, SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOZORDER|SWP_DRAWFRAME|SWP_NOSENDCHANGING);
					}
				}

				// Erase the drag rectangle if user cancelled
				else
					drawDragFrame();

				// No need to trap the mouse any further since we're done
mousedone:		ReleaseCapture();
				MouseMoved = 0;
			}

			// Sizing a docked window?
			else if (SizingDockWnd)
			{
				// Indicate we're done with the sizing
				SizingDockWnd = 0;

				goto mousedone;
			}


			break;
		}

		case WM_MOUSEMOVE:
		{
			// Dragging?
			if (DraggingDockWnd)
			{
				// Yes. Move the drag rectangle.
				POINT			pt;
				register char	uDockSide;

				// Get mouse pointer position in screen coords
				GetCursorPos(&pt);

				// Did the mouse move?
				if (pt.x != MousePos.x || pt.y != MousePos.y)
				{
					// Indicate the drag rectangle (and therefore potentially the Docking Frame) has moved
					MouseMoved = 1;

					// Save new mouse pointer position
					MousePos.x = (short)pt.x;
					MousePos.y = (short)pt.y;
				}
	
				// Did the mouse move, or CTRL key status changed?
				if (MouseMoved)
				{
					// Clear CTRL key status change
					MouseMoved &= ~0x80;

					// Update position/size of drag rectangle and check if it has moved into
					// a dockable area
					uDockSide = checkDockingPos(dwp);

					// If we have moved over a dockable area, then temporarily change the size
					// of the drag rectangle to fill that dockable area
					if (DragRecType)
					{
						RECT		rc;

						// Get size/position of container window's client area, relative to screen
						GetClientRect(dwp->container, &rc);
						MapWindowPoints(dwp->container, 0, (POINT *)&rc, 2);

						switch (uDockSide)
						{
							case DWS_DOCKED_LEFT:
							{
								DragRecPlacement.right = rc.left + dwp->nDockedSize;
								DragRecPlacement.left = rc.left;
								goto setleft;
							}

							case DWS_DOCKED_RIGHT:
							{
								DragRecPlacement.left = rc.right - dwp->nDockedSize;
								DragRecPlacement.right = rc.right;
setleft:						DragRecPlacement.top = rc.top;
								DragRecPlacement.bottom = rc.bottom;
								break;
							}

							case DWS_DOCKED_TOP:
							{
								DragRecPlacement.bottom = rc.top + dwp->nDockedSize;
								DragRecPlacement.top = rc.top;
								goto settop;
							}

							case DWS_DOCKED_BOTTOM:
							{
								DragRecPlacement.top = rc.bottom - dwp->nDockedSize;
								DragRecPlacement.bottom = rc.bottom;
settop:							DragRecPlacement.left = rc.left;
								DragRecPlacement.right = rc.right;
	//							break;
							}
						}
					}

					// Draw the drag rectangle at the new position
					drawDragFrame();
				}
			}

			// Sizing a docked window?
			else if (SizingDockWnd)
			{
				// Yes
				POINT	pt;
				RECT	rc;

				// Get mouse pointer position in screen coords, and docking frame dimensions
				GetCursorPos(&pt);
				GetWindowRect(hwnd, &rc);

				// Increase or decrease the window size by that amount
				if ((dwp->uDockedState & ~0x80) & (DWS_DOCKED_LEFT|DWS_DOCKED_RIGHT))
				{	
					pt.y = pt.x - MousePos.x;
					if (pt.y < (rc.right - rc.left))
						SetWindowPos(hwnd, 0, 0, 0, (rc.right - rc.left) + pt.y, rc.bottom - rc.top, SWP_NOMOVE|SWP_NOZORDER);
				}
				else
				{
					pt.x = pt.y;
					pt.y = pt.x - MousePos.x;
					if (pt.y < (rc.bottom - rc.top))
						SetWindowPos(hwnd, 0, 0, 0, rc.right - rc.left, (rc.bottom - rc.top) + pt.y, SWP_NOMOVE|SWP_NOZORDER);
				}

				// Save new mouse pointer position
				MousePos.x = (short)pt.x;
			}

			break;
		}

		case WM_GETMINMAXINFO:
		{
			// Prevent window sizing if not docked, and style is DWS_NORESIZE
			if (dwp && dwp->uDockedState < 0 && (dwp->dwStyle & DWS_NORESIZE))
			{
				((MINMAXINFO *)lParam)->ptMinTrackSize.x = ((MINMAXINFO *)lParam)->ptMaxTrackSize.x = dwp->nFrameWidth;
				((MINMAXINFO *)lParam)->ptMaxTrackSize.y = ((MINMAXINFO *)lParam)->ptMinTrackSize.y = dwp->nFrameHeight;
				goto ret0;
			}

			break;
		}

		case WM_ERASEBKGND:
		{
			// Erase the background and draw the gripper bar
			eraseBkGnd(hwnd, dwp, (HDC)wParam);

			// Don't let windows erase the background
			return(1);
		}

		case WM_NCACTIVATE:
		{
			DOCKPARAMS	dockParams;

			dockParams.container = dwp->container;
			dockParams.hwnd = hwnd;
			dockParams.wParam = wParam;
			dockParams.lParam = lParam;
			return(DockingActivate(&dockParams));
		}

		case WM_CLOSE:
		{
			if ((*dwp->DockClose)(dwp))
			{
				if (!(dwp->dwStyle & DWS_NODESTROY)) DestroyWindow(hwnd);
				else
				{
					dwp->dwStyle |= (DWS_DONTSAVEPOS|DWS_HIDDEN);
					ShowWindow(hwnd, SW_HIDE);
					if (dwp->uDockedState > 0) updateLayout(dwp->container);
				}
			}

ret0:		return(0);
		}

		case WM_SETTINGCHANGE:
		{
			if (dwp->uDockedState < 0 && !(dwp->dwStyle & DWS_HIDDEN)) setFloatingWinPos(hwnd, dwp, SWP_NOACTIVATE|SWP_NOZORDER);
			goto ret0;
		}
	}

	// If caller supplied a DockMsg() callback, then call it. If not, or if
	// he doesn't handle the message, let Windows handle it as normally
	if (!dwp || !dwp->DockMsg)
def:	return(DefWindowProc(hwnd, msg, wParam, lParam));

	{
	register LRESULT	result;

	if ((result = (*dwp->DockMsg)(dwp, msg, wParam, lParam)) == -1) goto def;
	return(result);
	}
}




/********************* dockResizeFunc() ********************
 * Default callback for resizing/positioning the contents of
 * the client area of a Docking Frame.
 *
 * dwp =	DOCKINFO * for the Docking Frame.
 * border =	RECT containing the client area.
 */

static void WINAPI dockResizeFunc(DOCKINFO *dwp, RECT *border)
{
	MoveWindow(dwp->focusWindow, border->left, border->top, border->right, border->bottom, TRUE);
	updateLayout(dwp->hwnd);
}




/********************* dockCloseFunc() ********************
 * Default callback for querying if a specific Docking Frame
 * window can be closed.
 *
 * dwp =	DOCKINFO * for the Docking Frame.
 *
 * RETURNS: TRUE to allow the window to close, or FALSE otherwise.
 */

static DWORD WINAPI dockCloseFunc(DOCKINFO *dwp)
{
	return(TRUE);
}



















////////////////////////////////////////////////////////////////////////
// Application-callable API.
////////////////////////////////////////////////////////////////////////

/********************* DockingInitialize() ********************
 * Initializes use of the docking functions. The application
 * calls this once only, before using the docking library API.
 *
 * docklist =	An initialized DOCKLIST containing pointers
 *				to callback functions in the application which
 *				manage a collection of DOCKINFO structs,
 *
 * RETURNS: 0 if success, or an error number.
 */

ULONG WINAPI DockingInitialize(void *param)
{
	g_wndproc = param;
	// Save the module handle. It will be the same for all instances of the DLL
	ThisModule = GetModuleHandle(0);

	// Not dragging a Docking Frame right now
	DraggingDockWnd = 0;

	// Register window class for the Docking Frame.
	// Its message procedure is dockWndProc
	{
		WNDCLASSEX	wc;

		ZeroMemory(&wc, sizeof(wc));
		wc.cbSize = sizeof(WNDCLASSEX);
		//	wc.style = 0;
		wc.lpfnWndProc = dockWndProc;
		//	wc.cbClsExtra = wc.cbWndExtra = 0;
		wc.hInstance = ThisModule;
		//	wc.hIcon = 0;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
		//	wc.lpszMenuName = 0;
		wc.lpszClassName = &DockClassName[0];
		//	wc.hIconSm = 0;

		if (!(DockingFrameAtom = RegisterClassEx(&wc)))
		{
			TCHAR		buffer[160];

			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), &buffer[0], 160, 0);
			MessageBox(0, &buffer[0], &DockClassName[0], MB_OK);
			return(0);
		}
	}

	return 0;
}





/******************** DockingUnInitialize() ******************
 * Completes use of the docking library. An application calls
 * this once when done using the library.
 */

void WINAPI DockingUnInitialize()
{
	// Unregister docking frame window
	UnregisterClass(&DockClassName[0], ThisModule);
}





/********************** DockingAlloc() *********************
 * Gets an initialized DOCKINFO struct.
 *
 * initialDocking =	Initial docking state of the frame. Must be
 *					one of DWS_FLOATING, DWS_DOCKED_LEFT,
 *					DWS_DOCKED_RIGHT, DWS_DOCKED_DOCKTOP, or
 *					DWS_DOCKED_BOTTOM.
 *
 * RETURNS: Pointer to the new DOCKINFO, or 0 if an error.
 *
 * NOTE: Typically, the application will change some fields
 * of the returned DOCKINFO if the app wants something other
 * than default behavior.
 */

DOCKINFO * WINAPI DockingAlloc(char initialDocking)
{
	DOCKINFO		*dwp;

	// Allocate a DOCKINFO struct and initialize it
	if ((dwp = (DOCKINFO *)GlobalAlloc(GMEM_FIXED, sizeof(DOCKINFO))))
	{
		ZeroMemory(dwp, sizeof(DOCKINFO));

		// Set floating width/height to defaults
		dwp->cxFloating = 400;
		dwp->cyFloating = 200;

		// Set size when docked. NOTE: We need only the width or height when docked
		if (initialDocking & (DWS_DOCKED_BOTTOM|DWS_DOCKED_TOP))
			dwp->nDockedSize = 100;
		else
			dwp->nDockedSize = 200;

		// Assume gripper drawn when docked and docking allowed for all sides of container window
		dwp->dwStyle = DWS_DRAWGRIPPERDOCKED|DWS_ALLOW_DOCKALL;

		// Set default callbacks
		dwp->DockResize = dockResizeFunc;
		dwp->DockClose = dockCloseFunc;
		dwp->DockDestroy = DockingFree;

		// Store the initial docking state
		if (initialDocking == (char)DWS_FLOATING) initialDocking |= DWS_DOCKED_BOTTOM;
		dwp->uDockedState = initialDocking;
	}

	return(dwp);
}





/********************** DockingFree() *********************
 * Frees a DOCKINFO struct.
 *
 * dwp =	Pointer to the DOCKINFO to be freed.
 *
 * NOTE: A DOCKINFO (that has been successfully put into
 * service via DockingCreateFrame) should not be freed until
 * the docking library calls the DOCKINFO->DockDestroy
 * callback (or sometime after it has been passed to that
 * callback).
 *
 * If an application does not set its own DockDestroy
 * callback, then it should never call DockingFree(). The
 * docking library will free the DOCKINFO instead.
 */

void WINAPI DockingFree(DOCKINFO *dwp)
{
	GlobalFree(dwp);
}





/****************** DockingCreateFrame() ******************
 * Creates a Docking Frame window.
 *
 * dwp =		An initialized DOCKINFO struct allocated with
 *				DockingAlloc().
 * container =	Handle of window to which the Docking Frame
 *				can be docked.
 * caption =	Nul-terminated title for Docking Frame.
 *
 * RETURNS: The new docking frame handle, or 0 if an error.
 *
 * NOTE: DockingCreateFrame will call the application's
 * DockAddItem callback to make this DOCKINFO available
 * for use. If the app's callback returns TRUE, then the
 * app must not free that DOCKINFO until the DOCKINFO is
 * passed to the DockDestroy callback.
 *
 * If DockingCreateFrame fails, the caller is responsible
 * for freeing the DOCKINFO via DockingFree if it is not
 * using the default DockDestroy callback.
 */

HWND WINAPI DockingCreateFrame(DOCKINFO *dwp, HWND container, LPCTSTR caption)
{
	UINT		dwStyle;
	UINT		dwExStyle;

	// Save the container window
	dwp->container = container;

	// Create as a popup window initially so that CreateWindowEx honors the "owner" parameter passed to it
	dwStyle = POPUP_STYLES;
	dwExStyle = POPUP_EXSTYLES;

	// Create the Docking Frame window
	(dwp->hwnd = CreateWindowEx(dwExStyle, &DockClassName[0], caption, dwStyle,
		dwp->xpos, dwp->ypos, 0, 0,
		(dwp->dwStyle & DWS_FREEFLOAT) ? 0 : container, 0, ThisModule, 0));
	DWORD dwer = GetLastError();
	if (dwp->hwnd)
	{
		// Set styles depending upon whether window should be created docked or floating
		if (dwp->uDockedState > 0)
		{
			dwStyle = CHILD_STYLES;
			dwExStyle = CHILD_EXSTYLES;
			SetWindowLong(dwp->hwnd, GWL_STYLE, CHILD_STYLES);
			SetWindowLong(dwp->hwnd, GWL_EXSTYLE, CHILD_EXSTYLES);
			SetParent(dwp->hwnd, container);
		}

		// Store the DOCKINFO struct in the docking frame's GWLP_USERDATA field for our own access
		SetWindowLong(dwp->hwnd, GWLP_USERDATA, (LONG)dwp);

		// Calculate the total window size of the docking frame (including the system's
		// size of borders and caption) when floating
		calcFloatingSize(dwp->hwnd, dwp);
	}

	// If using the default DockDestroy callback, then free the DOCKINFO now
	else if (dwp->DockDestroy == DockingFree)
	{
		DockingFree(dwp);
		return(0);
	}

	// Return Docking Frame
	return(dwp->hwnd);
}





/******************* DockingShowFrame() ********************
 * Shows/activates a Docking Frame.
 *
 * dwp =	The DOCKINFO struct for the Docking Frame.
 */

void WINAPI DockingShowFrame(DOCKINFO *dwp)
{
	// Clear any hidden flag
	dwp->dwStyle &= ~(DWS_DONTSAVEPOS|DWS_HIDDEN);
	
	if (dwp->uDockedState > 0)
	{
		dwp->dwStyle |= DWS_DONTSAVEPOS;
		setFloatingWinPos(dwp->hwnd, dwp, SWP_SHOWWINDOW|SWP_NOACTIVATE);
		dwp->dwStyle &= ~DWS_DONTSAVEPOS;

		// Force the container window to redraw its contents, which now includes this docked window
		updateLayout(dwp->container);
	}
	else
	{
		calcFloatingSize(dwp->hwnd, dwp);
		SetWindowPos(dwp->hwnd, HWND_TOP, dwp->xpos, dwp->ypos, dwp->nFrameWidth, dwp->nFrameHeight, SWP_SHOWWINDOW|SWP_NOACTIVATE|SWP_NOZORDER);
	}

	if (dwp->focusWindow) SetFocus(dwp->focusWindow);
	else
		SendMessage(dwp->hwnd, WM_NCACTIVATE, TRUE, 0);
}





/******************* DockingRedrawFrame() ********************
 * Redraws a Docking Frame and its contents, and calls the
 * DOCKINFO's DockResize() callback.
 *
 * dwp =	The DOCKINFO struct for the Docking Frame.
 */

void WINAPI DockingRedrawFrame(DOCKINFO *dwp)
{
	RECT	border;

	GetClientRect(dwp->hwnd, &border);

	// ======= Resize the client area
	if (dwp->uDockedState > 0)
	{
		// Allow space for the gripper if caller wants it
		if (dwp->dwStyle & DWS_DRAWGRIPPERDOCKED)
		{
			border.left += 7;
			border.right -= 7;
		}
	}
	else
	{
		border.left = border.top = 0;
		if (dwp->dwStyle & DWS_DRAWGRIPPERFLOATING) border.left = 3;
		border.right = dwp->cxFloating;
		border.bottom = dwp->cyFloating - border.left;
	}

	// Make sure the entire window is invalidated to force everything
	// to be redrawn. This just makes things easier
	InvalidateRect(dwp->hwnd, 0, TRUE);

	// Position/size the child window(s) which are inside of the Docking Frame
	(*dwp->DockResize)(dwp, &border);
}





/********************** DockingEnable() ******************
 * Enables/disables all Docking Frame windows. The
 * container window calls this when it receives a
 * WM_ENABLE message. The purpose of this function is to
 * keep the enabled state of all tool windows in sync with
 * the container window.
 *
 * params =		Pointer to DOCKPARAMS struct filled in by caller.
 */

static BOOL CALLBACK syncEnableProc(HWND hwnd, LPARAM params)
{
	DOCKINFO	*dwp;
	
	// Is this window one of the docking frames for the particular container?
	if (GetClassWord(hwnd, GCW_ATOM) == DockingFrameAtom &&
		(dwp = (DOCKINFO *)GetWindowLong(hwnd, GWLP_USERDATA)) &&
		dwp->container == ((DOCKPARAMS *)params)->container &&

		// DO NOT send this msg to the window procedure for the same
		// window that called DockingActivate()
		dwp->hwnd != ((DOCKPARAMS *)params)->hwnd && (!(dwp->dwStyle & DWS_NODISABLE) || ((DOCKPARAMS *)params)->wParam))
	{
		EnableWindow(dwp->hwnd, ((DOCKPARAMS *)params)->wParam);
	}

	return(TRUE);
}

LRESULT WINAPI DockingEnable(DOCKPARAMS *params)
{
	// Enable/Disable all Docking Frame windows except for the one that
	// called DockingEnable()
	EnumWindows(syncEnableProc, (LPARAM)params);
	EnumChildWindows(params->container, syncEnableProc, (LPARAM)params);

	// Do the default for the window that called here
	return(DefWindowProc(params->hwnd, WM_ENABLE, params->wParam, params->lParam));
}







/*********************** DockingActivate() **********************
 * Sends WM_NCACTIVATE to all the container's docked windows.
 * The container window calls this in response to receiving a
 * WM_NCACTIVATE message. The purpose of this function is to
 * keep the activation of all tool windows titlebars in sync.
 *
 * params =		Pointer to DOCKPARAMS struct filled in by caller.
 */

static BOOL CALLBACK syncActivateProc(HWND hwnd, LPARAM params)
{
	DOCKINFO	*dwp;

	// Is this window one of our docking frames for this container?
	if (GetClassWord(hwnd, GCW_ATOM) == DockingFrameAtom &&
		(dwp = (DOCKINFO *)GetWindowLong(hwnd, GWLP_USERDATA)) &&
		dwp->container == ((DOCKPARAMS *)params)->container &&

		// DO NOT send this msg to the window procedure for the same
		// window that called DockingActivate(). Also don't bother
		// sending it if the window is hidden
		dwp->hwnd != ((DOCKPARAMS *)params)->hwnd && dwp->hwnd != (HWND)((DOCKPARAMS *)params)->lParam && !(dwp->dwStyle & DWS_HIDDEN))
	{
		SendMessage(dwp->hwnd, WM_NCACTIVATE, ((DOCKPARAMS *)params)->wParam, (LPARAM)-1);
	}

	return(TRUE);
}

static BOOL CALLBACK activateProc(HWND hwnd, LPARAM params)
{
	DOCKINFO	*dwp;

	// Is this window one of our docking frames for this container?
	if (GetClassWord(hwnd, GCW_ATOM) == DockingFrameAtom &&
		(dwp = (DOCKINFO *)GetWindowLong(hwnd, GWLP_USERDATA)) &&
		dwp->container == ((DOCKPARAMS *)params)->container &&

		// Is the other window being activated/deactivated (i.e. not the
		// one that called DockingActivate) one for this container?
		(HWND)((DOCKPARAMS *)params)->lParam == dwp->hwnd)
	{
		// Ok, it's one of our own, so leave its titlebar selected,
		// and don't send any other windows any WM_NCACTIVATE msgs.
		// We indicate this to DockingActivate() by clearing
		// "container"
		((DOCKPARAMS *)params)->container = 0;

		// We're done enumerating the docking frames of this container
		return(FALSE);
	}

	return(TRUE);
}

LRESULT WINAPI DockingActivate(DOCKPARAMS *params)
{
	// If this WM_NCACTIVATE was sent by our own loop below, then do normal
	// WM_NCACTIVATE processing. We'll know that it was our own WM_NCACTIVATE
	// because lParam = -1 (instead of some window handle)
	if (params->lParam == -1) params->lParam = 0;
	else
	{
		// Ok, this is the window that originally called DockingActivate().

		if (params->container == (HWND)params->lParam) goto mine;

		// Check if the window that is about to be activated/deactivated (i.e.
		// not the one that called DockingActivate) is one for this container
		EnumWindows(activateProc, (LPARAM)params);
		if (params->container) EnumChildWindows(params->container, activateProc, (LPARAM)params);

		if (!params->container)
		{
			// Ok, it's one of our own, so leave its titlebar selected,
			// and don't send any other windows any WM_NCACTIVATE msgs
mine:		params->wParam = TRUE;
			goto defproc;
		}

		// Sync all other Docking Frame windows to the same state as the one that
		// called DockingActivate
		EnumWindows(syncActivateProc, (LPARAM)params);
		EnumChildWindows(params->container, syncActivateProc, (LPARAM)params);

		// Also sync the container window (if it wasn't the one that called here)
		if (params->container != params->hwnd && params->container != (HWND)params->lParam)
			SendMessage(params->container, WM_NCACTIVATE, params->wParam, (LPARAM)-1);
	}

defproc:
	return(DefWindowProc(params->hwnd, WM_NCACTIVATE, params->wParam, params->lParam));
}






/******************* DockingCountFrames() ********************
 * Counts the number of Docking Frame windows for the passed
 * container window.
 *
 * container =	Handle to container window.
 * onlyDocked =	1 if counting only docked windows. 0 if all
 *				Docking Frames.
 */

static BOOL CALLBACK countProc(HWND hwnd, LPARAM lParam)
{
	DOCKINFO	*dwp;
	
	// Is this window one of the docking frames for the particular container?
	if (GetClassWord(hwnd, GCW_ATOM) == DockingFrameAtom &&
		(dwp = (DOCKINFO *)GetWindowLong(hwnd, GWLP_USERDATA)) &&
		dwp->container == ((DOCKCOUNTPARAMS *)lParam)->container)
	{
		// Increment count
		((DOCKCOUNTPARAMS *)lParam)->count += 1;
	}

	return(TRUE);
}

UINT WINAPI DockingCountFrames(HWND container, UINT onlyDocked)
{
	DOCKCOUNTPARAMS	dockCount;

	dockCount.count = 0;
	dockCount.container = container;

	if (!onlyDocked)
		EnumWindows(countProc, (LPARAM)&dockCount);
	EnumChildWindows(container, countProc, (LPARAM)&dockCount);

	return(dockCount.count);
}







/****************** DockingArrangeWindows() ******************
 * Positions any docked Docking Frame windows in the container
 * window. The container window normally calls this when it
 * receives a WM_SIZE message.
 *
 * container =	Handle to container window.
 * hdwp =		Handle from container window's call to
 *				BeginDeferWindowPos().
 * rect =		Contains the area within the container's
 *				client where windows can be docked.
 *
 * NOTE: Container window must call BeginDeferWindowPos()
 * before calling here, and must call EndDeferWindowPos()
 * when this returns.
 *
 * DockingArrangeWindows() updates the RECT so that it
 * describes the area that is not covered by docked
 * windows. Normally, this would be used to position/size
 * any other, non-dockable windows inside of the container
 * window (for example, a child window containing some
 * "document").
 */

static BOOL CALLBACK layoutProc(HWND hwnd, LPARAM lParam)
{
	DOCKINFO	*dwp;
	
	// Is this window one of our docking frames?
	if (GetClassWord(hwnd, GCW_ATOM) == DockingFrameAtom &&

		// Get its DOCKWINDOW
		(dwp = (DOCKINFO *)GetWindowLong(hwnd, GWLP_USERDATA)))
	{
		// Docked?
//		if (dwp->uDockedState > 0)
//		{
			// For hidden docked windows, don't redraw them
			if (!(dwp->dwStyle & DWS_HIDDEN) && (((DOCKLAYOUTPARAMS *)lParam)->which & dwp->uDockedState))
			{
				unsigned	n;

				n = dwp->nDockedSize;

				switch (dwp->uDockedState & ~0x80)
				{
					case DWS_DOCKED_LEFT:
					{
						DeferWindowPos(((DOCKLAYOUTPARAMS *)lParam)->hdwp, dwp->hwnd, 0, ((DOCKLAYOUTPARAMS *)lParam)->rect->left, ((DOCKLAYOUTPARAMS *)lParam)->rect->top, n, ((DOCKLAYOUTPARAMS *)lParam)->rect->bottom - ((DOCKLAYOUTPARAMS *)lParam)->rect->top, SWP_NOZORDER);
						((DOCKLAYOUTPARAMS *)lParam)->rect->left += n;
						break;
					}

					case DWS_DOCKED_RIGHT:
					{
						DeferWindowPos(((DOCKLAYOUTPARAMS *)lParam)->hdwp, dwp->hwnd, 0, ((DOCKLAYOUTPARAMS *)lParam)->rect->right-n, ((DOCKLAYOUTPARAMS *)lParam)->rect->top, n, ((DOCKLAYOUTPARAMS *)lParam)->rect->bottom - ((DOCKLAYOUTPARAMS *)lParam)->rect->top, SWP_NOZORDER);
						((DOCKLAYOUTPARAMS *)lParam)->rect->right -= n;
						break;
					}

					case DWS_DOCKED_TOP:
					{
						DeferWindowPos(((DOCKLAYOUTPARAMS *)lParam)->hdwp, dwp->hwnd, 0, ((DOCKLAYOUTPARAMS *)lParam)->rect->left, ((DOCKLAYOUTPARAMS *)lParam)->rect->top, ((DOCKLAYOUTPARAMS *)lParam)->rect->right - ((DOCKLAYOUTPARAMS *)lParam)->rect->left, n, SWP_NOZORDER);
						((DOCKLAYOUTPARAMS *)lParam)->rect->top += n;
						break;
					}

					case DWS_DOCKED_BOTTOM:
					{
						DeferWindowPos(((DOCKLAYOUTPARAMS *)lParam)->hdwp, dwp->hwnd, 0, ((DOCKLAYOUTPARAMS *)lParam)->rect->left, ((DOCKLAYOUTPARAMS *)lParam)->rect->bottom-n, ((DOCKLAYOUTPARAMS *)lParam)->rect->right - ((DOCKLAYOUTPARAMS *)lParam)->rect->left, n, SWP_NOZORDER);
						((DOCKLAYOUTPARAMS *)lParam)->rect->bottom -= n;
					}
				}
			}
	//	}
	}

	return(TRUE);
}


void WINAPI DockingArrangeWindows(HWND container, HDWP hdwp, RECT *rect)
{
	DOCKLAYOUTPARAMS	dockParams;

	dockParams.hdwp = hdwp;
	dockParams.rect = rect;

	if (rect->left)
		return;

	// Dock the horizontal bars first (across the TOP+BOTTOM)
	dockParams.which = DWS_DOCKED_TOP|DWS_DOCKED_BOTTOM;
	EnumChildWindows(container, layoutProc, (LPARAM)&dockParams);

	// Dock the vertical Docking Frames now (LEFT and RIGHT)
	dockParams.which = DWS_DOCKED_LEFT|DWS_DOCKED_RIGHT;
	EnumChildWindows(container, layoutProc, (LPARAM)&dockParams);
}







/****************** DockingSavePlacement() ******************
 * Saves a Docking Frame's state and position to a registry
 * key that the caller has created.
 *
 * dwp =	The DOCKINFO struct for the Docking Frame.
 * hKey =	Handle to registry key that caller has created.
 *
 * RETURNS: 0 if success, or an error number.
 *
 * NOTE: Caller must create a unique key for each of his
 * Docking Frame windows.
 */

DWORD WINAPI DockingSavePlacement(DOCKINFO *dwp, HKEY hKey)
{
	DWORD	rc;

	// Store X, Y, Width, and Height in binary format in the "Place" value
	if (!(rc = RegSetValueEx(hKey, &PlaceKey[0], 0, REG_BINARY, (LPBYTE)&dwp->xpos, sizeof(unsigned short) * 5)))
	{
		// Store docked state in "State" value
		rc = RegSetValueEx(hKey, &StateKey[0], 0, REG_BINARY, (LPBYTE)&dwp->uDockedState, 1);
	}

	return(rc);
}





/****************** DockingLoadPlacement() ******************
 * Loads a Docking Frame's state and position from a registry
 * key that was saved via DockingSavePlacement().
 *
 * dwp =	The DOCKINFO struct for the Docking Frame.
 * hKey =	Handle to registry key that caller has opened.
 *
 * NOTE: The DOCKINFO should have been initialized with
 * default values before calling here.
 */

void WINAPI DockingLoadPlacement(DOCKINFO *dwp, HKEY hKey)
{
	DWORD			size, type;
	unsigned char	state;

	size = 1;
	if (!RegQueryValueEx(hKey, &StateKey[0], 0, &type, (LPBYTE)&state, &size) && size == 1 && type == REG_BINARY)
	{
		dwp->uDockedState = (unsigned char)state;
		size = sizeof(unsigned short) * 5;
		RegQueryValueEx(hKey, &PlaceKey[0], 0, 0, (LPBYTE)&dwp->xpos, &size);
	}
}






/****************** DockingIsToolWindow() ******************
 * Determines if a window handle is a tool window belonging to
 * a particular container window.
 *
 * container =	Handle to container window.
 * hwnd =		Handle to window to check.
 *
 * RETURNS: Pointer to the tool window's DOCKINFO if "hwnd"
 * is a tool window belonging to "container", or 0 if not.
 *
 * NOTE: If "container" is 0, then this just checks if "hwnd"
 * is a tool window.
 */

DOCKINFO * WINAPI DockingIsToolWindow(HWND container, HWND hwnd)
{
	DOCKINFO	*dwp;

	// Is this window one of the docking frames for this container?
	if (GetClassWord(hwnd, GCW_ATOM) == DockingFrameAtom &&
		(dwp = (DOCKINFO *)GetWindowLong(hwnd, GWLP_USERDATA)) &&
		(!container || dwp->container == container))
	{
		return(dwp);
	}

	return(0);
}







/***************** DockingDestroyFreeFloat() ******************
 * Destroys all the free floating tool windows belonging to
 * a particular container window.
 *
 * container =	Handle to container window.
 */

static BOOL CALLBACK destroyFloatProc(HWND hwnd, LPARAM lParam)
{
	DOCKINFO	*dwp;
	
	// Is this window one of the docking frames for the particular container,
	// and is it free floating?
	if (GetClassWord(hwnd, GCW_ATOM) == DockingFrameAtom &&
		(dwp = (DOCKINFO *)GetWindowLong(hwnd, GWLP_USERDATA)) &&
		dwp->container == (HWND)lParam &&
		!GetWindow(dwp->hwnd, GW_OWNER))
	{
		// Call its DockClose callback, but ignore the return
		(*dwp->DockClose)(dwp);

		// Destroy the window
		DestroyWindow(dwp->hwnd);
	}

	return(TRUE);
}

void WINAPI DockingDestroyFreeFloat(HWND container)
{
	EnumWindows(destroyFloatProc, (LPARAM)container);
}






/****************************** DllMain() *****************************
 * Automatically called by the operating system when the DLL is
 * loaded/unloaded.
 */

// NOTE: For the release version, we don't link with any C startup code, which we
// don't need because we don't use any C library functions. So this reduces DLL
// size.
#ifndef _DEBUG
BOOL WINAPI _DllMainCRTStartup(HANDLE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
#else
BOOL WINAPI DllMain(HANDLE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)  /* <--- Doesn't replace startup code */
#endif
{
    switch(fdwReason)
	{
		// ==============================================================
		case DLL_PROCESS_ATTACH:
		{
			// Save the module handle. It will be the same for all instances of the DLL
			ThisModule = hinstDLL;

			// Not dragging a Docking Frame right now
			DraggingDockWnd = 0;

			// Register window class for the Docking Frame.
			// Its message procedure is dockWndProc
			{
			WNDCLASSEX	wc;

			ZeroMemory(&wc, sizeof(wc));
			wc.cbSize = sizeof(WNDCLASSEX);
		//	wc.style = 0;
			wc.lpfnWndProc = dockWndProc;
		//	wc.cbClsExtra = wc.cbWndExtra = 0;
			wc.hInstance = ThisModule;
		//	wc.hIcon = 0;
			wc.hCursor = LoadCursor(NULL, IDC_ARROW);
			wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
		//	wc.lpszMenuName = 0;
			wc.lpszClassName = &DockClassName[0];
		//	wc.hIconSm = 0;

			if (!(DockingFrameAtom = RegisterClassEx(&wc)))
			{
				TCHAR		buffer[160];

				FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), &buffer[0], 160, 0);
				MessageBox(0, &buffer[0], &DockClassName[0], MB_OK);
				return(0);
			}
			}

			break;
		}

		// ==============================================================
		case DLL_THREAD_ATTACH:
		{
			// We don't need to do anything for THREAD ATTACH, so we can
			// disable this support.
			DisableThreadLibraryCalls(hinstDLL);
			break;
		}

//		case DLL_THREAD_DETACH:
//			break;

		// ==============================================================
		case DLL_PROCESS_DETACH:
		{
			// Unregister docking frame window
			UnregisterClass(&DockClassName[0], ThisModule);
		}
	}

	// Success
	return(1);
}
