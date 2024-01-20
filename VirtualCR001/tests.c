#include "tests.h"
#include "ui.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int test_get_lastchar(const char *hexfile, const char c) {
    int lastchar = -1;
    char *ptr = (char *)hexfile;
    
    while (*ptr++) {
        if (*ptr == c) {
            lastchar = ptr - hexfile;
        }
    }

    return lastchar;
}

char *test_getfile(const char *hexfile) {
    int lastdot = test_get_lastchar(hexfile, '.');
    int hexfilename_length = strlen(hexfile);
    int newlength = 0;

    if (lastdot == -1) {
        newlength = hexfilename_length + 5;
        lastdot = hexfilename_length;
    } else {
        newlength = lastdot + 5;
    }

    char *newfilename = malloc(newlength + 1);
    strncpy(newfilename, hexfile, newlength - 5);
    strncpy(newfilename + lastdot, ".test", 6);

    return newfilename;
}

void test_run(const char *hexfile) {
    ui_writelog(WARNLOG, "Running tests...\n");

    char *testfile = test_getfile(hexfile);

    ui_writelog(WARNLOG, "Loading tests in %s\n", testfile);
    FILE *fp_testfile = fopen(testfile, "r");
    if (fp_testfile == NULL) {
        ui_writelog(WARNLOG, "Could not open test file %s: %s\n", testfile, strerror(errno));
        free(testfile);
        return;
    }

    

    free(testfile);
}