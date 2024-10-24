
/*

# Hash Map

## Usage

### Include

To generate the implementations include the header with setting `IMPL_HMAP` before. Do this only once e.g. in main.c

```
#define IMPL_HMAP
#include "hmap.h"
```

After that include hmap.h like a normal header everywhere the declarations are needed
```
#include "hmap.h"
```
### Basic Usage

```
typedef struct {
    char name[32];
    double complex sex;
    HAMPITEM_PROP()
} person;

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

void main() {
    hmap_t people;
    hmap_init(&people, hmap_hash_first_char_value, hmap_equals_str);

    person alex = {0}, riley = {0};

    alex.name = "Alex";
    alex.sex = 1.0 + 3.0 * I;

    riley.name = "Riley";
    riley.sex = 4.0 + 2.0 * I;

    hmap_set(&people, &alex.name, HMAPITEM_OF(person, &alex));
    hmap_set(&people, &riley.name, HMAPITEM_OF(person, &riley));

    person* p = HMAP_GET(person, &people, &alex.name);
}
```

## License APGL

Copyright (C) 2024 Mario Aichinger <aichingm@gmail.com>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
License as published by the Free Software Foundation, version 3.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more
details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see
<https://www.gnu.org/licenses/>.

*/

#ifndef DS_MAP_H
#define DS_MAP_H
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/**
 * The default initial number of buckets per map.
 */
#define HMAP_INITIAL_CAPACITY 32

/**
 * The name of the default property name.
 */
#define HMAP_DEFAULT_PROPERTY_NAME default_hmap_item_name

/**
 * Get the map item associated with the given key and return a pointer to the struct holding the item using the default
 * item property name.
 */
#define HMAP_GET(type, map, key) HMAP_GET_s(type, map, key, HMAP_DEFAULT_PROPERTY_NAME)

/**
 * Get the map item associated with the given key and return a pointer to the struct holding the item.
 */
#define HMAP_GET_s(type, map, key, property_name) \
    (hmap_has(map, key) ? HMAPITEM_AS_s(type, hmap_get(map, key), property_name) : NULL)

/**
 * Add the item to the map under the given name using the default item property name.
 */
#define HMAP_SET(type, map, key, item) HMAP_SET_s(type, map, key, item, HMAP_DEFAULT_PROPERTY_NAME)

/**
 * Add the item to the map under the given name using the default item property name.
 */
#define HMAP_SET_s(type, map, key, item, property_name) hmap_set(map, key, HMAPITEM_OF_s(type, item, property_name))

/**
 * Inject a hmapitem_t property with a given name into a struct.
 */
#define HMAPITEM_PROP() HMAPITEM_PROP_s(HMAP_DEFAULT_PROPERTY_NAME)

/**
 * Inject a hmapitem_t struct with the default property name into a struct.
 */
#define HMAPITEM_PROP_s(property_name) hmapitem_t property_name

/**
 * Convert a type + pointer  name to a hmapitem_t* using the default item property name.
 */
#define HMAPITEM_OF(type, ptr) HMAPITEM_OF_s(type, ptr, HMAP_DEFAULT_PROPERTY_NAME)

/**
 * Convert a type + pointer + property name to a hmapitem_t*.
 */
#define HMAPITEM_OF_s(type, ptr, property_name) (hmapitem_t*)(((char*)ptr) + offsetof(type, property_name))

/**
 * Convert a hmapitem_t to a pointer to its holding struct using the offset of the default property name.
 */
#define HMAPITEM_AS(type, ptr) HMAPITEM_AS_s(type, ptr, HMAP_DEFAULT_PROPERTY_NAME)

/**
 * Convert a hmapitem_t to a pointer to its holding struct using the offset of the given property's name.
 */
#define HMAPITEM_AS_s(type, ptr, property_name) ((type*)(((char*)ptr) - ((char*)offsetof(type, property_name))))

/**
 * internal use only: function parameter type generator for the hash function
 */
#define HMAP_HASH_TYPE(name) size_t (*name)(void* a)

/**
 * internal use only: function parameter type generator for the equals function
 */
#define HMAP_EQUALS_TYPE(name) bool (*name)(void* a, void* b)

typedef struct hmapitem_s {
    void* map_ptr;
    void* key;
} hmapitem_t;

typedef struct hmapstat_s {
    void* map_ptr;
    void* key;
} hmapstat_t;

typedef struct hmap_s {
    bool managed;
    float managed_min_load;
    float managed_max_load;
    size_t managed_min_capacity;
    size_t length;
    size_t capacity;
    size_t last_set_collisions;
    hmapitem_t** data;
    HMAP_HASH_TYPE(hash);
    HMAP_EQUALS_TYPE(equals);
} hmap_t;

/**
 * Iterate over all key value entries in a map.
 */
#define HMAP_ITER(entry, map) for (hmapitem_t** entry = (map)->data; entry < ((map)->data + (map)->capacity); entry++)

/**
 * Convert a key value entry coming from an iterator to a key.
 */
#define HMAP_ITER_KEY_AS(type, ptr) (type*)(*ptr)->key
/**
 * Convert a key value entry coming from an iterator to a pointer to the value of the hmapitem_t holding struct using
 * the default item property name.
 */
#define HMAP_ITER_VALUE_AS(type, ptr) HMAP_ITER_VALUE_AS_s(type, ptr, HMAP_DEFAULT_PROPERTY_NAME)

/**
 * Convert a key value entry coming from an iterator to a pointer to the value of the hmapitem_t holding struct.
 */
#define HMAP_ITER_VALUE_AS_s(type, ptr, property_name) \
    ((type*)(((char*)(*ptr) - ((char*)offsetof(type, property_name)))))

/**
 * Initialize a managed map.
 * Managed means that the map manages its capacity according to its load factor.
 */
void hmap_init(hmap_t* m, HMAP_HASH_TYPE(hash), HMAP_EQUALS_TYPE(equals));

/**
 * Initialize an unmanaged map.
 * Unmanaged map do not automatically adjust their capacity. The user has to check before every hmap_set call if length
 * < capacity. Also performance might degenerate.
 */
void hmap_init_unmanaged(hmap_t* m, HMAP_HASH_TYPE(hash), HMAP_EQUALS_TYPE(equals), size_t capacity);

/**
 * Enable automatic capacity management.
 * See hmap_init.
 */
void hmap_managed(hmap_t* m, float min_load, float max_load, size_t min_capacity);

/**
 * Disable automatic capacity management.
 * See hmap_init_unmanaged.
 */
void hmap_unmanaged(hmap_t* m);

/**
 * Adjust the maps capacity according to the current load factor and the load factor limits.
 */
int hmap_manage(hmap_t* m);

/**
 * Adjust the maps capacity.
 */
void hmap_adjust_capacity(hmap_t* m, size_t capacity);

/**
 * Set the hash and equals function and reposition all items accordingly to the new hash values.
 */
void hmap_rehash(hmap_t* m, HMAP_HASH_TYPE(hash), HMAP_EQUALS_TYPE(equals));

/**
 * Transfer all items to a different map.
 */
void hmap_rehash_to(hmap_t* m, hmap_t* target);

/**
 * Free all resources allocated by a call to hmap_init*() functions.
 */
void hmap_destroy(hmap_t* m);

/**
 * Return the number of items in the map.
 */
size_t hmap_length(hmap_t* m);

/**
 * Return the maximum number of items the map can hold.
 */
size_t hmap_capacity(hmap_t* m);

/**
 * Calculate the load factor (length/capacity = [0, 1]) of the map.
 */
float hmap_stats_load_factor(hmap_t* m);
/**
 * Return the number of hash collision the last call to hmap_set produced.
 */
size_t hmap_stats_last_set_collisions(hmap_t* m);

/**
 * Associate the given key with the item and store it in the map. Possible existing associations will be overwritten.
 */
void hmap_set(hmap_t* m, void* key, hmapitem_t* i);

/**
 * Return true if the given key is associated with a value in the map, false otherwise.
 */
bool hmap_has(hmap_t* m, void* key);

/**
 * Return the item associated with the given key. Null if the key has no association.
 */
hmapitem_t* hmap_get(hmap_t* m, void* key);
/**
 * Remove an association between a key and an item from the map. Return a pointer to the disassociated item if a
 * association existed, null otherwise.
 */
hmapitem_t* hmap_delete(hmap_t* m, void* key);

/**
 * Call iter on every key value entry in the map.
 */
void hmap_foreach(hmap_t* m, void (*iter)(void* key, hmapitem_t*, void*), void* userdata);

/**
 * Initialize a hmapitem_t.
 */
void hmapitem_init(hmapitem_t* i);

/**
 * Return true if the item has an association in the given map.
 */
bool hmapitem_in_map(hmapitem_t* i, hmap_t* m);

#if defined(IMPL_HMAP) || defined(_CLANGD)
#include <assert.h>
#include <stdio.h>

void hmap_init_unmanaged(hmap_t* m, HMAP_HASH_TYPE(hash), HMAP_EQUALS_TYPE(equals), size_t initial_capacity) {
    assert(m != NULL);

    memset(m, 0, sizeof(hmap_t));
    m->managed = false;
    m->managed_min_load = 0.;
    m->managed_max_load = 1.;
    m->data = calloc(initial_capacity, sizeof(hmapitem_t*));
    m->capacity = initial_capacity;
    m->hash = hash;
    m->equals = equals;
}

void hmap_init(hmap_t* m, HMAP_HASH_TYPE(hash), HMAP_EQUALS_TYPE(equals)) {
    hmap_init_unmanaged(m, hash, equals, HMAP_INITIAL_CAPACITY);
    hmap_managed(m, .2, .6, HMAP_INITIAL_CAPACITY);
}

void hmap_managed(hmap_t* m, float min_load, float max_load, size_t min_capacity) {
    assert(m != NULL);

    m->managed = true;
    m->managed_min_load = min_load;
    m->managed_max_load = max_load;
    m->managed_min_capacity = min_capacity;
}

void hmap_unmanaged(hmap_t* m) {
    assert(m != NULL);

    m->managed = false;
}

int hmap_manage(hmap_t* m) {
    assert(m != NULL);

    float min_lf = m->managed_min_load;
    float max_lf = m->managed_max_load;
    size_t min_mc = m->managed_min_capacity;
    float lf = hmap_stats_load_factor(m);

    int ret = 0;
    if (lf < min_lf && (m->capacity / 2) >= min_mc) {  // only shrink if new capacity would be > min_mc
        hmap_unmanaged(m);
        hmap_adjust_capacity(m, m->capacity / 2.);
        hmap_managed(m, min_lf, max_lf, min_mc);
        ret = -1;
    } else if (lf > max_lf) {  // only grow if load factor exceeds max load factor
        hmap_unmanaged(m);
        hmap_adjust_capacity(m, m->capacity * 2.);
        hmap_managed(m, min_lf, max_lf, min_mc);
        ret = 1;
    }
    return ret;
}

void hmap_adjust_capacity(hmap_t* m, size_t new_capacity) {
    assert(m != NULL);

    size_t length = hmap_length(m);
    assert(new_capacity >= length);

    size_t capacity = hmap_capacity(m);
    hmapitem_t** data = m->data;

    m->data = calloc(new_capacity, sizeof(hmapitem_t*));
    m->capacity = new_capacity;
    m->length = 0;

    void* key = NULL;
    for (size_t i = 0; i < capacity; i++) {
        if (data[i] == NULL) {
            continue;
        }
        key = data[i]->key;
        data[i]->map_ptr = NULL;
        data[i]->key = NULL;
        hmap_set(m, key, data[i]);
    }

    assert(length == hmap_length(m));

    free(data);
}

void hmap_rehash(hmap_t* m, HMAP_HASH_TYPE(hash), HMAP_EQUALS_TYPE(equals)) {
    assert(m != NULL);

    m->hash = hash;
    m->equals = equals;
    hmap_adjust_capacity(m, m->capacity);
}

void hmap_rehash_to(hmap_t* source, hmap_t* target) {
    assert(source != NULL);
    assert(target != NULL);

    size_t source_length = hmap_length(source);
    assert(target->managed || source_length <= (hmap_capacity(target) - hmap_length(target)));

    void* key = NULL;
    for (size_t i = 0; i < source_length; i++) {
        if (source->data[i] == NULL) {
            continue;
        }
        key = source->data[i]->key;
        source->data[i]->map_ptr = NULL;
        source->data[i]->key = NULL;
        source->length--;
        hmap_set(target, key, source->data[i]);
    }

    assert(source->length == 0);
}

void hmap_destroy(hmap_t* m) {
    assert(m != NULL);
    free(m->data);
    memset(m, 0, sizeof(hmap_t));
}

size_t hmap_length(hmap_t* m) {
    assert(m != NULL);
    return m->length;
}

size_t hmap_capacity(hmap_t* m) {
    assert(m != NULL);
    return m->capacity;
}

float hmap_stats_load_factor(hmap_t* m) {
    assert(m != NULL);
    return m->length / (float)m->capacity;
}

size_t hmap_stats_last_set_collisions(hmap_t* m) {
    assert(m != NULL);
    return m->last_set_collisions;
}

void hmap_set(hmap_t* m, void* key, hmapitem_t* i) {
    assert(m != NULL);
    assert(m->capacity > 0);
    assert(m->capacity > m->length);
    assert(m->hash != NULL);
    assert(m->equals != NULL);
    assert(i->map_ptr == NULL);
    assert(i->key == NULL);

    size_t length = m->length;
    size_t collisions = 0;
    size_t index = m->hash(key) % m->capacity;
    while (m->data[index] != NULL && !m->equals(m->data[index]->key, key)) {
        index = (index + 1) % m->capacity;
        collisions++;
    }

    if (m->data[index] != NULL) {
        m->data[index]->map_ptr = NULL;
        m->data[index]->key = NULL;
        m->length--;
    }

    m->data[index] = i;
    m->data[index]->map_ptr = m;
    m->data[index]->key = key;
    m->length++;
    m->last_set_collisions = collisions;

    if (m->managed && length != m->length) {
        hmap_manage(m);
    }
}

hmapitem_t** hmap_internal_find(hmap_t* m, void* key) {
    assert(m != NULL);

    if (hmap_length(m) == 0) {
        return NULL;
    }

    size_t hash = m->hash(key);
    size_t start_index = hash % m->capacity;
    size_t index = start_index;
    while (m->data[index] != NULL) {
        if (m->equals(m->data[index]->key, key)) {
            return &m->data[index];
        }

        if (((index + 1) % m->capacity) == start_index) {
            return m->equals(m->data[index]->key, key) ? &m->data[index] : NULL;
        }

        index = (index + 1) % m->capacity;
    }
    return NULL;
}

bool hmap_has(hmap_t* m, void* key) {
    assert(m != NULL);

    return hmap_get(m, key) != NULL;
}

hmapitem_t* hmap_get(hmap_t* m, void* key) {
    assert(m != NULL);

    hmapitem_t** i_ptr = hmap_internal_find(m, key);
    if (i_ptr == NULL) {
        return NULL;
    }
    return *hmap_internal_find(m, key);
}

hmapitem_t* hmap_delete(hmap_t* m, void* key) {
    assert(m != NULL);

    hmapitem_t** item_ptr = hmap_internal_find(m, key);

    if (item_ptr == NULL) {
        return NULL;
    }

    hmapitem_t* item = *item_ptr;
    *item_ptr = NULL;

    item->map_ptr = NULL;
    item->key = NULL;
    m->length--;

    /*
    Lets assume a scenario like this:

    hash(a) = 3 = x
    hash(b) = 2 = y
    hash(c) = 3 = x
    hash(d) = 3 = x
    set(a)
    set(b)
    set(c)
    set(d)

    [b, c, d, a]

    When delete(a): resetting rel0 to 3 maps the data to:

    [a, b, c, d]

    Now the running index will always grow and the checks for b, c and d are easier, since we only need to check if they
    need to be moved left (WITHOUT wrapping to counter the modulo operation) [0, b, c, d]
     */

    size_t rel0 = m->hash(key) % m->capacity;
    size_t index = (item_ptr - m->data - rel0) % m->capacity;
    size_t shift_to_index = index;

    while ((++index) < m->capacity && m->data[(index + rel0) % m->capacity] != NULL) {
        size_t hash_index = ((m->hash(m->data[(index + rel0) % m->capacity]->key) - rel0) % m->capacity);
        if (hash_index > shift_to_index) {
            continue;
        }

        m->data[(shift_to_index + rel0) % m->capacity] = m->data[(index + rel0) % m->capacity];
        m->data[(index + rel0) % m->capacity] = NULL;
        shift_to_index = index;
    }

    if (m->managed) {  // no need to check for length change here since short circuit if key not found
        hmap_manage(m);
    }

    return item;
}

void hmap_foreach(hmap_t* m, void (*iter)(void* key, hmapitem_t*, void*), void* userdata) {
    assert(m != NULL);
    assert(iter != NULL);

    for (size_t i = 0; i < m->capacity; i++) {
        if (m->data[i] != NULL) {
            iter(m->data[i]->key, m->data[i], userdata);
        }
    }
}

void hmapitem_init(hmapitem_t* i) {
    assert(i != NULL);
    i->key = NULL;
    i->map_ptr = NULL;
}

bool hmapitem_in_map(hmapitem_t* i, hmap_t* m) {
    assert(i != NULL);
    assert(m != NULL);
    return i->map_ptr == m;
}

#endif
#endif
