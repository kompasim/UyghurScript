// sound

#include "raylib.h"
#include "../uyghur/uyghur.c"

// tool

char *get_audio_tag_for_sound(char *path)
{
    return tools_format("<R-SOUND:%s>", path);
}

Sound raylib_load_sound_by_tag(char *tag, char *path)
{
    Sound *sound = Hashmap_get(resourcesMap, tag);
    if (sound != NULL) {
        return sound[0];
    }
    Sound s = LoadSound(path);
    sound = (Sound *)malloc(sizeof(s));
    sound[0] = s;
    Hashmap_set(resourcesMap, tag, sound);
    return s;
}

Sound raylib_get_sound_by_tag(char *tag)
{
    Sound *sound = Hashmap_get(resourcesMap, tag);
    if (sound == NULL) return LoadSound("");
    return sound[0];
}

void raylib_unload_sound_by_tag(char *tag)
{
    Sound *sound = Hashmap_get(resourcesMap, tag);
    if (sound == NULL) return;
    UnloadSound(sound[0]);
}

// api

void ug_board_sound_load(Bridge *bridge)
{
    char *path = Bridge_receiveString(bridge);
    char *tag = get_audio_tag_for_sound(path);
    raylib_load_sound_by_tag(tag, path);
    Bridge_returnString(bridge, tag);
}

void ug_board_sound_unload(Bridge *bridge)
{
    char *tag = Bridge_receiveString(bridge);
    raylib_unload_sound_by_tag(tag);
    Bridge_returnEmpty(bridge);
}

void ug_board_sound_play(Bridge *bridge)
{
    char *tag = Bridge_receiveString(bridge);
    Sound sound = raylib_get_sound_by_tag(tag);
    PlaySound(sound);
    Bridge_returnEmpty(bridge);
}

void ug_board_sound_stop(Bridge *bridge)
{
    char *tag = Bridge_receiveString(bridge);
    Sound sound = raylib_get_sound_by_tag(tag);
    StopSound(sound);
    Bridge_returnEmpty(bridge);
}

void ug_board_sound_resume(Bridge *bridge)
{
    char *tag = Bridge_receiveString(bridge);
    Sound sound = raylib_get_sound_by_tag(tag);
    ResumeSound(sound);
    Bridge_returnEmpty(bridge);
}

void ug_board_sound_pause(Bridge *bridge)
{
    char *tag = Bridge_receiveString(bridge);
    Sound sound = raylib_get_sound_by_tag(tag);
    PauseSound(sound);
    Bridge_returnEmpty(bridge);
}

void ug_board_sound_is_playing(Bridge *bridge)
{
    char *tag = Bridge_receiveString(bridge);
    Sound sound = raylib_get_sound_by_tag(tag);
    bool r = IsSoundPlaying(sound);
    Bridge_returnBoolean(bridge, r);
}

void ug_board_sound_set_volume(Bridge *bridge)
{
    char *tag = Bridge_receiveString(bridge);
    Sound sound = raylib_get_sound_by_tag(tag);
    float volume = Bridge_receiveNumber(bridge);
    SetSoundVolume(sound, volume);
    Bridge_returnEmpty(bridge);
}

// other

void lib_raylib_sound_register(Bridge *bridge)
{
    //
    Bridge_startBox(bridge);
    //
    Bridge_bindNative(bridge, "ekirish", ug_board_sound_load);
    Bridge_bindNative(bridge, "tazilash", ug_board_sound_unload);
    Bridge_bindNative(bridge, "quyush", ug_board_sound_play);
    Bridge_bindNative(bridge, "toxtitish", ug_board_sound_stop);
    Bridge_bindNative(bridge, "turghuzush", ug_board_sound_pause);
    Bridge_bindNative(bridge, "mangghuzush", ug_board_sound_resume);
    Bridge_bindNative(bridge, "quyuliwatamdu", ug_board_sound_is_playing);
    Bridge_bindNative(bridge, "yuqiriliqiniBikitish", ug_board_sound_set_volume);
    //
    Bridge_register(bridge, "awaz");
}
