#ifndef __TESTS_H
#define __TESTS_H

/* Implements TDD for the Virtual CR001
 * Tests are run from a .test file, named the same as the hexfile
 * so program.hex will find its tests in program.test
 */

void test_run(const char *hexfile);

#endif