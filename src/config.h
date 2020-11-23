/*!
 * This file is PART of nti56acad project
 * @author hongjun.liao <docici@126.com>, @date 2020/09/11
 *
 * config
 * */

#ifndef CONFIG_H
#define CONFIG_H

#ifdef HAVE_CONFIG_H

/* define to enable GNU extension */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

/* define to disable debug */
#ifndef NDEBUG
#endif /* NDEBUG */

/* the Git commit id */
#ifndef GIT_COMMIT_ID
#define GIT_COMMIT_ID "HEAD"
#endif /* GIT_COMMIT_ID */

/* the Git branch name */
#ifndef GIT_BRANCH_NAME
#define GIT_BRANCH_NAME "(unknown)"
#endif /* GIT_BRANCH_NAME */

#ifndef IMGUI_IMPL_OPENGL_LOADER_GLAD
#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#endif

//#define  NTI_USE_OPENGL

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define  _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#endif

#ifndef _AFX_ALL_WARNINGS
#define _AFX_ALL_WARNINGS
#endif

#endif /* HAVE_CONFIG_H */

#endif /* CONFIG_H */
