#include "tests/acutest.h"

// include implementations

#define IMPL_LIST
#include "src/list.h"

#define IMPL_HMAP
#include "src/hmap.h"

// include tests
#include "tests/list.h"
#include "tests/hmap.h"

TEST_LIST = {
    LIST_TESTS,
    HMAP_TESTS,
    {NULL, NULL}
};

