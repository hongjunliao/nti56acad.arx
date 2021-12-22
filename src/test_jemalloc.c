/*!
 * This file is PART of nti56acad project
 * @author hongjun.liao <docici@126.com>, @date 2020/09/11
 *
 * main
 * */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <assert.h>
#ifdef __cplusplus
extern "C" {
#endif
#include "redis/src/zmalloc.h"
#include "jemalloc/jemalloc.h"
#ifdef __cplusplus
}
#endif

static void outOfMemoryHandler(size_t allocation_size) {}

/////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char ** argv)
{
	zmalloc_enable_thread_safeness();
	zmalloc_set_oom_handler(outOfMemoryHandler);

	void * ptr = je_calloc(1, sizeof(int));
	assert(ptr);
	je_free(ptr);

	return 0;
}