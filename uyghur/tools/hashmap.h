// hashmap

#ifndef H_PCT_HASHMAP
#define H_PCT_HASHMAP

#define HASHMAP_DEFAULT_CAPACITY 4097

typedef struct _Hashmap {
    struct _Object;
    int size;
    Hashkey *position;
} Hashmap;

Hashmap* Hashmap_new() {
    Hashmap *map = (Hashmap *)pct_mallloc(sizeof(Hashmap) * HASHMAP_DEFAULT_CAPACITY);
    map->size = HASHMAP_DEFAULT_CAPACITY;
    for (int i = 0; i < map->size; ++i ) map[i].position = NULL;
    Object_init(map, PCT_OBJ_HASHMAP);
    return map;
}

// TODO: release removed value
void Hashmap_free(Hashmap *this) {
    assert(this != NULL);
    Hashkey *ptr;
    Hashkey *tmp;
    for (int i = 0; i < HASHMAP_DEFAULT_CAPACITY; ++i) {
        ptr = this[i].position;
        while (ptr != NULL) {
            tmp = ptr;
            ptr = ptr->next;
            Object_release(tmp);
        }
    }
    Object_free(this);
}

void *Hashmap_set(Hashmap *this, char *_key, void *value) {
    assert(this != NULL);
    assert(_key != NULL);
    assert(value != NULL);
    String *key = String_format(_key);
    int pos = String_hash(key) % HASHMAP_DEFAULT_CAPACITY;
    //
    void *tmp = NULL;
    Hashkey *ptr = this[pos].position;
    if (ptr == NULL) {
        this[pos].position = Hashkey_new(key, value);
        Object_release(key);
        return NULL;
    }
    while (ptr != NULL) {
        if (String_equal(key, ptr->key)) {
            tmp = ptr->value;
            Hashkey_set(ptr, value);
            Object_release(key);
            // TODO: release tmp
            return tmp;
        }
        ptr = ptr->next;
    }
    Hashkey *pnode = Hashkey_new(key, value);
    Object_release(key);
    pnode->next = this[pos].position;
    this[pos].position = pnode;
    return NULL;
}

void *Hashmap_get(Hashmap *this, char *_key) {
    assert(this != NULL);
    assert(_key != NULL);
    String *key = String_format(_key);
    int pos = String_hash(key) % HASHMAP_DEFAULT_CAPACITY;
    //
    Hashkey *ptr = this[pos].position;
    while (ptr != NULL) {
        if (String_equal(key, ptr->key)) {
            Object_release(key);
            return ptr->value;
        }
        ptr = ptr->next;
    }
    Object_release(key);
    return NULL;
}

// TODO: release removed value
void *Hashmap_del(Hashmap *this, char *_key) {
    assert(this != NULL);
    assert(_key != NULL);
    String *key = String_format(_key);
    int pos = String_hash(key) % HASHMAP_DEFAULT_CAPACITY;
    //
    void *tmp = NULL;
    Hashkey *ptr = this[pos].position;
    if (ptr == NULL) {
        Object_release(key);
        return NULL;
    }
    Hashkey *pre = NULL;
    while (ptr != NULL) {
        if (String_equal(key, ptr->key)) {
            if (pre == NULL) {
                this[pos].position = NULL;
            } else {
                pre->next = ptr->next;
            }
            tmp = ptr->value;
            Object_release(ptr);
            break;
        }
        pre = ptr;
        ptr = ptr->next;
    }
    Object_release(key);
    return tmp;
}

char *Hashmap_toString(Hashmap *this)
{
    return tools_format("[Hashmap => p:%p s:%i]", this, this->size);
}

#endif
