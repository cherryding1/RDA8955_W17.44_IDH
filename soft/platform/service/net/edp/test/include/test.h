#ifndef __TEST_H__
#define __TEST_H__

#include "dbg.h"

#define TEST_LOG(format, ...) \
        do { \
		    dbg_TraceOutputText(0, (format), ##__VA_ARGS__); \
		} while(0)

/* Terminate current test with error */
#define fail()	return __LINE__

/* Successful end of the test case */
#define done() return 0

/* Check single condition */
#define check(cond) do { if (!(cond)) fail(); } while (0)

int test_all_for_json(void);

#endif /* __TEST_H__ */
