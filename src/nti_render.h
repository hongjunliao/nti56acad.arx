/*!
 * This file is PART of nti56acad project
 * @author hongjun.liao <docici@126.com>, @date 2020/09/11
 *
 * imgui windows
 * */
#ifndef NTI_RENDER_H
#define NTI_RENDER_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "nti_imgui.h"
/////////////////////////////////////////////////////////////////////////////////////
void nti_tabswnd_render(nti_imgui_wnddata * wnddata);
void nti_tabswnd_reactor(nti_imgui_wnddata * wnddata);
void nti_tabswnd_simple(nti_imgui_wnddata * wnddata);
void nti_tabswnd_another(nti_imgui_wnddata * wnddata);
void sample_render(nti_imgui_wnddata * wnddata);
void nti_render_blocks(nti_imgui_wnddata * wnddata);
#endif //NTI_RENDER_H
