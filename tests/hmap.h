#include "acutest.h"

#include "src/hmap.h"

#define HMAP_TESTS \
    { "hmap init", test_hmap_init }, \
    { "hmap managed init", test_hmap_managed_init }, \
    { "hmap set", test_hmap_set }, \
    { "hmap set overwrite", test_hmap_set_overwrite }, \
    { "hmap has", test_hmap_has }, \
    { "hmap get", test_hmap_get }, \
    { "hmap get bad hash", test_hmap_get_bad_hash }, \
    { "hmapimtem in map ", test_hmapitem_in_map }, \
    { "hmap hash collisions", test_hmap_hash_collisions }, \
    { "hmap hash collisions out of order", test_hmap_hash_collisions_out_of_order }, \
    { "hmap hash collisions wrap", test_hmap_hash_collisions_wrap }, \
    { "hmap delete clear item", test_hmap_delete_clear_item }, \
    { "hmap delete no collision", test_hmap_delete_no_collision }, \
    { "hmap delete 1 same hash collision", test_hmap_delete_1_same_hash_collision }, \
    { "hmap delete full same hash collision", test_hmap_delete_full_same_hash_collision }, \
    { "hmap delete out of order hash collision", test_hmap_delete_out_of_order_hash_collision }, \
    { "hmap delete from collisions with different collisions inside", test_hmap_delete_from_collisions_with_different_collisions_inside }, \
    { "hmap managed capacity", test_hmap_managed_capacity }, \
    { "hmap managed min capacity", test_hmap_managed_min_capacity }, \
    { "hmap rehash", test_hmap_rehash }, \
    { "hmap rehash to", test_hmap_rehash_to }, \
    { "hmap foreach", test_hmap_foreach }, \
    { "hmap iter", test_hmap_iter }

#define ZERO(x) x={0}

void hmap_debug_str_keys(hmap_t* m) {
    printf("\nDebug keys\n");
    for(size_t i = 0; i < m->capacity; i++) {
        if(m->data[i] != NULL){
            printf("%ld: %s, %ld, %ld\n", i, (char*)m->data[i]->key, m->hash(m->data[i]->key), m->hash(m->data[i]->key) % m->capacity);
        } else {
            printf("%ld: NULL\n", i);
        }
    }
}

size_t hmap_hash_bad_same(void* _ptr) {
    ((void)_ptr);
    return 3;
}

size_t hmap_hash_first_char_value(void* ptr) {
    char* c = (char*)ptr;
    return *c - 'a';
}

bool hmap_equals_str(void* a, void* b) {
    if ((a == NULL) ^ (b == NULL)){
        return false;
    }
    return strcmp(a, b) == 0;
}

typedef struct {
    char* data;
    hmapitem_t item;
} explicit_hmapitem_t;

typedef struct {
    char* data;
    HMAPITEM_PROP();
} implicit_hmapitem_t;


void test_hmap_init(){
    hmap_t ZERO(m);
    hmap_init_unmanaged(&m, hmap_hash_first_char_value, hmap_equals_str, 16);

    TEST_ASSERT(hmap_capacity(&m) == 16);
    TEST_ASSERT(hmap_length(&m) == 0);
    TEST_ASSERT(hmap_stats_load_factor(&m) == 0.0);
    TEST_ASSERT(hmap_stats_last_set_collisions(&m) == 0);

    hmap_destroy(&m);
}

void test_hmap_managed_init(){
    hmap_t ZERO(m);
    hmap_init(&m, hmap_hash_first_char_value, hmap_equals_str);

    TEST_ASSERT(hmap_capacity(&m) == HMAP_INITIAL_CAPACITY);
    TEST_ASSERT(hmap_length(&m) == 0);
    TEST_ASSERT(hmap_stats_load_factor(&m) == 0.0);
    TEST_ASSERT(hmap_stats_last_set_collisions(&m) == 0);

    hmap_destroy(&m);
}

void test_hmap_set() {
    explicit_hmapitem_t ZERO(a), ZERO(b), ZERO(c);
    char* key_a = "kljaeksdan";
    char* key_b = "klfdbgklsdfb";
    char* key_c = "pniowe";
    hmap_t ZERO(m);
    hmap_init_unmanaged(&m, hmap_hash_first_char_value, hmap_equals_str, 16);

    TEST_ASSERT(hmap_length(&m) == 0);

    hmap_set(&m, key_a, &a.item);
    TEST_ASSERT(hmap_length(&m) == 1);
    TEST_ASSERT(a.item.key == key_a);
    TEST_ASSERT(a.item.map_ptr == &m);

    hmap_set(&m, key_b, &b.item);
    TEST_ASSERT(hmap_length(&m) == 2);
    TEST_ASSERT(b.item.key == key_b);
    TEST_ASSERT(b.item.map_ptr == &m);

    hmap_set(&m, key_c, &c.item);
    TEST_ASSERT(hmap_length(&m) == 3);
    TEST_ASSERT(c.item.key == key_c);
    TEST_ASSERT(c.item.map_ptr == &m);

    hmap_destroy(&m);
}

void test_hmap_delete_clear_item() {
    implicit_hmapitem_t ZERO(a);
    hmap_t ZERO(m);
    hmap_init_unmanaged(&m, hmap_hash_first_char_value, hmap_equals_str, 16);

    TEST_ASSERT(hmap_length(&m) == 0);

    char* key = "a";

    hmap_set(&m, key, HMAPITEM_OF(implicit_hmapitem_t, &a));
    TEST_ASSERT(hmap_length(&m) == 1);
    TEST_ASSERT(a.default_hmap_item_name.key == key);
    TEST_ASSERT(a.default_hmap_item_name.map_ptr == &m);


    implicit_hmapitem_t* i = HMAPITEM_AS(implicit_hmapitem_t, hmap_delete(&m, key));
    TEST_ASSERT(hmap_length(&m) == 0);
    TEST_ASSERT(i->default_hmap_item_name.map_ptr == NULL);
    TEST_ASSERT(i->default_hmap_item_name.key == NULL);


    hmap_destroy(&m);
}

void test_hmap_set_overwrite() {
    explicit_hmapitem_t ZERO(a), ZERO(b);
    char* key_a = "kljaeksdan";
    hmap_t ZERO(m);
    hmap_init_unmanaged(&m, hmap_hash_first_char_value, hmap_equals_str, 16);

    TEST_ASSERT(hmap_length(&m) == 0);

    hmap_set(&m, key_a, &a.item);
    TEST_ASSERT(hmap_length(&m) == 1);
    TEST_ASSERT(a.item.key == key_a);
    TEST_ASSERT(a.item.map_ptr == &m);
    TEST_ASSERT(m.data[hmap_hash_first_char_value(key_a)%16] == &a.item);

    hmap_set(&m, key_a, &b.item);
    TEST_ASSERT(hmap_length(&m) == 1);
    TEST_ASSERT(b.item.key == key_a);
    TEST_ASSERT(b.item.map_ptr == &m);
    TEST_ASSERT(m.data[hmap_hash_first_char_value(key_a)%16] == &b.item);


    TEST_ASSERT(a.item.key == NULL);
    TEST_ASSERT(a.item.map_ptr == NULL);

    hmap_destroy(&m);
}

void test_hmap_has() {
    explicit_hmapitem_t ZERO(a), ZERO(b), ZERO(c);
    char* key_a = "kljaeksdan";
    char* key_b = "klfdbgklsdfb";
    char* key_c = "pniowe";
    hmap_t ZERO(m);
    hmap_init_unmanaged(&m, hmap_hash_first_char_value, hmap_equals_str, 16);

    TEST_ASSERT(hmap_length(&m) == 0);

    TEST_ASSERT(!hmap_has(&m, key_a));
    hmap_set(&m, key_a, &a.item);
    TEST_ASSERT(hmap_has(&m, key_a));

    TEST_ASSERT(!hmap_has(&m, key_b));
    hmap_set(&m, key_b, &b.item);
    TEST_ASSERT(hmap_has(&m, key_b));

    TEST_ASSERT(!hmap_has(&m, key_c));
    hmap_set(&m, key_c, &c.item);
    TEST_ASSERT(hmap_has(&m, key_c));

    hmap_destroy(&m);
}

void test_hmapitem_in_map() {

    implicit_hmapitem_t ZERO(a);
    char* key_a = "a";
    hmap_t ZERO(m);
    hmap_t ZERO(n);
    hmap_init_unmanaged(&m, hmap_hash_first_char_value, hmap_equals_str, 16);
    hmap_init_unmanaged(&n, hmap_hash_first_char_value, hmap_equals_str, 16);

    TEST_ASSERT(!hmapitem_in_map(HMAPITEM_OF(implicit_hmapitem_t, &a), &m));
    TEST_ASSERT(!hmapitem_in_map(HMAPITEM_OF(implicit_hmapitem_t, &a), &n));

    HMAP_SET(implicit_hmapitem_t, &m, &key_a, &a);

    TEST_ASSERT(hmapitem_in_map(HMAPITEM_OF(implicit_hmapitem_t, &a), &m));
    TEST_ASSERT(!hmapitem_in_map(HMAPITEM_OF(implicit_hmapitem_t, &a), &n));

    hmap_delete(&m, &key_a);

    TEST_ASSERT(!hmapitem_in_map(HMAPITEM_OF(implicit_hmapitem_t, &a), &m));
    TEST_ASSERT(!hmapitem_in_map(HMAPITEM_OF(implicit_hmapitem_t, &a), &n));

    hmap_destroy(&m);
    hmap_destroy(&n);
}

void test_hmap_get() {
    implicit_hmapitem_t ZERO(a), ZERO(b), ZERO(c);
    char* key_a = "kljaeksdan";
    char* key_b = "klfdbgklsdfb";
    char* key_c = "pniowe";
    hmap_t ZERO(m);
    hmap_init_unmanaged(&m, hmap_hash_first_char_value, hmap_equals_str, 16);

    hmap_set(&m, key_a, HMAPITEM_OF(implicit_hmapitem_t, &a));
    hmap_set(&m, key_b, HMAPITEM_OF(implicit_hmapitem_t, &b));
    hmap_set(&m, key_c, HMAPITEM_OF(implicit_hmapitem_t, &c));

    implicit_hmapitem_t* x = HMAP_GET(implicit_hmapitem_t, &m, key_a);
    TEST_ASSERT(x == &a);

    x = HMAP_GET(implicit_hmapitem_t, &m, key_b);
    TEST_ASSERT(x == &b);

    x = HMAP_GET(implicit_hmapitem_t, &m, key_c);
    TEST_ASSERT(x == &c);
    hmap_destroy(&m);
}

void test_hmap_get_bad_hash() {
    implicit_hmapitem_t ZERO(a), ZERO(b), ZERO(c);
    char* key_a = "kljaeksdan";
    char* key_b = "klfdbgklsdfb";
    char* key_c = "pniowe";
    hmap_t ZERO(m);
    hmap_init_unmanaged(&m, hmap_hash_bad_same, hmap_equals_str, 16);

    hmap_set(&m, key_a, HMAPITEM_OF(implicit_hmapitem_t, &a));
    hmap_set(&m, key_b, HMAPITEM_OF(implicit_hmapitem_t, &b));
    hmap_set(&m, key_c, HMAPITEM_OF(implicit_hmapitem_t, &c));

    implicit_hmapitem_t* x = HMAP_GET(implicit_hmapitem_t, &m, key_a);
    TEST_ASSERT(x == &a);

    x = HMAP_GET(implicit_hmapitem_t, &m, key_b);
    TEST_ASSERT(x == &b);

    x = HMAP_GET(implicit_hmapitem_t, &m, key_c);
    TEST_ASSERT(x == &c);
    hmap_destroy(&m);
}

void test_hmap_hash_collisions(){
    implicit_hmapitem_t ZERO(a), ZERO(b), ZERO(c), ZERO(d);
    hmap_t ZERO(m);
    hmap_init_unmanaged(&m, hmap_hash_first_char_value, hmap_equals_str, 16);

    hmap_set(&m, "a_1", HMAPITEM_OF(implicit_hmapitem_t, &a));
    TEST_ASSERT(m.data[0] == &a.default_hmap_item_name);

    hmap_set(&m, "a_2", HMAPITEM_OF(implicit_hmapitem_t, &b));
    TEST_ASSERT(m.data[1] == &b.default_hmap_item_name);

    hmap_set(&m, "c_3", HMAPITEM_OF(implicit_hmapitem_t, &c));
    TEST_ASSERT(m.data[2] == &c.default_hmap_item_name);

    TEST_ASSERT(m.data[3] == NULL);

    hmap_set(&m, "e_3", HMAPITEM_OF(implicit_hmapitem_t, &d));
    TEST_ASSERT(m.data[4] == &d.default_hmap_item_name);

    // layout is now [a, b, c, 0, d]
    hmap_destroy(&m);

}

void test_hmap_hash_collisions_out_of_order(){
    implicit_hmapitem_t ZERO(a), ZERO(b), ZERO(c);
    hmap_t ZERO(m);
    hmap_init_unmanaged(&m, hmap_hash_first_char_value, hmap_equals_str, 16);

    hmap_set(&m, "a_1", HMAPITEM_OF(implicit_hmapitem_t, &a));
    TEST_ASSERT(m.data[0] == &a.default_hmap_item_name);

    hmap_set(&m, "b_2", HMAPITEM_OF(implicit_hmapitem_t, &b));
    TEST_ASSERT(m.data[1] == &b.default_hmap_item_name);

    hmap_set(&m, "a_3", HMAPITEM_OF(implicit_hmapitem_t, &c));
    TEST_ASSERT(m.data[2] == &c.default_hmap_item_name);

    // layout is now [a, b, c]
    hmap_destroy(&m);

}

void test_hmap_hash_collisions_wrap(){
    implicit_hmapitem_t ZERO(a), ZERO(b), ZERO(c), ZERO(d);
    hmap_t ZERO(m);
    hmap_init_unmanaged(&m, hmap_hash_first_char_value, hmap_equals_str, 4);

    hmap_set(&m, "c_1", HMAPITEM_OF(implicit_hmapitem_t, &a));
    TEST_ASSERT(m.data[2] == &a.default_hmap_item_name);

    hmap_set(&m, "c_2", HMAPITEM_OF(implicit_hmapitem_t, &b));
    TEST_ASSERT(m.data[3] == &b.default_hmap_item_name);

    hmap_set(&m, "c_3", HMAPITEM_OF(implicit_hmapitem_t, &c));
    TEST_ASSERT(m.data[0] == &c.default_hmap_item_name);

    hmap_set(&m, "c_4", HMAPITEM_OF(implicit_hmapitem_t, &d));
    TEST_ASSERT(m.data[1] == &d.default_hmap_item_name);

    // layout is now [c, d, a, b]
    hmap_destroy(&m);

}

// delete without collision
void test_hmap_delete_no_collision() {
    implicit_hmapitem_t ZERO(a);
    hmap_t ZERO(m);
    hmap_init_unmanaged(&m, hmap_hash_first_char_value, hmap_equals_str, 100);

    HMAP_SET(implicit_hmapitem_t, &m, "key", &a);
    TEST_ASSERT(&a == HMAP_GET(implicit_hmapitem_t, &m, "key"));
    hmap_delete(&m, "key");
    TEST_ASSERT(NULL == HMAP_GET(implicit_hmapitem_t, &m, "key"));
    TEST_ASSERT(a.default_hmap_item_name.map_ptr == NULL);
    TEST_ASSERT(a.default_hmap_item_name.key == NULL);

    hmap_destroy(&m);
}

// delete with 1 same hash collision
void test_hmap_delete_1_same_hash_collision() {
    implicit_hmapitem_t ZERO(a), ZERO(b);
    hmap_t ZERO(m);
    hmap_init_unmanaged(&m, hmap_hash_first_char_value, hmap_equals_str, 100);

    HMAP_SET(implicit_hmapitem_t, &m, "p_1", &a);
    HMAP_SET(implicit_hmapitem_t, &m, "p_2", &b);
    TEST_ASSERT(&a == HMAP_GET(implicit_hmapitem_t, &m, "p_1"));
    TEST_ASSERT(&b == HMAP_GET(implicit_hmapitem_t, &m, "p_2"));

    hmap_delete(&m, "p_1");

    TEST_ASSERT(&b == HMAP_GET(implicit_hmapitem_t, &m, "p_2"));

    hmap_destroy(&m);
}

// delete full with same hash collision
void test_hmap_delete_full_same_hash_collision() {
    implicit_hmapitem_t ZERO(a), ZERO(b), ZERO(c), ZERO(d);
    hmap_t ZERO(m);
    hmap_init_unmanaged(&m, hmap_hash_first_char_value, hmap_equals_str, 4);

    HMAP_SET(implicit_hmapitem_t, &m, "b_1", &a);
    HMAP_SET(implicit_hmapitem_t, &m, "b_2", &b);
    HMAP_SET(implicit_hmapitem_t, &m, "b_3", &c);
    HMAP_SET(implicit_hmapitem_t, &m, "b_4", &d);

    TEST_ASSERT(&a == HMAP_GET(implicit_hmapitem_t, &m, "b_1"));
    TEST_ASSERT(&b == HMAP_GET(implicit_hmapitem_t, &m, "b_2"));
    TEST_ASSERT(&c == HMAP_GET(implicit_hmapitem_t, &m, "b_3"));
    TEST_ASSERT(&d == HMAP_GET(implicit_hmapitem_t, &m, "b_4"));


    TEST_ASSERT(m.data[0] == &d.default_hmap_item_name);
    TEST_ASSERT(m.data[1] == &a.default_hmap_item_name);
    TEST_ASSERT(m.data[2] == &b.default_hmap_item_name);
    TEST_ASSERT(m.data[3] == &c.default_hmap_item_name);

    hmap_delete(&m, "b_1");
    TEST_ASSERT(m.data[0] == NULL);
    TEST_ASSERT(m.data[1] == &b.default_hmap_item_name);
    TEST_ASSERT(m.data[2] == &c.default_hmap_item_name);
    TEST_ASSERT(m.data[3] == &d.default_hmap_item_name);

    hmap_delete(&m, "b_3");
    TEST_ASSERT(m.data[0] == NULL);
    TEST_ASSERT(m.data[1] == &b.default_hmap_item_name);
    TEST_ASSERT(m.data[2] == &d.default_hmap_item_name);
    TEST_ASSERT(m.data[3] == NULL);

    hmap_delete(&m, "b_4");
    TEST_ASSERT(m.data[0] == NULL);
    TEST_ASSERT(m.data[1] == &b.default_hmap_item_name);
    TEST_ASSERT(m.data[2] == NULL);
    TEST_ASSERT(m.data[3] == NULL);

    hmap_delete(&m, "b_2");
    TEST_ASSERT(m.data[0] == NULL);
    TEST_ASSERT(m.data[1] == NULL);
    TEST_ASSERT(m.data[2] == NULL);
    TEST_ASSERT(m.data[3] == NULL);

    TEST_ASSERT(hmap_length(&m) == 0);

    hmap_destroy(&m);
}

// delete out of order hash collision
void test_hmap_delete_out_of_order_hash_collision() {
    implicit_hmapitem_t ZERO(a), ZERO(b), ZERO(c), ZERO(d);
    hmap_t ZERO(m);
    hmap_init_unmanaged(&m, hmap_hash_first_char_value, hmap_equals_str, 4);

    HMAP_SET(implicit_hmapitem_t, &m, "d_1", &a);
    HMAP_SET(implicit_hmapitem_t, &m, "a_1", &b);
    HMAP_SET(implicit_hmapitem_t, &m, "d_2", &c);
    HMAP_SET(implicit_hmapitem_t, &m, "d_3", &d);

    TEST_ASSERT(&a == HMAP_GET(implicit_hmapitem_t, &m, "d_1"));
    TEST_ASSERT(&b == HMAP_GET(implicit_hmapitem_t, &m, "a_1"));
    TEST_ASSERT(&c == HMAP_GET(implicit_hmapitem_t, &m, "d_2"));
    TEST_ASSERT(&d == HMAP_GET(implicit_hmapitem_t, &m, "d_3"));


    TEST_ASSERT(m.data[0] == &b.default_hmap_item_name);
    TEST_ASSERT(m.data[1] == &c.default_hmap_item_name);
    TEST_ASSERT(m.data[2] == &d.default_hmap_item_name);
    TEST_ASSERT(m.data[3] == &a.default_hmap_item_name);

    hmap_delete(&m, "d_3");
    TEST_ASSERT(m.data[0] == &b.default_hmap_item_name);
    TEST_ASSERT(m.data[1] == &c.default_hmap_item_name);
    TEST_ASSERT(m.data[2] == NULL);
    TEST_ASSERT(m.data[3] == &a.default_hmap_item_name);

    hmap_delete(&m, "d_1");
    TEST_ASSERT(m.data[0] == &b.default_hmap_item_name);
    TEST_ASSERT(m.data[1] == NULL);
    TEST_ASSERT(m.data[2] == NULL);
    TEST_ASSERT(m.data[3] == &c.default_hmap_item_name);

    hmap_delete(&m, "a_1");
    TEST_ASSERT(m.data[0] == NULL);
    TEST_ASSERT(m.data[1] == NULL);
    TEST_ASSERT(m.data[2] == NULL);
    TEST_ASSERT(m.data[3] == &c.default_hmap_item_name);

    hmap_delete(&m, "d_2");
    TEST_ASSERT(m.data[0] == NULL);
    TEST_ASSERT(m.data[1] == NULL);
    TEST_ASSERT(m.data[2] == NULL);
    TEST_ASSERT(m.data[3] == NULL);

    hmap_destroy(&m);
}

void test_hmap_delete_from_collisions_with_different_collisions_inside() {
    implicit_hmapitem_t ZERO(a), ZERO(b), ZERO(c), ZERO(d);
    hmap_t ZERO(m);
    hmap_init_unmanaged(&m, hmap_hash_first_char_value, hmap_equals_str, 4);

    HMAP_SET(implicit_hmapitem_t, &m, "a_1", &a);
    HMAP_SET(implicit_hmapitem_t, &m, "b_1", &b);
    HMAP_SET(implicit_hmapitem_t, &m, "b_2", &c);
    HMAP_SET(implicit_hmapitem_t, &m, "a_2", &d);

    TEST_ASSERT(&a == HMAP_GET(implicit_hmapitem_t, &m, "a_1"));
    TEST_ASSERT(&b == HMAP_GET(implicit_hmapitem_t, &m, "b_1"));
    TEST_ASSERT(&c == HMAP_GET(implicit_hmapitem_t, &m, "b_2"));
    TEST_ASSERT(&d == HMAP_GET(implicit_hmapitem_t, &m, "a_2"));

    hmap_delete(&m, "a_1");

    TEST_ASSERT(m.data[0] == &d.default_hmap_item_name);
    TEST_ASSERT(m.data[1] == &b.default_hmap_item_name);
    TEST_ASSERT(m.data[2] == &c.default_hmap_item_name);
    TEST_ASSERT(m.data[3] == NULL);

    hmap_destroy(&m);
}

struct named_thing {
    char name;
    HMAPITEM_PROP();
};

void test_hmap_managed_capacity() {
    struct named_thing items[64];
    memset(&items, 0, sizeof(struct named_thing)*64);
    for (int i = 0; i < 64; i++) {
        items[i].name = 'A' + i;
    }

    hmap_t ZERO(m);
    hmap_init_unmanaged(&m, hmap_hash_first_char_value, hmap_equals_str, 4);
    hmap_managed(&m, .2, .6, 2);

    int i = 0;
    // grow once
    for(; i < 3; i++) {
        hmap_set(&m, &items[i].name, HMAPITEM_OF(struct named_thing, &items[i]));
    }

    TEST_ASSERT(hmap_length(&m) == 3);
    TEST_ASSERT(hmap_capacity(&m) == 8);

    // shrink twice but max to > 0
    for(; i >= 0; i--) {
        hmap_delete(&m, &items[i].name);
    }

    TEST_ASSERT(hmap_length(&m) == 0);
    TEST_ASSERT(hmap_capacity(&m) > 0);

    // grow close to the max load factor limit
    for(i=0; i < 38; i++) {
        hmap_set(&m, &items[i].name, HMAPITEM_OF(struct named_thing, &items[i]));
    }

    TEST_ASSERT(hmap_length(&m) == 38);
    TEST_ASSERT(hmap_capacity(&m) == 64);

    // push load factor over the limit
    hmap_set(&m, &items[i].name, HMAPITEM_OF(struct named_thing, &items[i]));

    // expect capacity to grow
    TEST_ASSERT(hmap_length(&m) == 39);
    TEST_ASSERT(hmap_capacity(&m) == 128);

    // push load factor down below the max load factor but above the min load factor
    hmap_delete(&m, &items[i].name);

    // expect only the length to shrink
    TEST_ASSERT(hmap_length(&m) == 38);
    TEST_ASSERT(hmap_capacity(&m) == 128);

    // push more items
    for(; i < 64; i++) {
        hmap_set(&m, &items[i].name, HMAPITEM_OF(struct named_thing, &items[i]));
    }

    // expect no change
    TEST_ASSERT(hmap_length(&m) == 64);
    TEST_ASSERT(hmap_capacity(&m) == 128);

    hmap_destroy(&m);

}

void test_hmap_managed_min_capacity() {
    struct named_thing items[64];
    memset(&items, 0, sizeof(struct named_thing)*64);
    for (int i = 0; i < 64; i++) {
        items[i].name = 'A' + i;
    }

    hmap_t ZERO(m);
    hmap_init(&m, hmap_hash_first_char_value, hmap_equals_str);
    size_t initial_cap = hmap_capacity(&m);

    TEST_ASSERT(hmap_length(&m) == 0);
    TEST_ASSERT(hmap_capacity(&m) == initial_cap);

    int i = 0;
    for(; i < 64; i++) {
        hmap_set(&m, &items[i].name, HMAPITEM_OF(struct named_thing, &items[i]));
    }

    TEST_ASSERT(hmap_length(&m) == 64);
    TEST_ASSERT(hmap_capacity(&m) == 128);

    // shrink twice but max to > 0
    for(; i >= 0; i--) {
        hmap_delete(&m, &items[i].name);
    }

    TEST_ASSERT(hmap_length(&m) == 0);
    TEST_ASSERT(hmap_capacity(&m) == initial_cap);

    hmap_destroy(&m);

}

void test_hmap_rehash() {

    struct named_thing items[32];
    memset(&items, 0, sizeof(struct named_thing)*32);
    for (int i = 0; i < 32; i++) {
        items[i].name = 'A' + i; // generate uniqe keys
    }

    hmap_t ZERO(m);
    hmap_init_unmanaged(&m, hmap_hash_bad_same, hmap_equals_str, 32); // use very bad hash (only collisions)

    // insert all
    for(int i = 0; i < 32; i++) {
        hmap_set(&m, &items[i].name, HMAPITEM_OF(struct named_thing, &items[i]));
    }

    TEST_ASSERT(hmap_stats_last_set_collisions(&m) == 31);

    hmap_delete(&m, &items[31].name);

    hmap_rehash(&m, hmap_hash_first_char_value, hmap_equals_str); // rehash with a function that generates uniqe hashes for all our keys

    hmap_set(&m, &items[31].name, HMAPITEM_OF(struct named_thing, &items[31]));

    // test for zero collisions
    TEST_ASSERT(hmap_stats_last_set_collisions(&m) == 0);

    hmap_destroy(&m);
}

void test_hmap_rehash_to() {

    struct named_thing items[32];
    memset(&items, 0, sizeof(struct named_thing)*32);
    for (int i = 0; i < 32; i++) {
        items[i].name = 'A' + i; // generate uniqe keys
    }

    hmap_t ZERO(m), ZERO(n);
    hmap_init_unmanaged(&m, hmap_hash_first_char_value, hmap_equals_str, 16);
    hmap_init_unmanaged(&n, hmap_hash_first_char_value, hmap_equals_str, 32);

    int i= 0;
    for(i = 0; i < 16; i++) {
        hmap_set(&m, &items[i].name, HMAPITEM_OF(struct named_thing, &items[i]));
    }

    for(; i < 32; i++) {
        hmap_set(&n, &items[i].name, HMAPITEM_OF(struct named_thing, &items[i]));
    }

    hmap_rehash_to(&m, &n);

    for (i = 0; i < 32; i++) {
        TEST_ASSERT(HMAP_GET(struct named_thing, &n, &items[i].name) != NULL);
    }

    hmap_destroy(&m);
    hmap_destroy(&n);

}

struct hmap_person {
    char name;
    unsigned int age;
    HMAPITEM_PROP();
};

void hmap_iter_sum(void* key, hmapitem_t* item, void* userdata) {
    char* k = (char*) key;
    struct hmap_person* p = HMAPITEM_AS(struct hmap_person, item);
    int* sum = (int*) userdata;

    *sum += k[0] == p->name ? p->age : 0;
}

void test_hmap_foreach() {
    struct hmap_person items[32];
    memset(&items, 0, sizeof(struct hmap_person)*32);
    for (int i = 0; i < 32; i++) {
        items[i].name = 'A' + i;
        items[i].age = i;
    }

    hmap_t ZERO(m);
    hmap_init_unmanaged(&m, hmap_hash_first_char_value, hmap_equals_str, 32);

    for(int i = 0; i < 32; i++) {
        hmap_set(&m, &items[i].name, HMAPITEM_OF(struct named_thing, &items[i]));
    }

    int sum  = 0;
    hmap_foreach(&m, hmap_iter_sum, &sum);

    TEST_ASSERT(sum == (32*(32-1))/2);

    hmap_destroy(&m);
}

void test_hmap_iter() {

    struct hmap_person items[32];
    memset(&items, 0, sizeof(struct hmap_person)*32);
    for (int i = 0; i < 32; i++) {
        items[i].name = 'A' + i;
        items[i].age = i;
    }

    hmap_t ZERO(m);
    hmap_init_unmanaged(&m, hmap_hash_first_char_value, hmap_equals_str, 32);

    for(int i = 0; i < 32; i++) {
        hmap_set(&m, &items[i].name, HMAPITEM_OF(struct named_thing, &items[i]));
    }

    int sum  = 0;

    HMAP_ITER(iter_item_ptr, &m) {
        char* key = HMAP_ITER_KEY_AS(char, iter_item_ptr);

        struct hmap_person* p = HMAP_ITER_VALUE_AS(struct hmap_person, iter_item_ptr);

        sum += key[0] == p->name ? p->age : 0;
    }

    TEST_ASSERT(sum == (32*(32-1))/2);

    hmap_destroy(&m);
}

