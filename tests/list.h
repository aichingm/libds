#include "acutest.h"

#include "src/list.h"

#define LIST_TESTS \
    { "list length", test_list_len }, \
    { "list push", test_list_push }, \
    { "list pop", test_list_pop }, \
    { "list unshift", test_list_unshift }, \
    { "list shift", test_list_shift }, \
    { "list get", test_list_get }, \
    { "list remove index", test_list_remove_index }, \
    { "list contains", test_list_contains }, \
    { "list to array", test_list_to_array }, \
    { "list data to array", test_list_data_to_array }, \
    { "list foreach", test_list_foreach }, \
    { "list foreach reverse", test_list_foreach_reverse }, \
    { "list macros", test_list_macros }, \
    { "listitem macros", test_listitem_macros }, \
    { "listitem in list", test_listitem_in_list }, \
    { "listitem unlink", test_listitem_unlink }, \
    { "listitem placement", test_listitem_placement }

#define ZERO(x) x={0}

typedef struct {
    char* data;
    listitem_t item;
} explicit_item_t;

typedef struct {
    char data[7];
      LISTITEM_PROP();
} item_at_start_t;

typedef struct {
    char data[23];
      LISTITEM_PROP();
    char* data2;
} item_in_middle_t;

typedef struct {
    char* data;
      LISTITEM_PROP();
} item_at_end_t;

typedef struct {
    int x;
      LISTITEM_PROP();
} item_t;

typedef struct {
    char name[32];
    unsigned char age;
    listitem_t explicit;
      LISTITEM_PROP();
      LISTITEM_PROP_s(named);
} push_variants_t;

void test_list_len() {
    explicit_item_t ZERO(a), ZERO(b), ZERO(c);
    list_t ZERO(l);

    TEST_ASSERT(list_len(&l) == 0);
    list_push(&l, &a.item);
    TEST_ASSERT(list_len(&l) == 1);
    list_push(&l, &b.item);
    TEST_ASSERT(list_len(&l) == 2);
    list_push(&l, &c.item);
    TEST_ASSERT(list_len(&l) == 3);

}

void test_list_pop() {
    explicit_item_t ZERO(a), ZERO(b), ZERO(c);
    list_t ZERO(l);

    TEST_ASSERT(list_pop(&l) == NULL);

    list_push(&l, &a.item);
    TEST_ASSERT(list_len(&l) == 1);

    TEST_ASSERT(list_pop(&l) == &a.item);
    TEST_ASSERT(list_len(&l) == 0);

    list_push(&l, &a.item);
    list_push(&l, &b.item);
    TEST_ASSERT(list_len(&l) == 2);

    TEST_ASSERT(list_pop(&l) == &b.item);
    TEST_ASSERT(list_pop(&l) == &a.item);
    TEST_ASSERT(list_len(&l) == 0);

    list_push(&l, &a.item);
    list_push(&l, &b.item);
    list_push(&l, &c.item);
    TEST_ASSERT(list_len(&l) == 3);

    TEST_ASSERT(list_pop(&l) == &c.item);
    TEST_ASSERT(list_pop(&l) == &b.item);
    TEST_ASSERT(list_pop(&l) == &a.item);
    TEST_ASSERT(list_len(&l) == 0);

}

void test_list_push(void) {

    push_variants_t ZERO(a);
    list_t l;
    list_init(&l);

    list_push(&l, &a.explicit);
    list_push(&l, LISTITEM_OF(push_variants_t, &a));
    list_push(&l, LISTITEM_OF_s(push_variants_t, &a, named));

    TEST_CHECK(list_len(&l) == 3);

    TEST_ASSERT(list_pop(&l) == LISTITEM_OF_s(push_variants_t, &a, named));
    TEST_ASSERT(list_pop(&l) == LISTITEM_OF(push_variants_t, &a));
    TEST_ASSERT(list_pop(&l) == &a.explicit);

}

void test_listitem_placement() {
    item_at_start_t ZERO(a);
    item_in_middle_t ZERO(b);
    item_at_end_t ZERO(c);
    list_t ZERO(l);

    list_push(&l, LISTITEM_OF(item_at_start_t, &a));
    TEST_ASSERT(list_pop(&l) == LISTITEM_OF(item_at_start_t, &a));

    list_push(&l, LISTITEM_OF(item_in_middle_t, &b));
    TEST_ASSERT(list_pop(&l) == LISTITEM_OF(item_in_middle_t, &b));
    list_push(&l, LISTITEM_OF(item_at_end_t, &c));
    TEST_ASSERT(list_pop(&l) == LISTITEM_OF(item_at_end_t, &c));

}

void test_listitem_in_list() {
    explicit_item_t ZERO(a);
    list_t ZERO(l);

    TEST_ASSERT(!listitem_in_list(&a.item));
    list_push(&l, LISTITEM_OF_s(explicit_item_t, &a, item));
    TEST_ASSERT(listitem_in_list(&a.item));
    list_pop(&l);
    TEST_ASSERT(!listitem_in_list(&a.item));
}

struct foreach_array_userdata {
    unsigned int index;
    void* array[4];
};

void collect_to_array(void* element, void* userdata) {
    struct foreach_array_userdata* a = userdata;
    a->array[a->index++] = element;
}

void collect_item_to_array(listitem_t* element, void* userdata) {
    struct foreach_array_userdata* a = userdata;
    a->array[a->index++] = element;
}

void test_list_foreach() {
    item_t ZERO(a), ZERO(b), ZERO(c), ZERO(d);
    list_t l;
    list_init(&l);

    list_push(&l, LISTITEM_OF(item_t, &a));
    list_push(&l, LISTITEM_OF(item_t, &b));
    list_push(&l, LISTITEM_OF(item_t, &c));
    list_push(&l, LISTITEM_OF(item_t, &d));

    struct foreach_array_userdata collector = { 0 };
    list_foreach_reverse(&l, collect_item_to_array, &collector);

    TEST_ASSERT(collector.array[0] == &d.default_list_item_name);
    TEST_ASSERT(collector.array[1] == &c.default_list_item_name);
    TEST_ASSERT(collector.array[2] == &b.default_list_item_name);
    TEST_ASSERT(collector.array[3] == &a.default_list_item_name);

}

void test_list_foreach_reverse() {
    item_t ZERO(a), ZERO(b), ZERO(c), ZERO(d);
    list_t l;
    list_init(&l);

    list_push(&l, LISTITEM_OF(item_t, &a));
    list_push(&l, LISTITEM_OF(item_t, &b));
    list_push(&l, LISTITEM_OF(item_t, &c));
    list_push(&l, LISTITEM_OF(item_t, &d));

    struct foreach_array_userdata collector = { 0 };
    list_foreach_reverse(&l, collect_item_to_array, &collector);

    TEST_ASSERT(collector.array[0] == &d.default_list_item_name);
    TEST_ASSERT(collector.array[1] == &c.default_list_item_name);
    TEST_ASSERT(collector.array[2] == &b.default_list_item_name);
    TEST_ASSERT(collector.array[3] == &a.default_list_item_name);

}

void test_list_to_array() {
    item_t ZERO(a), ZERO(b), ZERO(c), ZERO(d);
    list_t l;
    list_init(&l);

    list_push(&l, LISTITEM_OF(item_t, &a));
    list_push(&l, LISTITEM_OF(item_t, &b));
    list_push(&l, LISTITEM_OF(item_t, &c));
    list_push(&l, LISTITEM_OF(item_t, &d));

    listitem_t** array_heap = malloc(sizeof(listitem_t *) * 4);
    list_to_array(&l, array_heap);

    TEST_ASSERT(array_heap[0] == LISTITEM_OF(item_t, &a));
    TEST_ASSERT(array_heap[1] == LISTITEM_OF(item_t, &b));
    TEST_ASSERT(array_heap[2] == LISTITEM_OF(item_t, &c));
    TEST_ASSERT(array_heap[3] == LISTITEM_OF(item_t, &d));

    free(array_heap);

    listitem_t* array_stack[4];
    list_to_array(&l, array_stack);

    TEST_ASSERT(array_stack[0] == LISTITEM_OF(item_t, &a));
    TEST_ASSERT(array_stack[1] == LISTITEM_OF(item_t, &b));
    TEST_ASSERT(array_stack[2] == LISTITEM_OF(item_t, &c));
    TEST_ASSERT(array_stack[3] == LISTITEM_OF(item_t, &d));

}

void test_list_data_to_array() {
    item_t ZERO(a), ZERO(b), ZERO(c), ZERO(d);
    list_t l;
    list_init(&l);

    list_push(&l, LISTITEM_OF(item_t, &a));
    list_push(&l, LISTITEM_OF(item_t, &b));
    list_push(&l, LISTITEM_OF(item_t, &c));
    list_push(&l, LISTITEM_OF(item_t, &d));

    void** array_heap = malloc(sizeof(void *) * 4);
    list_data_to_array(&l, array_heap, LISTITEM_OFFSET(item_t));

    TEST_ASSERT(array_heap[0] == &a);
    TEST_ASSERT(array_heap[1] == &b);
    TEST_ASSERT(array_heap[2] == &c);
    TEST_ASSERT(array_heap[3] == &d);

    free(array_heap);

    void* array_stack[4];
    list_data_to_array(&l, array_stack, LISTITEM_OFFSET(item_t));

    TEST_ASSERT(array_stack[0] == &a);
    TEST_ASSERT(array_stack[1] == &b);
    TEST_ASSERT(array_stack[2] == &c);
    TEST_ASSERT(array_stack[3] == &d);

}

void test_list_unshift() {
    item_t ZERO(a), ZERO(b), ZERO(c), ZERO(d);
    list_t l;
    list_init(&l);

    list_unshift(&l, LISTITEM_OF(item_t, &a));
    list_unshift(&l, LISTITEM_OF(item_t, &b));
    list_unshift(&l, LISTITEM_OF(item_t, &c));
    list_unshift(&l, LISTITEM_OF(item_t, &d));

    TEST_ASSERT(list_pop(&l) == LISTITEM_OF(item_t, &a));
    TEST_ASSERT(list_pop(&l) == LISTITEM_OF(item_t, &b));
    TEST_ASSERT(list_pop(&l) == LISTITEM_OF(item_t, &c));
    TEST_ASSERT(list_pop(&l) == LISTITEM_OF(item_t, &d));

}

void test_list_shift() {
    item_t ZERO(a), ZERO(b), ZERO(c), ZERO(d);
    list_t l;
    list_init(&l);

    list_push(&l, LISTITEM_OF(item_t, &a));
    list_push(&l, LISTITEM_OF(item_t, &b));
    list_push(&l, LISTITEM_OF(item_t, &c));
    list_push(&l, LISTITEM_OF(item_t, &d));

    TEST_ASSERT(list_shift(&l) == LISTITEM_OF(item_t, &a));
    TEST_ASSERT(list_shift(&l) == LISTITEM_OF(item_t, &b));
    TEST_ASSERT(list_shift(&l) == LISTITEM_OF(item_t, &c));
    TEST_ASSERT(list_shift(&l) == LISTITEM_OF(item_t, &d));

}

void test_list_get() {
    item_t ZERO(a), ZERO(b), ZERO(c), ZERO(d);
    list_t l;
    list_init(&l);

    TEST_ASSERT(list_get(&l, -1) == NULL);
    TEST_ASSERT(list_get(&l, 0) == NULL);
    TEST_ASSERT(list_get(&l, -1) == NULL);

    list_push(&l, LISTITEM_OF(item_t, &a));
    TEST_ASSERT(list_get(&l, 0) == LISTITEM_OF(item_t, &a));

    list_push(&l, LISTITEM_OF(item_t, &b));
    TEST_ASSERT(list_get(&l, 1) == LISTITEM_OF(item_t, &b));

    list_push(&l, LISTITEM_OF(item_t, &c));
    TEST_ASSERT(list_get(&l, 2) == LISTITEM_OF(item_t, &c));

    list_push(&l, LISTITEM_OF(item_t, &d));
    TEST_ASSERT(list_get(&l, 3) == LISTITEM_OF(item_t, &d));

    TEST_ASSERT(list_get(&l, 4) == NULL);
}

void test_list_remove_index() {
    item_t ZERO(a), ZERO(b), ZERO(c), ZERO(d), ZERO(f);
    list_t l;
    list_init(&l);

    TEST_ASSERT(list_remove_index(&l, -1) == NULL);
    TEST_ASSERT(list_remove_index(&l, 0) == NULL);
    TEST_ASSERT(list_remove_index(&l, -1) == NULL);

    list_push(&l, LISTITEM_OF(item_t, &a));
    list_push(&l, LISTITEM_OF(item_t, &b));
    list_push(&l, LISTITEM_OF(item_t, &c));
    list_push(&l, LISTITEM_OF(item_t, &d));
    list_push(&l, LISTITEM_OF(item_t, &f));

    TEST_ASSERT(list_remove_index(&l, 5) == NULL);

    TEST_ASSERT(list_remove_index(&l, 0) == LISTITEM_OF(item_t, &a));	// remove start
    TEST_ASSERT(list_remove_index(&l, 0) == LISTITEM_OF(item_t, &b));	// remove new start
    TEST_ASSERT(list_remove_index(&l, 1) == LISTITEM_OF(item_t, &d));	// remove middle
    TEST_ASSERT(list_remove_index(&l, 1) == LISTITEM_OF(item_t, &f));	// remove end
    TEST_ASSERT(list_remove_index(&l, 0) == LISTITEM_OF(item_t, &c));	// remove last
    TEST_ASSERT(list_len(&l) == 0);

}

void test_list_contains() {
    item_t ZERO(a), ZERO(b), ZERO(c), ZERO(d);
    list_t l;
    list_init(&l);

    list_push(&l, LISTITEM_OF(item_t, &a));
    list_push(&l, LISTITEM_OF(item_t, &c));

    TEST_ASSERT(list_contains(&l, LISTITEM_OF(item_t, &a)));

    TEST_ASSERT(list_contains(&l, LISTITEM_OF(item_t, &c)));

    TEST_ASSERT(!list_contains(&l, LISTITEM_OF(item_t, &b)));

    TEST_ASSERT(!list_contains(&l, LISTITEM_OF(item_t, &d)));

    list_remove_index(&l, 0);
    list_push(&l, LISTITEM_OF(item_t, &d));

    TEST_ASSERT(!list_contains(&l, LISTITEM_OF(item_t, &a)));

    TEST_ASSERT(list_contains(&l, LISTITEM_OF(item_t, &c)));

    TEST_ASSERT(!list_contains(&l, LISTITEM_OF(item_t, &b)));

    TEST_ASSERT(list_contains(&l, LISTITEM_OF(item_t, &d)));

}

void test_list_macros() {

    push_variants_t ZERO(a);
    push_variants_t* x;
    push_variants_t* y;

    list_t l;
    list_init(&l);

    // test get operations on empty lists

    x = LIST_POP(push_variants_t, &l);
    TEST_ASSERT(x == NULL);

    x = LIST_SHIFT(push_variants_t, &l);
    TEST_ASSERT(x == NULL);

    x = LIST_GET(push_variants_t, &l, 0);
    TEST_ASSERT(x == NULL);

    // POP
    list_push(&l, LISTITEM_OF(push_variants_t, &a));
    list_push(&l, LISTITEM_OF_s(push_variants_t, &a, named));

    x = LIST_POP_s(push_variants_t, &l, named);
    y = LIST_POP(push_variants_t, &l);

    TEST_ASSERT(x == &a);
    TEST_ASSERT(y == &a);

    // SHIFT
    list_push(&l, LISTITEM_OF(push_variants_t, &a));
    list_push(&l, LISTITEM_OF_s(push_variants_t, &a, named));

    x = LIST_SHIFT(push_variants_t, &l);
    y = LIST_SHIFT_s(push_variants_t, &l, named);

    TEST_ASSERT(x == &a);
    TEST_ASSERT(y == &a);

    // GET
    list_push(&l, LISTITEM_OF(push_variants_t, &a));
    list_push(&l, LISTITEM_OF_s(push_variants_t, &a, named));

    x = LIST_GET(push_variants_t, &l, 0);
    y = LIST_GET_s(push_variants_t, &l, 1, named);

    TEST_ASSERT(x == &a);
    TEST_ASSERT(y == &a);

}

void test_listitem_macros() {

    push_variants_t ZERO(a);
    list_t l;
    list_init(&l);

    list_push(&l, LISTITEM_OF(push_variants_t, &a));
    list_push(&l, LISTITEM_OF_s(push_variants_t, &a, named));

    // LISTITEM_AS
    push_variants_t* ptr_to_a0 = LISTITEM_AS(push_variants_t, list_get(&l, 0));
    push_variants_t* ptr_to_a1 = LISTITEM_AS_s(push_variants_t, list_get(&l, 1), named);

    TEST_ASSERT(ptr_to_a0 == &a);
    TEST_ASSERT(ptr_to_a1 == &a);

    // LISTITEM_OFFSET
    size_t diff0 = LISTITEM_OFFSET(push_variants_t);
    size_t diff1 = LISTITEM_OFFSET_s(push_variants_t, named);

    TEST_ASSERT((listitem_t *)(((char *)&a) + diff0) == LISTITEM_OF(push_variants_t, &a));
    TEST_ASSERT((listitem_t *)(((char *)&a) + diff1) == LISTITEM_OF_s(push_variants_t, &a, named));

}

void test_listitem_unlink() {
    item_t ZERO(a), ZERO(b), ZERO(c);
    list_t l;
    list_init(&l);

    list_push(&l, LISTITEM_OF(item_t, &a));
    list_push(&l, LISTITEM_OF(item_t, &b));
    list_push(&l, LISTITEM_OF(item_t, &c));

    listitem_unlink(LISTITEM_OF(item_t, &b));
    TEST_ASSERT(!list_contains(&l, LISTITEM_OF(item_t, &b)));

    TEST_ASSERT(list_len(&l) == 2);

}

