// hashmap

#ifndef H_HASHMAP
#define H_HASHMAP

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#define CAPACITY 4097

typedef struct EntryNode{
    char *key;
    void *value;
    struct EntryNode *next;
} Entry;

typedef struct _Hashmap {
    int size;
    Entry *position;
} Hashmap;

Hashmap *Hashmap_new();
void* Hashmap_set(Hashmap *, char *, void *);
void* Hashmap_get(Hashmap *, char *);
void Hashmap_del(Hashmap *, char *);
void Hashmap_print(Hashmap *);
void Hashmap_free(Hashmap *);

static unsigned long hashcode(char *key) {
    register unsigned long hash = 0;
    unsigned long ch;
    while (ch = (unsigned long)*key++) {
        hash = hash * 131 + ch;
    }
    return hash;
}

static int hash(char *key) {
    return hashcode(key) % CAPACITY;
}

Entry *_hashmap_new_entry(char *key, char *value)
{
    Entry *pnode = (Entry *)malloc(sizeof(Entry));
    pnode->key = key;
    pnode->value = value;
    pnode->next = NULL;
    return pnode;
}

Hashmap* Hashmap_new() {
    Hashmap *map = (Hashmap *)malloc(sizeof(Hashmap) * CAPACITY);
    map->size = CAPACITY;
    for (int i = 0; i < CAPACITY; ++i ) {
         map[i].position = NULL;
    }
    return map;
}

void Hashmap_free(Hashmap *this) {
    Entry *ptr;
    Entry *head;
    for (int i = 0; i < CAPACITY; ++i) {
        ptr = this[i].position;
        while (ptr != NULL) {
            head = ptr;
            ptr = ptr->next;
            free(head);
        }
    }
    free(this);
}

void *Hashmap_setWithHash(Hashmap *this, char *key, void *value, int hashValue) {
    assert(this != NULL);
    int pos = hashValue;
    void *tmp = NULL;
    Entry *ptr = this[pos].position;
    if (ptr == NULL) {
        this[pos].position = _hashmap_new_entry(key, value);
        return NULL;
    } else {
        while (ptr != NULL) {
            if (!strcmp(key, ptr->key)) {
                tmp = ptr->value;
                ptr->value = value;
                return tmp;
            }
            ptr = ptr->next;
        }
        Entry *pnode = _hashmap_new_entry(key, value);
        pnode->next = this[pos].position;
        this[pos].position = pnode;
        return NULL;
    }
}

void *Hashmap_set(Hashmap *this, char *key, void *value) {
    int hashValue = hash(key);
    return Hashmap_setWithHash(this, key, value, hashValue);
}

void *Hashmap_fill(Hashmap *this, char *content) {
    return Hashmap_set(this, content, content);
}

void *Hashmap_getWithHash(Hashmap *this, char *key, int hashValue) {
    assert(this != NULL);
    int pos = hashValue;
    Entry *ptr = this[pos].position;
    while (ptr != NULL) {
        if (!strcmp(key, ptr->key)) {
            return ptr->value;
        }
        ptr = ptr->next;
    }
    return NULL;
}

void *Hashmap_get(Hashmap *this, char *key) {
    int hashValue = hash(key);
    return Hashmap_getWithHash(this, key, hashValue);
}

void Hashmap_del(Hashmap *this, char *key) {
    assert(this != NULL);
    int pos = hash(key);
    Entry *ptr = this[pos].position;
    Entry *pre = ptr;
    if (ptr->next == NULL) {
        free(ptr);
        this[pos].position = NULL;
        return;
    }
    while (ptr != NULL) {
        if (!strcmp(key, ptr->key)) {
            pre->next = ptr->next;
            free(ptr);
            return;
        }
        pre = ptr;
        ptr = ptr->next;
    }
}

Cursor *Hashmap_reset(Hashmap *this)
{
    return Cursor_new(NULL);
}

char *Hashmap_next(Hashmap *this, Cursor *cursor)
{
    void *checkItem = Cursor_get(cursor);
    bool emptyCursor = checkItem == NULL;
    bool returnNext = emptyCursor == true;
    if (checkItem) returnNext = true;
    for (int i = 0; i < CAPACITY; i++) {
        if (this[i].position != NULL) {
            Entry *ptr = this[i].position;
            while (ptr != NULL) {
                if (returnNext)
                {
                    if (emptyCursor)
                    {
                        Cursor_set(cursor, ptr);
                    }
                    return ptr->key;
                }
                else if (checkItem == ptr)
                {
                    returnNext = true;
                }
                ptr = ptr->next;
            }
        }
    } 
    return NULL;
}

void Hashmap_foreach(Hashmap *this, void *bindObj, void (*func)(void *, char *, void *))
{
    int i;
    for (i = 0; i < CAPACITY; i++) {
        if (this[i].position != NULL) {
            Entry *ptr = this[i].position;
            while (ptr != NULL) {
                func(bindObj, ptr->key, ptr->value);
                ptr = ptr->next;
            }
        }
    }  
}

void print_map_item(void *obj, char *key, void *data)
{
    printf(" %s -> %p\n", key, (void*)&data); 
}

void Hashmap_print(Hashmap *this) {
    int i;
    printf("[Hashmap_start] address:%p capacity:%d\n", (void*)&this, CAPACITY);
    Hashmap_foreach(this, NULL, print_map_item);
    printf("[Hashmap_end] address:%p", (void*)&this);
}

#endif
