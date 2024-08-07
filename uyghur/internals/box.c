// box

#include "../uyghur.c"

void native_box_iterate(Bridge *bridge)
{
    Value *box = Bridge_receiveValue(bridge, UG_TYPE_CNT);
    Value *wkr = Bridge_receiveValue(bridge, UG_TYPE_WKR);
    Container *_box = box->object;
    void (*_wkr)() = wkr->object;
    // 
    Executer *exe = bridge->uyghur->executer;
    Hashmap *map = _box->map;
    Hashkey *ptr;
    for (int i = 0; i < HASHMAP_DEFAULT_CAPACITY; ++i) {
        ptr = map[i].position;
        while (ptr != NULL) {
            String *_key = ptr->key;
            Value *key = Value_newString(_key, NULL);
            Value *val = ptr->value;
            Executer_clearStack(exe);
            Executer_pushStack(exe, val);
            Executer_pushStack(exe, key);
            Executer_applyWorker(exe, wkr, NULL);
            // 
            ptr = ptr->next;
        }
    }
    //
    Bridge_returnEmpty(bridge);
}

void lib_box_register(Bridge *bridge)
{
    Bridge_startBox(bridge);
    //
    BRIDGE_BIND_NATIVE(box_iterate);
    //
    Bridge_register(bridge, ALIAS_BOX);
}
