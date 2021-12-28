// baord

#include "raylib.h"
#include "../uyghur/uyghur.c"

// data
Bridge *uyghurBridge;
Hashmap *resourcesMap;
Font defaultFont;
Image defaultImage;
Texture2D defaultTexture;

// tool

Color color_from_bridge(Bridge *bridge)
{
    char *str = Bridge_popString(bridge);
    int len = strlen(str);
    if (len != 6 && len != 8) return BLACK;
    int r = char_to_int(str[0]) * 16 + char_to_int(str[1]);
    int g = char_to_int(str[2]) * 16 + char_to_int(str[3]);
    int b = char_to_int(str[4]) * 16 + char_to_int(str[5]);
    int a = len == 6 ? 255 : char_to_int(str[6]) * 16 + char_to_int(str[7]);;
    return (Color){r, g, b, a};
}

Vector2 vector_from_bridge(Bridge *bridge)
{
    int x = Bridge_popNumber(bridge);
    int y = Bridge_popNumber(bridge);
    return (Vector2){x, y};
}

Rectangle rectangle_from_bridge(Bridge *bridge)
{
    int x = Bridge_popNumber(bridge);
    int y = Bridge_popNumber(bridge);
    int w = Bridge_popNumber(bridge);
    int h = Bridge_popNumber(bridge);
    return (Rectangle){x, y, w, h};
}

Image raylib_load_image(char *path)
{
    if (path == NULL) path = "";
    if (strlen(path) == 0) return defaultImage;
    Image *img;
    img = Hashmap_get(resourcesMap, path);
    if (img != NULL) {
        return img[0];
    }
    Image image = LoadImage(path);
    if (!image.data) image = defaultImage;
    img = (Image *)malloc(sizeof(image));
    img[0] = image;
    Hashmap_set(resourcesMap, path, img);
    return image;
}

void raylib_unload_image(char *path)
{
    if (path == NULL) path = "";
    Image *img = Hashmap_get(resourcesMap, path);
    if (img == NULL) return;
    Hashmap_del(resourcesMap, path);
    Image image = img[0];
    UnloadImage(image);
    free(img);
}

Font raylib_load_font(char *path)
{
    if (path == NULL) path = "";
    if (strlen(path) == 0) return defaultFont;
    Font *fnt;
    fnt = Hashmap_get(resourcesMap, path);
    if (fnt != NULL) {
        return fnt[0];
    }
    Font font = LoadFont(path);
    fnt = (Font *)malloc(sizeof(font));
    fnt[0] = font;
    Hashmap_set(resourcesMap, path, fnt);
    return font;
}

void raylib_unload_font(char *path)
{
    if (path == NULL) path = "";
    Font *fnt = Hashmap_get(resourcesMap, path);
    if (fnt == NULL) return;
    Hashmap_del(resourcesMap, path);
    Font image = fnt[0];
    UnloadFont(image);
    free(fnt);
}

typedef struct ImgInfo {
    char *path;
    int x;
    int y;
    int w;
    int h;
    int scaleX;
    int scaleY;
    bool flipX;
    bool flipY;
    int rotate;
    Color color;
} ImgInfo;

char *get_tex4img_tag(ImgInfo info)
{
    int x = info.x;
    int y = info.y;
    int w = info.w;
    int h = info.h;
    return tools_format(
        "<T-IMAGE:%s:%d,%d:%d,%d:%f%f:%s,%s:%d:%d,%d,%d,%d>",
        info.path,
        x, y, w, h,
        info.scaleX, info.scaleY,
        b2s(info.flipX), b2s(info.flipY),
        info.rotate,
        info.color.r, info.color.g, info.color.b, info.color.a
    );
}

Texture raylib_create_texture_from_image(ImgInfo info, char *tag)
{
    Texture *tex = Hashmap_get(resourcesMap, tag);
    if (tex != NULL) {
        return tex[0];
    }
    //
    int x = info.x;
    int y = info.y;
    int w = info.w;
    int h = info.h;
    int r = info.rotate;
    //
    Image img = raylib_load_image(info.path);
    int imgW = img.width;
    int imgH = img.height;
    x = MAX(0, MIN(x, imgW - 1));
    y = MAX(0, MIN(y, imgH - 1));
    int leftX = imgW - x;
    int leftY = imgH - y;
    w = MAX(1, MIN((w <= 0 ? imgW : w), leftX));
    h = MAX(1, MIN((h <= 0 ? imgH : h), leftY));
    img = ImageFromImage(img, (Rectangle){x, y, w, h});
    //
    if (info.scaleX >= 0 && info.scaleY >= 0) ImageResize(&img, w * info.scaleX, h * info.scaleY);
    if (info.flipX) ImageFlipHorizontal(&img);
    if (info.flipY) ImageFlipVertical(&img);
    if (r > 0)
    {
        while (r > 0) {
            ImageRotateCW(&img);
            r--;
        }
    }
    ImageColorTint(&img, info.color); 
    // // 
    Texture texture = LoadTextureFromImage(img);
    tex = (Texture *)malloc(sizeof(texture));
    tex[0] = texture;
    Hashmap_set(resourcesMap, tag, tex);
    UnloadImage(img);
    return texture;
}

typedef struct TxtInfo {
    char *path;
    char *text;
    int size;
    int spacing;
    Color color;
} TxtInfo;

char *get_tex4txt_tag(TxtInfo info)
{
    return tools_format(
        "<T-TEXT:%s:%s:%d:%d:%d,%d,%d,%d>",
        info.path,
        info.text,
        info.size,
        info.spacing,
        info.color.r, info.color.g, info.color.b, info.color.a
    );
}

Texture raylib_create_texture_from_text(TxtInfo info, char *tag)
{
    Texture *tex = Hashmap_get(resourcesMap, tag);
    if (tex != NULL) {
        return tex[0];
    }
    Font font = raylib_load_font(info.path);
    Image img = ImageTextEx(font, info.text, info.size, info.spacing, info.color);
    Texture texture = LoadTextureFromImage(img);
    tex = (Texture *)malloc(sizeof(texture));
    tex[0] = texture;
    Hashmap_set(resourcesMap, tag, tex);
    UnloadImage(img);
    return texture;
}

Texture ralib_get_texture_by_tag(char *tag)
{
    Texture *tex = Hashmap_get(resourcesMap, tag);
    if (tex == NULL) return defaultTexture;
    return tex[0];
}

void raylib_draw_texture_by_texture(
    Texture texture,
    int x, int y,
    float anchorX, float anchorY,
    Color color,
    int fromX, int fromY, int width, int height,
    float rotation,
    float scale
)
{
    //
    fromX = MAX(0, MIN(fromX, texture.width - 1));
    fromY = MAX(0, MIN(fromY, texture.height - 1));
    int leftX = texture.width - fromX;
    int leftY = texture.height - fromY;
    width = MAX(1, MIN((width <= 0 ? texture.width : width), leftX));
    height = MAX(1, MIN((height <= 0 ? texture.height : height), leftY));
    //
    Rectangle source = (Rectangle){fromX, fromY, width, height};
    float destW = width * scale;
    float destH = height * scale;
    float destX = x;
    float destY = y;
    Rectangle dest = (Rectangle){destX, destY, destW, destH};
    Vector2 origin = (Vector2){destW * anchorX, destH * anchorY};
    DrawTextureTiled(texture, source, dest, origin, rotation, scale, color);
}

// callback

void raylib_on_show()
{
    Bridge_startFunc(uyghurBridge, "doska_korsitish_qayturmisi");
    Bridge_call(uyghurBridge);
    
    // audio
    // RLAPI void InitAudioDevice(void);                                     // Initialize audio device and context
    // RLAPI void CloseAudioDevice(void);                                    // Close the audio device and context
    // RLAPI bool IsAudioDeviceReady(void);                                  // Check if audio device has been initialized successfully
    // RLAPI void SetMasterVolume(float volume);                             // Set master volume (listener)

    // sound
    // RLAPI Sound LoadSound(const char *fileName);                          // Load sound from file
    // RLAPI void UnloadSound(Sound sound);                                  // Unload sound
    // RLAPI void UpdateSound(Sound sound, const void *data, int sampleCount); // Update sound buffer with new data
    // RLAPI void PlaySound(Sound sound);                                    // Play a sound
    // RLAPI void StopSound(Sound sound);                                    // Stop playing a sound
    // RLAPI void PauseSound(Sound sound);                                   // Pause a sound
    // RLAPI void ResumeSound(Sound sound);                                  // Resume a paused sound
    // RLAPI bool IsSoundPlaying(Sound sound);                               // Check if a sound is currently playing
    // RLAPI void SetSoundVolume(Sound sound, float volume);                 // Set volume for a sound (1.0 is max level)

    // Music
    // RLAPI Music LoadMusicStream(const char *fileName);                    // Load music stream from file
    // RLAPI void UnloadMusicStream(Music music);                            // Unload music stream
    // RLAPI void UnloadMusicStream(Music music);                            // Unload music stream
    // RLAPI void PlayMusicStream(Music music);                              // Start music playing
    // RLAPI bool IsMusicStreamPlaying(Music music);                         // Check if music is playing
    // RLAPI void UpdateMusicStream(Music music);                            // Updates buffers for music streaming
    // RLAPI void StopMusicStream(Music music);                              // Stop music playing
    // RLAPI void PauseMusicStream(Music music);                             // Pause music playing
    // RLAPI void ResumeMusicStream(Music music);                            // Resume playing paused music
    // RLAPI void SeekMusicStream(Music music, float position);              // Seek music to a position (in seconds)
    // RLAPI void SetMusicVolume(Music music, float volume);                 // Set volume for music (1.0 is max level)
    // RLAPI float GetMusicTimeLength(Music music);                          // Get music time length (in seconds)
    // RLAPI float GetMusicTimePlayed(Music music);                          // Get current music time played (in seconds)

    // SetTraceLogLevel(LOG_NONE);
}
int test = 0;
void raylib_on_frame()
{
    Bridge_startFunc(uyghurBridge, "doska_sizish_qayturmisi");
    Bridge_call(uyghurBridge);
    //
    // Vector2 center = (Vector2){250, 250};
    // Color color = (Color){50, 100, 200, 255};
    // Rectangle rectangle = (Rectangle){250, 250, 200, 200};
    //
    DrawFPS(400, 450);


    // Image img = LoadImage("../resources/rose.png");             // Load image in CPU memory (RAM)
    // Image img = raylib_load_image("../resources/rose.png");
    // // // Image img = raylib_load_image("../resources/gift.png");
    // img = ImageFromImage(img, (Rectangle){0, 0, 300, 300});
    // int w = img.width;
    // int h = img.height;


    
    // Texture texture = LoadTextureFromImage(img);
    // DrawTextureEx(texture, (Vector2){100, 100}, 0, 1, WHITE);
    // DrawTextureRec(texture, (Rectangle){0, 150, 300, 150}, (Vector2){0, 0}, WHITE);
    // DrawRectangle(100, 100, 200, 200, GREEN); 
}

void raylib_on_focus()
{
    // RLAPI bool IsWindowFocused(void);
}

void raylib_on_resize()
{
    // RLAPI bool IsWindowResized(void); 
}

void raylib_on_drop()
{
    // RLAPI bool IsFileDropped(void);                                   // Check if a file has been dropped into window
    // RLAPI char **GetDroppedFiles(int *count);                         // Get dropped files names (memory should be freed)
    // RLAPI void ClearDroppedFiles(void); 

}

void raylib_on_hide()
{

}

void raylib_show_window(int width, int height, char *title, int mode)
{
    if (IsWindowReady()) return;
    if (width < 0) width = 500;
    if (height < 0) height = 500;
    if (strlen(title) == 0) title = "Uyghur Script!";
    if (mode < 0) mode = FLAG_WINDOW_RESIZABLE;
    SetConfigFlags(mode);
    InitWindow(width, height, title);
    defaultFont = GetFontDefault();
    defaultImage = GenImageGradientRadial(300, 300, 0, (Color){255, 255, 255, 50}, (Color){0, 0, 0, 50});
    defaultTexture = LoadTextureFromImage(defaultImage);
    raylib_on_show();
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);
        raylib_on_frame();
        EndDrawing();
    }
    CloseWindow();
    raylib_on_hide();
}

// api

void ug_board_show_window(Bridge *bridge)
{
    int w = Bridge_popNumber(bridge);
    int h = Bridge_popNumber(bridge);
    char *title = Bridge_popString(bridge);
    int mode = Bridge_popNumber(bridge);
    raylib_show_window(w, h, title, mode);
    Bridge_startResult(bridge);
    Bridge_return(bridge);
}

void ug_baord_hide_window(Bridge *bridge)
{
    if (IsWindowReady()) CloseWindow();
    Bridge_startResult(bridge);
    Bridge_return(bridge);
}

void ug_baord_is_fullscreen(Bridge *bridge)
{
    Bridge_startResult(bridge);
    Bridge_pushBoolean(bridge, IsWindowFullscreen());
    Bridge_return(bridge);
}

void ug_baord_is_hidden(Bridge *bridge)
{
    Bridge_startResult(bridge);
    Bridge_pushBoolean(bridge, IsWindowHidden());
    Bridge_return(bridge);
}

void ug_baord_is_minimized(Bridge *bridge)
{
    Bridge_startResult(bridge);
    Bridge_pushBoolean(bridge, IsWindowMinimized());
    Bridge_return(bridge);
}

void ug_baord_is_maximized(Bridge *bridge)
{
    Bridge_startResult(bridge);
    Bridge_pushBoolean(bridge, IsWindowMaximized());
    Bridge_return(bridge);
}

void ug_baord_toggle_fullscreen(Bridge *bridge)
{
    ToggleFullscreen();
    Bridge_startResult(bridge);
    Bridge_return(bridge);
}

void ug_baord_set_minimize(Bridge *bridge)
{
    MinimizeWindow();
    Bridge_startResult(bridge);
    Bridge_return(bridge);
}

void ug_baord_set_maximize(Bridge *bridge)
{
    MaximizeWindow();
    Bridge_startResult(bridge);
    Bridge_return(bridge);
}

void ug_baord_set_normalize(Bridge *bridge)
{
    RestoreWindow();
    Bridge_startResult(bridge);
    Bridge_return(bridge);
}

void ug_baord_set_title(Bridge *bridge)
{
    SetWindowTitle(Bridge_popString(bridge));
    Bridge_startResult(bridge);
    Bridge_return(bridge);
}

void ug_baord_set_icon(Bridge *bridge)
{
    char *path = Bridge_popString(bridge);
    Image image = LoadImage(path);
    SetWindowIcon(image);
    Bridge_startResult(bridge);
    Bridge_return(bridge);
}

void ug_baord_set_position(Bridge *bridge)
{
    int x = Bridge_popNumber(bridge);
    int y = Bridge_popNumber(bridge);
    SetWindowPosition(x, y); 
    Bridge_startResult(bridge);
    Bridge_return(bridge);
}

void ug_baord_get_position(Bridge *bridge)
{
    Vector2 vector2 = GetWindowPosition();
    Bridge_startResult(bridge);
    Bridge_pushNumber(bridge, vector2.x);
    Bridge_pushNumber(bridge, vector2.y);
    Bridge_return(bridge);
}

void ug_baord_set_size(Bridge *bridge)
{
    int w = Bridge_popNumber(bridge);
    int h = Bridge_popNumber(bridge);
    SetWindowSize(w, h); 
    Bridge_startResult(bridge);
    Bridge_return(bridge);
}

void ug_baord_get_size(Bridge *bridge)
{
    Bridge_startResult(bridge);
    Bridge_pushNumber(bridge, GetScreenWidth());
    Bridge_pushNumber(bridge, GetScreenHeight());
    Bridge_return(bridge);
}

void ug_baord_get_screen_size(Bridge *bridge)
{
    int w = GetScreenWidth();
    int h = GetScreenHeight();
    Bridge_startResult(bridge);
    Bridge_pushNumber(bridge, w);
    Bridge_pushNumber(bridge, h);
    Bridge_return(bridge);
}

void ug_baord_set_min_size(Bridge *bridge)
{
    int w = Bridge_popNumber(bridge);
    int h = Bridge_popNumber(bridge);
    SetWindowMinSize(w, h); 
    Bridge_startResult(bridge);
    Bridge_return(bridge);
}

void ug_baord_show_cursor(Bridge *bridge)
{
    bool b = Bridge_popBoolean(bridge);
    if (b) ShowCursor();
    if (!b) HideCursor();
    Bridge_startResult(bridge);
    Bridge_return(bridge);
}

void ug_baord_enable_cursor(Bridge *bridge)
{
    bool b = Bridge_popBoolean(bridge);
    if (b) EnableCursor();
    if (!b) DisableCursor();
    Bridge_startResult(bridge);
    Bridge_return(bridge);
}

void ug_baord_have_cursor(Bridge *bridge)
{
    Bridge_startResult(bridge);
    Bridge_pushBoolean(bridge, IsCursorOnScreen());
    Bridge_return(bridge);
} 

void ug_baord_set_fps(Bridge *bridge)
{
    SetTargetFPS(Bridge_popNumber(bridge));
    Bridge_startResult(bridge);
    Bridge_return(bridge);
}

void ug_baord_get_fps(Bridge *bridge)
{
    Bridge_startResult(bridge);
    Bridge_pushNumber(bridge, GetFPS());
    Bridge_return(bridge);
}

void ug_baord_draw_fps(Bridge *bridge)
{
    int x = Bridge_popNumber(bridge);
    int y = Bridge_popNumber(bridge);
    DrawFPS(x, y);
    Bridge_startResult(bridge);
    Bridge_return(bridge);
}

void ug_baord_set_clipboard(Bridge *bridge)
{
    char *c = Bridge_popString(bridge);
    SetClipboardText(c);
    Bridge_startResult(bridge);
    Bridge_return(bridge);
}

void ug_baord_get_clipboard(Bridge *bridge)
{
    char *r = (char*)GetClipboardText();
    Bridge_startResult(bridge);
    Bridge_pushString(bridge, r);
    Bridge_return(bridge);
}

void ug_baord_set_mourse_cursor(Bridge *bridge)
{
    SetMouseCursor(Bridge_popNumber(bridge));
    Bridge_startResult(bridge);
    Bridge_return(bridge);
}

void ug_baord_get_mouse_position(Bridge *bridge)
{
    Bridge_startResult(bridge);
    Bridge_pushNumber(bridge, GetMouseX());
    Bridge_pushNumber(bridge, GetMouseY());
    Bridge_return(bridge);
}

void ug_baord_get_mouse_wheel(Bridge *bridge)
{
    Bridge_startResult(bridge);
    Bridge_pushNumber(bridge, GetMouseWheelMove());
    Bridge_return(bridge);
}

void ug_baord_get_mouse_key_action(Bridge *bridge)
{
    int keyCode = Bridge_popNumber(bridge);
    int action = 0;
    if (IsMouseButtonPressed(keyCode)) action = 1;
    if (IsMouseButtonReleased(keyCode)) action = -1;
    Bridge_startResult(bridge);
    Bridge_pushNumber(bridge, action);
    Bridge_return(bridge);
}

void ug_baord_get_mouse_key_state(Bridge *bridge)
{
    int keyCode = Bridge_popNumber(bridge);
    int action = 0;
    if (IsMouseButtonDown(keyCode)) action = 1;
    if (IsMouseButtonUp(keyCode)) action = -1;
    Bridge_startResult(bridge);
    Bridge_pushNumber(bridge, action);
    Bridge_return(bridge);
}

void ug_baord_get_keyboard_key_action(Bridge *bridge)
{
    int keyCode = Bridge_popNumber(bridge);
    int action = 0;
    if (IsKeyPressed(keyCode)) action = 1;
    if (IsKeyReleased(keyCode)) action = -1;
    Bridge_startResult(bridge);
    Bridge_pushNumber(bridge, action);
    Bridge_return(bridge);
}

void ug_baord_get_keybaord_key_state(Bridge *bridge)
{
    int keyCode = Bridge_popNumber(bridge);
    int action = 0;
    if (IsKeyDown(keyCode)) action = 1;
    if (IsKeyUp(keyCode)) action = -1;
    Bridge_startResult(bridge);
    Bridge_pushNumber(bridge, action);
    Bridge_return(bridge);
}

void ug_baord_save_screenshot(Bridge *bridge)
{
    char *path = Bridge_popString(bridge);
    TakeScreenshot(path);
    Bridge_startResult(bridge);
    Bridge_return(bridge);
}

void ug_baord_draw_start(Bridge *bridge)
{
    BeginDrawing();
    Bridge_startResult(bridge);
    Bridge_return(bridge);
}

void ug_baord_draw_end(Bridge *bridge)
{
    EndDrawing();
    Bridge_startResult(bridge);
    Bridge_return(bridge);
}

void ug_baord_draw_pixel(Bridge *bridge)
{
    Vector2 poit = vector_from_bridge(bridge);
    Color color = color_from_bridge(bridge);
    DrawPixelV(poit, color);
    Bridge_startResult(bridge);
    Bridge_return(bridge);
}

void ug_baord_draw_line_no_controll(Bridge *bridge)
{
    Vector2 poit1 = vector_from_bridge(bridge);
    Vector2 poit2 = vector_from_bridge(bridge);
    int thickness = Bridge_popNumber(bridge);
    Color color = color_from_bridge(bridge);
    DrawLineEx(poit1, poit2, thickness, color);
    Bridge_startResult(bridge);
    Bridge_return(bridge);
}

void ug_baord_draw_line_one_controll(Bridge *bridge)
{
    Vector2 poit1 = vector_from_bridge(bridge);
    Vector2 poit2 = vector_from_bridge(bridge);
    int thickness = Bridge_popNumber(bridge);
    Color color = color_from_bridge(bridge);
    Vector2 controll1 = vector_from_bridge(bridge);
    DrawLineEx(poit1, poit2, thickness, color);
    DrawLineBezierQuad(poit1, poit2, controll1, thickness, color);
    Bridge_startResult(bridge);
    Bridge_return(bridge);
}

void ug_baord_draw_line_two_controll(Bridge *bridge)
{
    Vector2 poit1 = vector_from_bridge(bridge);
    Vector2 poit2 = vector_from_bridge(bridge);
    int thickness = Bridge_popNumber(bridge);
    Color color = color_from_bridge(bridge);
    Vector2 controll1 = vector_from_bridge(bridge);
    Vector2 controll2 = vector_from_bridge(bridge);
    DrawLineEx(poit1, poit2, thickness, color);
    DrawLineBezierCubic(poit1, poit2, controll1, controll1, thickness, color);
    Bridge_startResult(bridge);
    Bridge_return(bridge);
}

void ug_baord_draw_rectangle_fill_transformed(Bridge *bridge)
{
    Rectangle rectangle = rectangle_from_bridge(bridge);
    Vector2 anchor = vector_from_bridge(bridge);
    double rotation = Bridge_popNumber(bridge);
    Color color = color_from_bridge(bridge);
    DrawRectanglePro(rectangle, anchor, rotation, color);
    Bridge_startResult(bridge);
    Bridge_return(bridge);
}

void ug_baord_draw_rectangle_fill_colorful(Bridge *bridge)
{
    Rectangle rectangle = rectangle_from_bridge(bridge);
    int roundness = Bridge_popNumber(bridge);
    int thickness = Bridge_popNumber(bridge);
    Color leftTop = color_from_bridge(bridge);
    Color leftBottom = color_from_bridge(bridge);
    Color rightBottom = color_from_bridge(bridge);
    Color rightTop = color_from_bridge(bridge);
    DrawRectangleGradientEx(rectangle, leftTop, leftBottom, rightBottom, rightTop);
    Bridge_startResult(bridge);
    Bridge_return(bridge);
}

void ug_baord_draw_rectangle_fill_round(Bridge *bridge)
{
    Rectangle rectangle = rectangle_from_bridge(bridge);
    int roundness = Bridge_popNumber(bridge);
    Color color = color_from_bridge(bridge);
    DrawRectangleRounded(rectangle, roundness, 0, color);
    Bridge_startResult(bridge);
    Bridge_return(bridge);
}

void ug_baord_draw_rectangle_stroke(Bridge *bridge)
{
    Rectangle rectangle = rectangle_from_bridge(bridge);
    double roundness = Bridge_popNumber(bridge);
    double thickness = Bridge_popNumber(bridge);
    Color color = color_from_bridge(bridge);
    DrawRectangleRoundedLines(rectangle, roundness, 0, thickness, color);
    Bridge_startResult(bridge);
    Bridge_return(bridge);
}

void ug_baord_draw_circle_fill(Bridge *bridge)
{
    int centerX = Bridge_popNumber(bridge);
    int centerY = Bridge_popNumber(bridge);
    int radiusH = Bridge_popNumber(bridge);
    int radiusV = Bridge_popNumber(bridge);
    Color color = color_from_bridge(bridge);
    DrawEllipse(centerX, centerY, radiusH, radiusV, color);
    Bridge_startResult(bridge);
    Bridge_return(bridge);
}

void ug_baord_draw_circle_stroke(Bridge *bridge)
{
    int centerX = Bridge_popNumber(bridge);
    int centerY = Bridge_popNumber(bridge);
    int radiusH = Bridge_popNumber(bridge);
    int radiusV = Bridge_popNumber(bridge);
    Color color = color_from_bridge(bridge);
    DrawEllipseLines(centerX, centerY, radiusH, radiusV, color);
    Bridge_startResult(bridge);
    Bridge_return(bridge);
}

void ug_baord_draw_circle_gradient(Bridge *bridge)
{
    int centerX = Bridge_popNumber(bridge);
    int centerY = Bridge_popNumber(bridge);
    double radius = Bridge_popNumber(bridge);
    Color color1 = color_from_bridge(bridge);
    Color color2 = color_from_bridge(bridge);
    DrawCircleGradient(centerX, centerY, radius, color1, color2);
    Bridge_startResult(bridge);
    Bridge_return(bridge);
}

void ug_baord_draw_ring_fill(Bridge *bridge)
{
    Vector2 center = vector_from_bridge(bridge);
    double innerRadius = Bridge_popNumber(bridge);
    double outerRadius = Bridge_popNumber(bridge);
    Color color = color_from_bridge(bridge);
    double startAngle = Bridge_popNumber(bridge);
    double endAngle = Bridge_popNumber(bridge);
    DrawRing(center, innerRadius, outerRadius, startAngle, endAngle, 0, color);
    Bridge_startResult(bridge);
    Bridge_return(bridge);
}

void ug_baord_draw_ring_stroke(Bridge *bridge)
{
    Vector2 center = vector_from_bridge(bridge);
    double innerRadius = Bridge_popNumber(bridge);
    double outerRadius = Bridge_popNumber(bridge);
    Color color = color_from_bridge(bridge);
    double startAngle = Bridge_popNumber(bridge);
    double endAngle = Bridge_popNumber(bridge);
    DrawRingLines(center, innerRadius, outerRadius, startAngle, endAngle, 0, color);
    Bridge_startResult(bridge);
    Bridge_return(bridge);
}

void ug_baord_draw_triangle_fill(Bridge *bridge)
{
    Vector2 point1 = vector_from_bridge(bridge);
    Vector2 point2 = vector_from_bridge(bridge);
    Vector2 point3 = vector_from_bridge(bridge);
    Color color = color_from_bridge(bridge);
    DrawTriangle(point1, point2, point3, color);
    Bridge_startResult(bridge);
    Bridge_return(bridge);
}

void ug_baord_draw_triangle_stroke(Bridge *bridge)
{
    Vector2 point1 = vector_from_bridge(bridge);
    Vector2 point2 = vector_from_bridge(bridge);
    Vector2 point3 = vector_from_bridge(bridge);
    Color color = color_from_bridge(bridge);
    DrawTriangleLines(point1, point2, point3, color);
    Bridge_startResult(bridge);
    Bridge_return(bridge);
}

void ug_baord_draw_polygon_fill(Bridge *bridge)
{
    Vector2 center = vector_from_bridge(bridge);
    int sides = Bridge_popNumber(bridge);
    double radius = Bridge_popNumber(bridge);
    double rotation = Bridge_popNumber(bridge);
    Color color = color_from_bridge(bridge);
    DrawPoly(center, sides, radius, rotation, color);
    Bridge_startResult(bridge);
    Bridge_return(bridge);
}

void ug_baord_draw_polygon_stroke(Bridge *bridge)
{
    Vector2 center = vector_from_bridge(bridge);
    int sides = Bridge_popNumber(bridge);
    double radius = Bridge_popNumber(bridge);
    double rotation = Bridge_popNumber(bridge);
    double thickness = Bridge_popNumber(bridge);
    Color color = color_from_bridge(bridge);
    DrawPolyLinesEx(center, sides, radius, rotation, thickness, color);
    Bridge_startResult(bridge);
    Bridge_return(bridge);
}

// image

void ug_baord_create_texture_from_image(Bridge *bridge)
{
    ImgInfo info = (ImgInfo) {"../resources/rose.png", 100, 0, 200, 200, 2, 2, false, false, 0, (Color) {255, 255, 255, 255}};
    char *tag = get_tex4img_tag(info);
    Texture texture = raylib_create_texture_from_image(info, tag);
    // free(tag);
    Bridge_startResult(bridge);
    Bridge_pushString(bridge, tag);
    Bridge_return(bridge);
}

// text

// RLAPI Vector2 MeasureTextEx(Font font, const char *text, float fontSize, float spacing);
void ug_baord_create_texture_from_text(Bridge *bridge)
{
    TxtInfo info = (TxtInfo) {"../resources/ukij.ttf", "TEST", 36, 2, (Color) {255, 255, 255, 255}};
    char *tag = get_tex4txt_tag(info);
    Texture texture = raylib_create_texture_from_text(info, tag);
    // free(tag);
    Bridge_startResult(bridge);
    Bridge_pushString(bridge, tag);
    Bridge_return(bridge);
}

// texture

void ug_baord_draw_texture_by_tag(Bridge *bridge)
{
    char *tag = Bridge_popString(bridge);
    int x = Bridge_popNumber(bridge);
    int y = Bridge_popNumber(bridge);
    float anchorX = Bridge_popNumber(bridge);
    float anchorY = Bridge_popNumber(bridge);
    Color color = color_from_bridge(bridge);
    int fromX = Bridge_popNumber(bridge);
    int fromY = Bridge_popNumber(bridge);
    int width = Bridge_popNumber(bridge);
    int height = Bridge_popNumber(bridge);
    float rotation = Bridge_popNumber(bridge);
    float scale = Bridge_popNumber(bridge);
    //
    Texture texture = ralib_get_texture_by_tag(tag);
    raylib_draw_texture_by_texture(texture, x, y, anchorX, anchorY, color, fromX, fromY, width, height, rotation, scale);
    //
    Bridge_startResult(bridge);
    Bridge_return(bridge);
}

// other

void lib_raylib_register(Bridge *bridge)
{
    uyghurBridge = bridge;
    resourcesMap = Hashmap_new(NULL);
    //
    Bridge_startBox(bridge, "doska");
    //
    Bridge_pushKey(bridge, "korsitish");
    Bridge_pushFunction(bridge, ug_board_show_window);
    // 
    Bridge_pushKey(bridge, "chemberSizish");
    Bridge_pushFunction(bridge, ug_baord_draw_circle_fill);
    // 
    Bridge_pushKey(bridge, "resimEkirish");
    Bridge_pushFunction(bridge, ug_baord_create_texture_from_image);
    // 
    Bridge_pushKey(bridge, "xetEkirish");
    Bridge_pushFunction(bridge, ug_baord_create_texture_from_text);
    // 
    Bridge_pushKey(bridge, "tamghaBesish");
    Bridge_pushFunction(bridge, ug_baord_draw_texture_by_tag);
    //
    Bridge_register(bridge);
    //
    // raylib_show_window(-1, -1, "", 4096);
}
