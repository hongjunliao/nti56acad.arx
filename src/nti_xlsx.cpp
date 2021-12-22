/*!
 * This file is PART of nti56acad.arx project
 * @author hongjun.liao <docici@126.com>, @date 2020/11/10
 *
 * .xlsx file process
 * */
#if 0



#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <assert.h>
#include "hp/sdsinc.h" //sds
#include "hp/hp_log.h"			/* hp_log */
#include "nti_xlsx.h"
#include "xlsxio_read.h"

 /////////////////////////////////////////////////////////////////////////////////////

static int nti_import_from_excel_row(xlsxioreadersheet sheet)
{
	assert(sheet);
	int i, j;
	int n = 0;
	int ver = 1;

	printf("%s: ____begin sheet\n", __FUNCTION__);
	for (i = 0; xlsxioread_sheet_next_row(sheet); ++i) {
		//read all columns
		sds v[128] = { 0 };
		char * value = 0;
		for (j = 0; j < sizeof(v) / sizeof(v[0]); ++j){
			if(j== 0 || value){
				value = xlsxioread_sheet_next_cell(sheet);
			}
			v[j] = sdsnew(value);
			xlsxioread_free(value);

			fprintf(stdout, "'%s'%c", v[j], (j < 16? ',' : '\n'));
			fflush(stdout);
		}
		++n;
		for (j = 0; j < sizeof(v) / sizeof(v[0]);++j)
			sdsfree(v[j]);
	}
	printf("%s: ___end sheet\n", __FUNCTION__); 

	return n;
}

/*
 * @return: count of rows
 * */
int nti_import_from_excel(char const * path, sds * errstr)
{
	if (!(path))
		return -1;

	int r;
	//open .xlsx file for reading
	xlsxioreader xlsxioread;
	if ((xlsxioread = xlsxioread_open(path)) == NULL) {
		hp_log(stderr,
				"%s: Error opening .xlsx file: '%s'\n", __FUNCTION__, path);

		if (errstr)
			*errstr = sdscatprintf(*errstr, "Error opening .xlsx file: '%s'",
					path);
		return -2;
	}

	//read values from first sheet
	xlsxioreadersheet sheet;
	const char* sheetname = NULL;
	if ((sheet = xlsxioread_sheet_open(xlsxioread, sheetname,
			XLSXIOREAD_SKIP_EMPTY_ROWS)) != NULL) {
		
		r = nti_import_from_excel_row(sheet);
		xlsxioread_sheet_close(sheet);

		if(r == 0){
			hp_log(stderr, "%s: empty excel: '%s'\n", __FUNCTION__, path);
			r = 0;
		}
	}
	else r = -1;

	//clean up
	xlsxioread_close(xlsxioread);

	return r;
}

/////////////////////////////////////////////////////////////////////////////////////////
#ifndef NDEBUG

int test_nti_xlsx_main(int argc, char ** argv)
{
    return -1;
}
/////////////////////////////////////////////////////////////////////////////////////////
#endif /* NDEBUG */



#endif