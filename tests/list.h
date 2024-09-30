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
    { "list foreach", test_list_foreach }, \
    { "list foreach reverse", test_list_foreach_reverse }, \
    { "listitem used", test_listitem_used }, \
    { "listitem remove", test_listitem_remove }, \
    { "listitem placement", test_listitem_placement }, \
    { "listitem ptr", test_listitem_ptr_macro }

#define ZERO(x) x={0}

typedef struct {
    char* data;
    listitem_t item;
} explicit_item_t;

typedef struct {
    char data[7];
    LISTITEM();
} item_at_start_t;

typedef struct {
    char data[23];
    LISTITEM();
    char* data2;
} item_in_middle_t;

typedef struct {
    char* data;
    LISTITEM();
} item_at_end_t;

typedef struct {
    int x;
    LISTITEM();
} item_t;

void test_list_len() {
    explicit_item_t ZERO(a), ZERO(b), ZERO(c);
    list_t ZERO(l);

    TEST_ASSERT(list_len(&l) == 0);
    list_push(&l, &a, offsetof(explicit_item_t, item));
    TEST_ASSERT(list_len(&l) == 1);
    list_push(&l, &b, offsetof(explicit_item_t, item));
    TEST_ASSERT(list_len(&l) == 2);
    list_push(&l, &c, offsetof(explicit_item_t, item));
    TEST_ASSERT(list_len(&l) == 3);

}

void test_list_pop() {
    explicit_item_t ZERO(a), ZERO(b), ZERO(c);
    list_t ZERO(l);

    TEST_ASSERT(a.item.state == 0);
    TEST_ASSERT(list_pop(&l) == NULL);

    list_push(&l, &a, offsetof(explicit_item_t, item));
    TEST_ASSERT(list_len(&l) == 1);

    TEST_ASSERT(list_pop(&l) == &a);
    TEST_ASSERT(list_len(&l) == 0);

    list_push(&l, &a, offsetof(explicit_item_t, item));
    list_push(&l, &b, offsetof(explicit_item_t, item));
    TEST_ASSERT(list_len(&l) == 2);

    TEST_ASSERT(list_pop(&l) == &b);
    TEST_ASSERT(list_pop(&l) == &a);
    TEST_ASSERT(list_len(&l) == 0);

    list_push(&l, &a, offsetof(explicit_item_t, item));
    list_push(&l, &b, offsetof(explicit_item_t, item));
    list_push(&l, &c, offsetof(explicit_item_t, item));
    TEST_ASSERT(list_len(&l) == 3);

    TEST_ASSERT(list_pop(&l) == &c);
    TEST_ASSERT(list_pop(&l) == &b);
    TEST_ASSERT(list_pop(&l) == &a);
    TEST_ASSERT(list_len(&l) == 0);

}

typedef struct {
    char name[32];
    unsigned char age;
    listitem_t explicit;
    LISTITEM();
    LISTITEM_s(named);
} push_variants_t;

void test_list_push(void) {

    push_variants_t ZERO(a);
    list_t l;
    list_init(&l);

    list_push(&l, &a, offsetof(push_variants_t, explicit));
    list_push(&l, LISTITEM_OF(push_variants_t, &a));
    list_push(&l, LISTITEM_OF_s(push_variants_t, &a, named));

    push_variants_t* x = LISTITEM_AS(push_variants_t, list_get_item(&l, 1));

    TEST_ASSERT(x == &a);


    TEST_CHECK(list_len(&l) == 3);

    TEST_ASSERT(list_pop(&l) == &a);
    TEST_ASSERT(list_pop(&l) == &a);
    TEST_ASSERT(list_pop(&l) == &a);

}

void test_listitem_placement() {
    item_at_start_t ZERO(a);
    item_in_middle_t ZERO(b);
    item_at_end_t ZERO(c);
    list_t ZERO(l);

    list_push(&l, LISTITEM_OF(item_at_start_t, &a));
    TEST_ASSERT(list_pop(&l) == &a);

    list_push(&l, LISTITEM_OF(item_in_middle_t, &b));
    TEST_ASSERT(list_pop(&l) == &b);
    list_push(&l, LISTITEM_OF(item_at_end_t, &c));
    TEST_ASSERT(list_pop(&l) == &c);

}

void test_listitem_used() {
    explicit_item_t ZERO(a);
    list_t ZERO(l);

    TEST_ASSERT(!listitem_used(&a.item));
    list_push(&l, LISTITEM_OF_s(explicit_item_t, &a, item));
    TEST_ASSERT(listitem_used(&a.item));
    list_pop(&l);
    TEST_ASSERT(!listitem_used(&a.item));
}

void test_listitem_ptr_macro() {
    push_variants_t ZERO(a);

    TEST_ASSERT(LISTITEM_PTR(push_variants_t, &a) == &a.default_list_item_name);
    TEST_ASSERT(LISTITEM_PTR_s(push_variants_t, &a, named) == &a.named);
    TEST_ASSERT(LISTITEM_PTR_s(push_variants_t, &a, explicit) == &a.explicit);
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
    list_foreach(&l, collect_to_array, &collector);

    TEST_ASSERT(collector.array[0] == &a);
    TEST_ASSERT(collector.array[1] == &b);
    TEST_ASSERT(collector.array[2] == &c);
    TEST_ASSERT(collector.array[3] == &d);

    struct foreach_array_userdata item_collector = { 0 };
    list_foreach_reverse_item(&l, collect_item_to_array, &item_collector);

    TEST_ASSERT(item_collector.array[0] == &d.default_list_item_name);
    TEST_ASSERT(item_collector.array[1] == &c.default_list_item_name);
    TEST_ASSERT(item_collector.array[2] == &b.default_list_item_name);
    TEST_ASSERT(item_collector.array[3] == &a.default_list_item_name);

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
    list_foreach_reverse(&l, collect_to_array, &collector);

    TEST_ASSERT(collector.array[0] == &d);
    TEST_ASSERT(collector.array[1] == &c);
    TEST_ASSERT(collector.array[2] == &b);
    TEST_ASSERT(collector.array[3] == &a);

    struct foreach_array_userdata item_collector = { 0 };
    list_foreach_reverse_item(&l, collect_item_to_array, &item_collector);

    TEST_ASSERT(item_collector.array[0] == &d.default_list_item_name);
    TEST_ASSERT(item_collector.array[1] == &c.default_list_item_name);
    TEST_ASSERT(item_collector.array[2] == &b.default_list_item_name);
    TEST_ASSERT(item_collector.array[3] == &a.default_list_item_name);

}

void test_list_to_array() {
    item_t ZERO(a), ZERO(b), ZERO(c), ZERO(d);
    list_t l;
    list_init(&l);

    list_push(&l, LISTITEM_OF(item_t, &a));
    list_push(&l, LISTITEM_OF(item_t, &b));
    list_push(&l, LISTITEM_OF(item_t, &c));
    list_push(&l, LISTITEM_OF(item_t, &d));

    void** array_heap = malloc(sizeof(void *) * 4);
    list_to_array(&l, array_heap);

    TEST_ASSERT(array_heap[0] == &a);
    TEST_ASSERT(array_heap[1] == &b);
    TEST_ASSERT(array_heap[2] == &c);
    TEST_ASSERT(array_heap[3] == &d);

    free(array_heap);

    void* array_stack[4];
    list_to_array(&l, array_stack);

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

    TEST_ASSERT(list_pop(&l) == &a);
    TEST_ASSERT(list_pop(&l) == &b);
    TEST_ASSERT(list_pop(&l) == &c);
    TEST_ASSERT(list_pop(&l) == &d);

}

void test_list_shift() {
    item_t ZERO(a), ZERO(b), ZERO(c), ZERO(d);
    list_t l;
    list_init(&l);

    list_push(&l, LISTITEM_OF(item_t, &a));
    list_push(&l, LISTITEM_OF(item_t, &b));
    list_push(&l, LISTITEM_OF(item_t, &c));
    list_push(&l, LISTITEM_OF(item_t, &d));

    TEST_ASSERT(list_shift(&l) == &a);
    TEST_ASSERT(list_shift(&l) == &b);
    TEST_ASSERT(list_shift(&l) == &c);
    TEST_ASSERT(list_shift(&l) == &d);

}

void test_list_get() {
    item_t ZERO(a), ZERO(b), ZERO(c), ZERO(d);
    list_t l;
    list_init(&l);

    TEST_ASSERT(list_get(&l, -1) == NULL);
    TEST_ASSERT(list_get(&l, 0) == NULL);
    TEST_ASSERT(list_get(&l, -1) == NULL);

    list_push(&l, LISTITEM_OF(item_t, &a));
    TEST_ASSERT(list_get(&l, 0) == &a);
    TEST_ASSERT(list_get_item(&l, 0) == LISTITEM_PTR(item_t, &a));

    list_push(&l, LISTITEM_OF(item_t, &b));
    TEST_ASSERT(list_get(&l, 1) == &b);
    TEST_ASSERT(list_get_item(&l, 1) == LISTITEM_PTR(item_t, &b));

    list_push(&l, LISTITEM_OF(item_t, &c));
    TEST_ASSERT(list_get(&l, 2) == &c);
    TEST_ASSERT(list_get_item(&l, 2) == LISTITEM_PTR(item_t, &c));

    list_push(&l, LISTITEM_OF(item_t, &d));
    TEST_ASSERT(list_get(&l, 3) == &d);
    TEST_ASSERT(list_get_item(&l, 3) == LISTITEM_PTR(item_t, &d));

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

    TEST_ASSERT(list_remove_index(&l, 0) == &a);	// remove start
    TEST_ASSERT(list_remove_index(&l, 0) == &b);	// remove new start
    TEST_ASSERT(list_remove_index(&l, 1) == &d);	// remove middle
    TEST_ASSERT(list_remove_index(&l, 1) == &f);	// remove end
    TEST_ASSERT(list_remove_index(&l, 0) == &c);	// remove last
    TEST_ASSERT(list_len(&l) == 0);

}

void test_list_contains() {
    item_t ZERO(a), ZERO(b), ZERO(c), ZERO(d);
    list_t l;
    list_init(&l);

    list_push(&l, LISTITEM_OF(item_t, &a));
    list_push(&l, LISTITEM_OF(item_t, &c));

    TEST_ASSERT(list_contains(&l, LISTITEM_OF(item_t, &a)));
    TEST_ASSERT(list_contains_item(&l, LISTITEM_PTR(item_t, &a)));
    TEST_ASSERT(list_contains_data(&l, &a));

    TEST_ASSERT(list_contains(&l, LISTITEM_OF(item_t, &c)));
    TEST_ASSERT(list_contains_item(&l, LISTITEM_PTR(item_t, &c)));
    TEST_ASSERT(list_contains_data(&l, &c));

    TEST_ASSERT(!list_contains(&l, LISTITEM_OF(item_t, &b)));
    TEST_ASSERT(!list_contains_item(&l, LISTITEM_PTR(item_t, &b)));
    TEST_ASSERT(!list_contains_data(&l, &b));

    TEST_ASSERT(!list_contains(&l, LISTITEM_OF(item_t, &d)));
    TEST_ASSERT(!list_contains_item(&l, LISTITEM_PTR(item_t, &d)));
    TEST_ASSERT(!list_contains_data(&l, &d));

    list_remove_index(&l, 0);
    list_push(&l, LISTITEM_OF(item_t, &d));

    TEST_ASSERT(!list_contains(&l, LISTITEM_OF(item_t, &a)));
    TEST_ASSERT(!list_contains_item(&l, LISTITEM_PTR(item_t, &a)));
    TEST_ASSERT(!list_contains_data(&l, &a));

    TEST_ASSERT(list_contains(&l, LISTITEM_OF(item_t, &c)));
    TEST_ASSERT(list_contains_item(&l, LISTITEM_PTR(item_t, &c)));
    TEST_ASSERT(list_contains_data(&l, &c));

    TEST_ASSERT(!list_contains(&l, LISTITEM_OF(item_t, &b)));
    TEST_ASSERT(!list_contains_item(&l, LISTITEM_PTR(item_t, &b)));
    TEST_ASSERT(!list_contains_data(&l, &b));

    TEST_ASSERT(list_contains(&l, LISTITEM_OF(item_t, &d)));
    TEST_ASSERT(list_contains_item(&l, LISTITEM_PTR(item_t, &d)));
    TEST_ASSERT(list_contains_data(&l, &d));

}

void test_listitem_remove() {
    item_t ZERO(a), ZERO(b), ZERO(c), ZERO(d);
    list_t l;
    list_init(&l);

    list_push(&l, LISTITEM_OF(item_t, &a));
    list_push(&l, LISTITEM_OF(item_t, &b));
    list_push(&l, LISTITEM_OF(item_t, &c));
    list_push(&l, LISTITEM_OF(item_t, &d));

    listitem_remove(LISTITEM_OF(item_t, &b));
    TEST_ASSERT(!list_contains(&l, LISTITEM_OF(item_t, &b)));

    listitem_remove_item(LISTITEM_PTR(item_t, &c));
    TEST_ASSERT(!list_contains(&l, LISTITEM_OF(item_t, &c)));

    TEST_ASSERT(list_len(&l) == 2);

}

