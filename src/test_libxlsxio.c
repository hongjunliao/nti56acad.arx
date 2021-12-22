 /* This file is PART of xhmdm project
 * @author hongjun.liao <docici@126.com>, @date 2019/11/11
 *
 * tests for libxlsxwriter
 *
 * */
#if 0


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>	       /* malloc */
#include <errno.h>         /* errno */
#include <string.h>	       /* strncmp */
#include <ctype.h>	       /* isblank */
#include <limits.h>	       /* INT_MAX, IOV_MAX */
#include <assert.h>        /* define NDEBUG to disable assertion */
#include "hp/sdsinc.h"        /* sds */

#include "xlsxio_read.h"
#include "xlsxio_write.h"
/////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////
#ifndef NDEBUG

int test_libxlsxio_read_main(int argc, char ** argv)
{
	//open .xlsx file for reading
	char const * filename = "test/test_libxlsxio_read.xlsx";
	xlsxioreader xlsxioread;
	if ((xlsxioread = xlsxioread_open(filename)) == NULL) {
	  fprintf(stderr, "Error opening .xlsx file: '%s'\n", filename);
	  return 1;
	}

	//read values from first sheet
	char* value;
	xlsxioreadersheet sheet;
	const char* sheetname = NULL;
	printf("Contents of first sheet:\n");
	if ((sheet = xlsxioread_sheet_open(xlsxioread, sheetname, XLSXIOREAD_SKIP_EMPTY_ROWS)) != NULL) {
	  //read all rows
	  while (xlsxioread_sheet_next_row(sheet)) {
	    //read all columns
	    while ((value = xlsxioread_sheet_next_cell(sheet)) != NULL) {
	      printf("%s\t", value);
		  xlsxioread_free(value);
	    }
	    printf("\n");
	  }
	  xlsxioread_sheet_close(sheet);
	}

	//clean up
	xlsxioread_close(xlsxioread);

	return 0;
}

int test_libxlsxio_write_main(int argc, char ** argv)
{
	//open .xlsx file for writing (will overwrite if it already exists)
	char const * filename = "test/test_libxlsxio_write.xlsx";
	xlsxiowriter handle;
	if ((handle = xlsxiowrite_open(filename, "MySheet")) == NULL) {
		fprintf(stderr, "Error creating .xlsx file\n");
		return 1;
	}

	//write column names
	xlsxiowrite_add_column(handle, "Col1", 16);
	xlsxiowrite_add_column(handle, "Col2", 0);
	xlsxiowrite_next_row(handle);

	//write data
	int i;
	for (i = 0; i < 1000; i++) {
		xlsxiowrite_add_cell_string(handle, "Test");
		xlsxiowrite_add_cell_int(handle, i);
		xlsxiowrite_next_row(handle);
	}

	//close .xlsx file
	xlsxiowrite_close(handle);

	return 0;
}


int test_libxlsxio_main(int argc, char ** argv)
{
	int r;
	r = test_libxlsxio_read_main(argc, argv);
	assert(r == 0);
	r = test_libxlsxio_write_main(argc, argv);
	assert(r == 0);
	return r;
}

#endif /* NDEBUG */
#endif