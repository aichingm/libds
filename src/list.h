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
    LISTITEM()
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
}
```

## License APGL

Copyright (C) 2024 Mario Aichinger <aichingm@gmail.com>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, version 3.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.

*/

// TODO remove al *_item functions ust use LISTITEM_AS everywhere a listitem needs to be converted

#ifndef DS_LIST_H
#define DS_LIST_H
#include <stddef.h>

/**
 * Type to the indices and lengths.
 */
#define LIST_LEN_TYPE unsigned int

/**
 * Inject a listitem_t stuct with a given name into a struct.
 */
#define LISTITEM_PROP_s(item_name) listitem_t item_name

/**
 * Convert a type + pointer + attribute name to a parameter list (pointer, offset).
 */
#define LISTITEM_OF_s(type, data_ptr, item_name) data_ptr, offsetof(type, item_name)

/**
 * Inject a listitem_t struct with the default attribute name into a struct.
 */
#define LISTITEM_PROP() LISTITEM_s(default_list_item_name)

/**
 * Convert a type + pointer to a parameter list (pointer, offset) using the default attribute name.
 */
#define LISTITEM_OF(type, data_ptr) LISTITEM_OF_s(type, data_ptr, default_list_item_name)

/**
 * Convert a type + pointer + name to a pointer to a listitem_t
 */
#define LISTITEM_PTR_s(type, data_ptr, item_name) (listitem_t*)(((char*) data_ptr) + offsetof(type, item_name))


#define LISTITEM_AS(type, ptr) ((type*)(((char*) ptr) - ((char*) offsetof(type, default_list_item_name))))
#define LISTITEM_AS_s(type, ptr, property_name) ((type*)(((char*) ptr) - ((char*) offsetof(type, property_name))))

/**
 * Convert a type + pointer + offset to a pointer to a listitem_t.
 */
#define LISTITEM_PTR(type, data_ptr) LISTITEM_PTR_s(type, data_ptr, default_list_item_name)

/**
 * Convert a type + listitem_t to a pointer to userdefined struct.
 */
#define LISTITEM_TO_DATA(type, item) (type) (((char*)item) - ((char*)item->offset))

// TODO 
/**
 * A struct 
 */
typedef struct listitem_s {
    unsigned char state;
    size_t offset;
    void* list_ptr;
    struct listitem_s* prev;
    struct listitem_s* next;
} listitem_t;

/**
 * A struct which holds a list of items.
 */
typedef struct {
    listitem_t* first;
    listitem_t* last;
    LIST_LEN_TYPE length;
} list_t;

/**
 * Iterate over the items of a list.
 */
#define LIST_ITER_ITEM(iter_name, begin) for(listitem_t* iter_name = begin; iter_name != NULL; iter_name = iter_name->next)

/**
 * Iterate in reverse order over the items of a list.
 */
#define LIST_ITER_REVERSE_ITEM(iter_name, begin) for(listitem_t* iter_name = begin; iter_name != NULL; iter_name = iter_name->prev)

/**
 * Initialize a list.
 */
void list_init(list_t* l);

/**
 * Returns the number of items in the list.
 */
LIST_LEN_TYPE list_len(list_t* l);

/**
 * Add an item to the end of the list.
 */
void list_push(list_t* l, void* data, size_t offset);

/**
 * Add an item to the end of the list.
 */
void list_push_item(list_t* l, listitem_t* i);

/**
 * Remove an item from the end of the list.
 */
void* list_pop(list_t* l);

/**
 * Add an item to the beginning of the list.
 */
void list_unshift(list_t* l, void* data, size_t offset);

/**
 * Add an item to the beginning of the list.
 */
void list_unshift_item(list_t* l, listitem_t* i);

/**
 * Remove an item from the beginning of the list.
 */
void* list_shift(list_t* l);

/**
 * Get the nth item in the list.
 */
void* list_get(list_t* l, LIST_LEN_TYPE i);

/**
 * Get the nth item in the list.
 */
listitem_t* list_get_item(list_t* l, LIST_LEN_TYPE i);

/**
 * Remove the nth item from the list and return the associated data.
 */
void* list_remove_index(list_t* l, LIST_LEN_TYPE i);

/**
 * Check if the list contains a given item.
 */
bool list_contains(list_t* l, void* data, size_t offset);

/**
 * Check if the list contains a given item.
 */
bool list_contains_item(list_t* l, listitem_t* i);

/**
 * Check if the list contains a given data pointer.
 */
bool list_contains_data(list_t* l, void* data);

/**
 * Iterate over the data in the list by calling the iter function with every data pointer.
 */
void list_foreach(list_t* l, void (*iter)(void* , void* ), void* userdata);

/**
 * Iterate over the items in the list by calling the iter function with every item.
 */
void list_foreach_item(list_t* l, void (*iter)(listitem_t* , void* ), void* userdata);

/**
 * Iterate, in reverse order, over the data in the list by calling the iter function with every data pointer.
 */
void list_foreach_reverse(list_t* l, void (*iter)(void* , void* ), void* userdata);

/**
 * Iterate, in reverse order, over the items in the list by calling the iter function with every item.
 */
void list_foreach_revere_item(list_t* l, void (*iter)(listitem_t* , void* ), void* userdata);

/**
 * Write all data pointers contained in the list to a caller managed array.
 */
void list_to_array(list_t* l, void** array);

/**
 * Initializte a listitem.
 */
void listitem_init(void* data, size_t offset);

/**
 * Set the offset for a given listitem.
 */
void listitem_set_offset(listitem_t* data, size_t offset);

/**
 * Check if the given listitem has an offset set.
 */
bool listitem_has_offset(listitem_t* i);

/**
 * Check if the given listitem is currently contained in a list.
 */
bool listitem_used(listitem_t* i);

/**
 * Remove the listitem from the list it currently is contained in.
 */
void listitem_remove(void* data, size_t offset);

/**
 * Remove the listitem from the list it currently is contained in.
 */
void listitem_remove_item(listitem_t* i);

#define IMPL_LIST
#ifdef IMPL_LIST

#define LISTITEM_STATE_OFFSET_INITIALIZED 0b1
#include <assert.h>

void list_init(list_t* l) {
    l->length = 0;
    l->first = NULL;
    l->last = NULL;
}

LIST_LEN_TYPE list_len(list_t* l) {
    return l->length;
}

void list_push(list_t* l, void* data, size_t offset) {
    listitem_t* i = (listitem_t *)(((char *)data) + offset);
    listitem_set_offset(i, offset);
    list_push_item(l, i);
}

void list_push_item(list_t* l, listitem_t* i) {

    assert(listitem_has_offset(i) && "listitem_t: offset is set");
    assert(!listitem_used(i) && "listitem_t: is not part of a list");

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

void* list_pop(list_t* l) {
    if (l->length == 0) {
	return NULL;
    }

    listitem_t* i = l->last;
    void* data_ptr = LISTITEM_TO_DATA(void* , i);

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

    return data_ptr;
}

void list_unshift(list_t* l, void* data, size_t offset) {
    listitem_t* i = (listitem_t *)(((char *)data) + offset);
    listitem_set_offset(i, offset);
    list_unshift_item(l, i);
}

void list_unshift_item(list_t* l, listitem_t* i) {

    assert(listitem_has_offset(i) && "listitem_t: offset not set");
    assert(!listitem_used(i) && "listitem_t: part of a list (or not correctly initialized)");

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

void* list_shift(list_t* l) {
    if (l->length == 0) {
	return NULL;
    }

    listitem_t* i = l->first;
    void* data_ptr = LISTITEM_TO_DATA(void* , i);

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

    return data_ptr;
}

void* list_get(list_t* l, LIST_LEN_TYPE index) {
    listitem_t* i = list_get_item(l, index);
    if (i == NULL) {
	return NULL;
    }
    return (void *)(((char *)i) - ((char *)i->offset));
}

listitem_t* list_get_item(list_t* l, LIST_LEN_TYPE index) {
    if (index >= list_len(l)) {
	return NULL;
    }

    LIST_LEN_TYPE idx = 0;
    LIST_ITER_ITEM(elem, l->first) {
	if (idx == index) {
	    return elem;
	}
	idx++;
    }
    return NULL;
}

void* list_remove_index(list_t* l, LIST_LEN_TYPE index) {
    listitem_t* i = list_get_item(l, index);
    if (i == NULL) {
	return NULL;
    }

    listitem_remove_item(i);
    return LISTITEM_TO_DATA(void* , i);
}

bool list_contains(list_t* l, void* data, size_t offset) {
    listitem_t* i = (listitem_t *)(((char *)data) + offset);
    return list_contains_item(l, i);
}

bool list_contains_item(list_t* l, listitem_t* i) {
    return i->list_ptr == l;
}

bool list_contains_data(list_t* l, void* data) {
    LIST_ITER_ITEM(item, l->first) {
	void* iter_data = LISTITEM_TO_DATA(void* , item);
	if (iter_data == data) {
	    return true;
	}
    }
    return false;
}

void list_foreach(list_t* l, void (*iter)(void* , void* ), void* userdata) {
    LIST_ITER_ITEM(item, l->first) {
	void* data = LISTITEM_TO_DATA(void* , item);
	iter(data, userdata);
    }
}

void list_foreach_item(list_t* l, void (*iter)(listitem_t* , void* ), void* userdata) {
    LIST_ITER_ITEM(elem, l->first) {
	iter(elem, userdata);
    }
}

void list_foreach_reverse(list_t* l, void (*iter)(void* , void* ), void* userdata) {
    LIST_ITER_REVERSE_ITEM(item, l->last) {
	void* data = LISTITEM_TO_DATA(void* , item);
	iter(data, userdata);
    }
}

void list_foreach_reverse_item(list_t* l, void (*iter)(listitem_t* , void* ), void* userdata) {
    LIST_ITER_REVERSE_ITEM(elem, l->last) {
	iter(elem, userdata);
    }
}

void list_to_array(list_t* l, void** array) {
    LIST_LEN_TYPE i = 0;
    LIST_ITER_ITEM(elem, l->first) {
	array[i++] = (void *)(((char *)elem) - ((char *)elem->offset));
    }
}

void listitem_init(void* data, size_t offset) {
    listitem_t* i = (listitem_t *)(((char *)data) + offset);
    listitem_set_offset(i, offset);
    i->list_ptr = NULL;
    i->prev = NULL;
    i->next = NULL;
}

void listitem_set_offset(listitem_t* i, size_t offset) {
    i->offset = offset;
    i->state |= LISTITEM_STATE_OFFSET_INITIALIZED;
}

bool listitem_has_offset(listitem_t* i) {
    return i->state & LISTITEM_STATE_OFFSET_INITIALIZED;
}

bool listitem_used(listitem_t* i) {
    return i->list_ptr != NULL;
}

void listitem_remove(void* data, size_t offset) {
    listitem_t* i = (listitem_t *)(((char *)data) + offset);
    listitem_remove_item(i);
}

void listitem_remove_item(listitem_t* i) {

    assert(listitem_has_offset(i) && "listitem_t: offset not set");
    assert(listitem_used(i) && "listitem_t' is not part of any list_t");

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

