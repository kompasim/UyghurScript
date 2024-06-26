// file

#include "../uyghur/uyghur.c"

void ug_write_file(Bridge *bridge)
{
    char *path = Bridge_receiveString(bridge);
    char *content = Bridge_receiveString(bridge);
    bool r = file_write(path, content);
    Bridge_returnBoolean(bridge, r);
}

void ug_read_file(Bridge *bridge)
{
    char *path = Bridge_receiveString(bridge);
    char *r = file_read(path);
    if (r != NULL) {
        Bridge_returnString(bridge, r);
        pct_free(r);
    } else {
        Bridge_returnEmpty(bridge);
    }
}

void ug_file_copy(Bridge *bridge)
{
    char *from = Bridge_receiveString(bridge);
    char *to = Bridge_receiveString(bridge);
    bool r = file_copy(from, to);
    Bridge_returnBoolean(bridge, r);
}

void ug_file_rename(Bridge *bridge)
{
    char *from = Bridge_receiveString(bridge);
    char *to = Bridge_receiveString(bridge);
    double r = file_rename(from, to);
    Bridge_returnNumber(bridge, r);
}

void ug_file_remove(Bridge *bridge)
{
    char *path = Bridge_receiveString(bridge);
    double r = file_remove(path);
    Bridge_returnNumber(bridge, r);
}

void ug_file_exist(Bridge *bridge)
{
    char *path = Bridge_receiveString(bridge);
    bool r = file_exist(path);
    Bridge_returnBoolean(bridge, r);
}

void ug_file_is_file(Bridge *bridge)
{
    char *path = Bridge_receiveString(bridge);
    bool r = file_is_file(path);
    Bridge_returnBoolean(bridge, r);
}

void ug_file_is_directory(Bridge *bridge)
{
    char *path = Bridge_receiveString(bridge);
    bool r = file_is_directory(path);
    Bridge_returnBoolean(bridge, r);
}

void ug_file_create_directory(Bridge *bridge)
{
    char *path = Bridge_receiveString(bridge);
    int r = file_create_directory(path);
    Bridge_returnNumber(bridge, r);
}

void lib_file_register(Bridge *bridge)
{
    Bridge_startBox(bridge);
    //
    Bridge_bindNative(bridge, "yezish", ug_write_file);
    Bridge_bindNative(bridge, "oqush", ug_read_file);
    Bridge_bindNative(bridge, "kuchurush", ug_file_copy);
    Bridge_bindNative(bridge, "yotkesh", ug_file_rename);
    Bridge_bindNative(bridge, "uchurush", ug_file_remove);
    Bridge_bindNative(bridge, "buMewjutmu", ug_file_exist);
    Bridge_bindNative(bridge, "buXojjetmu", ug_file_is_file);
    Bridge_bindNative(bridge, "buSomkimu", ug_file_is_directory);
    Bridge_bindNative(bridge, "somkaQurush", ug_file_create_directory);
    //
    Bridge_register(bridge, "xojjet");
}
