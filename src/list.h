
/*

# List

## Usage

### Include

To generate the implementations include the header with setting IMPL_LIST before. Do this only once e.g. in main.c

```
#define IMPL_LIST
#include "list.h"
```

After that include list.h like a normal header everywhere the declarations are needed
```
#include "list.h"
```
### Basic Usage

```
typedef struct {
    char name[32];
    double complex sex;
    LISTITEM_PROP()
} person;

void main() {
    list_t people;
    list_init(&people);

    person alex = {0}, riley = {0};

    alex.name = "Alex";
    alex.sex = 1.0 + 3.0 * I;

    riley.name = "Riley";
    riley.sex = 4.0 + 2.0 * I;

    list_push(&list LISTITEM_OF(person*, &alex));
    list_push(&list LISTITEM_OF(person*, &alex));

    person* p0 = LIST_GET(person, &l, 0);
}
```

## License APGL

Copyright (C) 2024 Mario Aichinger <aichingm@gmail.com>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, version 3.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.

*/

#ifndef DS_LIST_H
#define DS_LIST_H
#include <stddef.h>

/**
 * Type to the indices and lengths.
 */
#define LIST_LENGTH_TYPE unsigned int

// Remove an item from the end of the list and return a pointer to the struct holding the item.
#define LIST_POP_s(type, list, property_name) ((list)->last != NULL ? LISTITEM_AS_s(type, list_pop(list), property_name): NULL)

// Remove an item from the end of the list and return a pointer to the struct holding the item using the default item property name.
#define LIST_POP(type, list) LIST_POP_s(type, list, default_list_item_name)

// Remove an item from the beginning of the list and return a pointer to the struct holding the item.
#define LIST_SHIFT_s(type, list, property_name) ((list)->first != NULL ? LISTITEM_AS_s(type, list_shift(list), property_name) : NULL)

// Remove an item from the beginning of the list and return a pointer to the struct holding the item using the default item property name.
#define LIST_SHIFT(type, list) LIST_SHIFT_s(type, list, default_list_item_name)

// Get the nth item list and return a pointer to the struct holding the item.
#define LIST_GET_s(type, list, index, property_name) ((list_length(list) > index && index >= 0) ? LISTITEM_AS_s(type, list_get(list, index), property_name) : NULL)

// Get the nth item list and return a pointer to the struct holding the item using the default item property name.
#define LIST_GET(type, list, index) LIST_GET_s(type, list, index, default_list_item_name)

/**
 * Inject a listitem_t property with a given name into a struct.
 */
#define LISTITEM_PROP_s(item_name) listitem_t item_name

/**
 * Inject a listitem_t struct with the default property name into a struct.
 */
#define LISTITEM_PROP() LISTITEM_PROP_s(default_list_item_name)

/**
 * Convert a type + pointer + property name to a list item.
 */
#define LISTITEM_OF_s(type, data_ptr, item_name) (listitem_t*)(((char*)data_ptr) + offsetof(type, item_name))

/**
 * Convert a type + pointer to a list item using the default property name.
 */
#define LISTITEM_OF(type, data_ptr) LISTITEM_OF_s(type, data_ptr, default_list_item_name)

/**
 * Convert a listitem_t to a pointer to its holding struct using the offset of the given property's name.
 */
#define LISTITEM_AS_s(type, ptr, property_name) ((type*)(((char*) ptr) - ((char*) offsetof(type, property_name))))

/**
 * Convert a listitem_t to a pointer to its holding struct using the offset of the default property name.
 */
#define LISTITEM_AS(type, ptr) LISTITEM_AS_s(type, ptr, default_list_item_name)

/**
 * Calculate the offset of the default property name in bytes.
 */
#define LISTITEM_OFFSET(type) LISTITEM_OFFSET_s(type, default_list_item_name)

/**
 * Calculate the offset of a given property name in bytes.
 */
#define LISTITEM_OFFSET_s(type, property_name) offsetof(type, property_name)

typedef struct listitem_s {
    void* list_ptr;
    struct listitem_s* prev;
    struct listitem_s* next;
} listitem_t;

typedef struct {
    listitem_t* first;
    listitem_t* last;
    LIST_LENGTH_TYPE length;
} list_t;

/**
 * Iterate over the items of a list.
 */
#define LIST_ITER(iter_name, begin) for(listitem_t* iter_name = begin; iter_name != NULL; iter_name = iter_name->next)

/**
 * Iterate in reverse order over the items of a list.
 */
#define LIST_ITER_REVERSE(iter_name, begin) for(listitem_t* iter_name = begin; iter_name != NULL; iter_name = iter_name->prev)

/**
 * Initialize a list.
 */
void list_init(list_t* l);

/**
 * Returns the number of items in the list.
 */
LIST_LENGTH_TYPE list_length(list_t* l);

/**
 * Add an item to the end of the list.
 */
void list_push(list_t* l, listitem_t* i);

/**
 * Remove an item from the end of the list and return it.
 */
listitem_t* list_pop(list_t* l);

/**
 * Add an item to the beginning of the list.
 */
void list_unshift(list_t* l, listitem_t* i);

/**
 * Remove an item from the beginning of the list and return it.
 */
listitem_t* list_shift(list_t* l);

/**
 * Get the nth item in the list.
 */
listitem_t* list_get(list_t* l, LIST_LENGTH_TYPE i);

/**
 * Remove the item at the given index, return the item or NULL if the index points outside the list.
 */
listitem_t* list_remove_index(list_t* l, LIST_LENGTH_TYPE index);

/**
 * Check if the list contains a given item. Runtime O(n).
 */
bool list_contains(list_t* l, listitem_t* i);

/**
 * Iterate over the items in the list by calling the iter function with every item. Runtime O(n)
 */
void list_foreach(list_t* l, void (*iter)(listitem_t* , void* ), void* userdata);

/**
 * Iterate, in reverse order, over the items in the list by calling the iter function with every item. Runtime O(n).
 */
void list_foreach_revere(list_t* l, void (*iter)(listitem_t* , void* ), void* userdata);

/**
 * Write all item pointers contained in the list to a caller managed array. Runtime O(n).
 */
void list_to_array(list_t* l, listitem_t** array);

/**
 * Write all data pointers contained in the list to a caller managed array. Runtime O(n).
 */
void list_data_to_array(list_t* l, void** array, size_t ptr_offset);

/**
 * Remove the item from the list it currently is contained in.
 */
void listitem_unlink(listitem_t* i);

/**
 * Check if the item is currently in a list.
 */
bool listitem_in_list(listitem_t* i);

#define IMPL_LIST
#ifdef IMPL_LIST

#define LISTITEM_STATE_OFFSET_INITIALIZED 0b1
#include <assert.h>

void list_init(list_t* l) {
    l->length = 0;
    l->first = NULL;
    l->last = NULL;
}

LIST_LENGTH_TYPE list_length(list_t* l) {
    return l->length;
}

void list_push(list_t* l, listitem_t* i) {

    assert(!listitem_in_list(i));

    i->list_ptr = l;

    if (l->length == 0) {
	l->first = i;
	l->last = i;
    } else {
	i->prev = l->last;
	l->last->next = i;
	l->last = i;
    }
    l->length++;
}

listitem_t* list_pop(list_t* l) {
    if (l->length == 0) {
	return NULL;
    }

    listitem_t* i = l->last;

    // clean up list
    if (l->length == 1) {
	l->first = NULL;
	l->last = NULL;
	l->length = 0;
    } else {
	l->last = i->prev;
	i->prev->next = NULL;
	l->length--;
    }

    // cleanup item
    i->prev = NULL;
    i->next = NULL;
    i->list_ptr = NULL;

    return i;
}

void list_unshift(list_t* l, listitem_t* i) {

    assert(!listitem_in_list(i));

    i->list_ptr = l;

    if (l->length == 0) {
	l->first = i;
	l->last = i;
    } else {
	l->first->prev = i;
	i->next = l->first;
	l->first = i;
    }
    l->length++;
}

listitem_t* list_shift(list_t* l) {
    if (l->length == 0) {
	return NULL;
    }

    listitem_t* i = l->first;

    // clean up list
    if (l->length == 1) {
	l->first = NULL;
	l->last = NULL;
	l->length = 0;
    } else {
	l->first = i->next;
	i->next->prev = NULL;
	l->length--;
    }

    // cleanup item
    i->prev = NULL;
    i->next = NULL;
    i->list_ptr = NULL;

    return i;
}

listitem_t* list_get(list_t* l, LIST_LENGTH_TYPE index) {
    if (index >= list_length(l)) {
        return NULL;
    }

    LIST_LENGTH_TYPE idx = 0;
    LIST_ITER(elem, l->first) {
	if (idx == index) {
	    return elem;
	}
	idx++;
    }
    return NULL;
}

listitem_t* list_remove_index(list_t* l, LIST_LENGTH_TYPE index) {
    listitem_t* i = list_get(l, index);
    if (i == NULL) {
	return NULL;
    }

    listitem_unlink(i);
    return i;
}

bool list_contains(list_t* l, listitem_t* i) {
    return i->list_ptr == l;
}

void list_foreach(list_t* l, void (*iter)(listitem_t* , void* ), void* userdata) {
    LIST_ITER(elem, l->first) {
	iter(elem, userdata);
    }
}

void list_foreach_reverse(list_t* l, void (*iter)(listitem_t* , void* ), void* userdata) {
    LIST_ITER_REVERSE(elem, l->last) {
	iter(elem, userdata);
    }
}

void list_to_array(list_t* l, listitem_t** array) {
    LIST_LENGTH_TYPE i = 0;
    LIST_ITER(elem, l->first) {
	array[i++] = elem;
    }
}

void list_data_to_array(list_t* l, void** array, size_t offset) {
    LIST_LENGTH_TYPE i = 0;
    LIST_ITER(elem, l->first) {
	array[i++] = (void *)(((char *)elem) - ((char *)offset));
    }
}

void listitem_init(listitem_t* i) {
    i->list_ptr = NULL;
    i->prev = NULL;
    i->next = NULL;
}

bool listitem_in_list(listitem_t* i) {
    return i->list_ptr != NULL;
}

void listitem_unlink(listitem_t* i) {

    assert(listitem_in_list(i));

    list_t* l = i->list_ptr;

    if (l->first == i) {
	l->first = i->next;
    }

    if (l->last == i) {
	l->last = i->prev;
    }

    if (i->prev != NULL) {
	i->prev->next = i->next;
    }

    if (i->next != NULL) {
	i->next->prev = i->prev;
    }

    l->length--;

    i->prev = NULL;
    i->next = NULL;
    i->list_ptr = NULL;

}

#endif
#endif

