// system

#include "../uyghur.c"

void native_system_get_name(Bridge *bridge)
{
    Bridge_returnString(bridge, system_get_name());
}

void native_system_exit_program(Bridge *bridge)
{
    int code = 0;
    if (Bridge_topType(bridge) != UG_TYPE_NON) code = Bridge_receiveNumber(bridge);
    system_exit_program(code);
    Bridge_returnEmpty(bridge);
}

void native_system_execute_command(Bridge *bridge)
{
    char *command = Bridge_receiveString(bridge);
    char *r = system_execute_command(command);
    Bridge_returnString(bridge, r);
    pct_free(r);
}

void native_system_set_env(Bridge *bridge)
{
    char *name = Bridge_receiveString(bridge);
    char *value = Bridge_receiveString(bridge);
    system_set_env(name, value);
    Bridge_returnEmpty(bridge);
}

void native_system_get_env(Bridge *bridge)
{
    char *name = Bridge_receiveString(bridge);
    char *value = system_get_env(name);
    Bridge_returnString(bridge, value);
}

void lib_system_register(Bridge *bridge)
{
    Bridge_startBox(bridge);
    //
    BRIDGE_BIND_NATIVE(system_get_name);
    BRIDGE_BIND_NATIVE(system_exit_program);
    BRIDGE_BIND_NATIVE(system_execute_command);
    BRIDGE_BIND_NATIVE(system_set_env);
    BRIDGE_BIND_NATIVE(system_get_env);
    //
    Bridge_register(bridge, ALIAS_SYSTEM);
}
