// baord

#include "raylib.h"
#include "../uyghur/uyghur.c"

// tool

Color color_from_bridge(Bridge *bridge)
{
    char *str = Bridge_receiveString(bridge);
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
    float x = Bridge_receiveNumber(bridge);
    float y = Bridge_receiveNumber(bridge);
    return (Vector2){x, y};
}

Rectangle rectangle_from_bridge(Bridge *bridge)
{
    float x = Bridge_receiveNumber(bridge);
    float y = Bridge_receiveNumber(bridge);
    float w = Bridge_receiveNumber(bridge);
    float h = Bridge_receiveNumber(bridge);
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
    bool flipX;
    bool flipY;
} ImgInfo;

char *get_texture_tag_for_image(ImgInfo info)
{
    int x = info.x;
    int y = info.y;
    int w = info.w;
    int h = info.h;
    return tools_format(
        "<R-IMAGE:%s:%d,%d:%d,%d:%s,%s>",
        info.path,
        x, y, w, h,
        b2s(info.flipX), b2s(info.flipY)
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
    if (info.flipX) ImageFlipHorizontal(&img);
    if (info.flipY) ImageFlipVertical(&img);
    //
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
    float size;
    float spacing;
} TxtInfo;

char *get_texture_tag_for_text(TxtInfo info)
{
    return tools_format(
        "<R-TEXT:%s:%s:%d:%d>",
        info.path,
        info.text,
        info.size,
        info.spacing
    );
}

Texture raylib_create_texture_from_text(TxtInfo info, char *tag)
{
    Texture *tex = Hashmap_get(resourcesMap, tag);
    if (tex != NULL) {
        return tex[0];
    }
    Font font = raylib_load_font(info.path);
    Image img = ImageTextEx(font, info.text, info.size, info.spacing, WHITE);
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

// draw

void ug_baord_draw_start(Bridge *bridge)
{
    BeginDrawing();
    Bridge_returnEmpty(bridge);
}

void ug_baord_draw_end(Bridge *bridge)
{
    EndDrawing();
    Bridge_returnEmpty(bridge);
}

void ug_baord_draw_pixel(Bridge *bridge)
{
    Vector2 poit = vector_from_bridge(bridge);
    Color color = color_from_bridge(bridge);
    DrawPixelV(poit, color);
    Bridge_returnEmpty(bridge);
}

void ug_baord_draw_line_no_controll(Bridge *bridge)
{
    Vector2 poit1 = vector_from_bridge(bridge);
    Vector2 poit2 = vector_from_bridge(bridge);
    int thickness = Bridge_receiveNumber(bridge);
    Color color = color_from_bridge(bridge);
    DrawLineEx(poit1, poit2, thickness, color);
    Bridge_returnEmpty(bridge);
}

void ug_baord_draw_line_one_controll(Bridge *bridge)
{
    Vector2 poit1 = vector_from_bridge(bridge);
    Vector2 poit2 = vector_from_bridge(bridge);
    int thickness = Bridge_receiveNumber(bridge);
    Color color = color_from_bridge(bridge);
    Vector2 controll1 = vector_from_bridge(bridge);
    DrawLineEx(poit1, poit2, thickness, color);
    DrawLineBezierQuad(poit1, poit2, controll1, thickness, color);
    Bridge_returnEmpty(bridge);
}

void ug_baord_draw_line_two_controll(Bridge *bridge)
{
    Vector2 poit1 = vector_from_bridge(bridge);
    Vector2 poit2 = vector_from_bridge(bridge);
    int thickness = Bridge_receiveNumber(bridge);
    Color color = color_from_bridge(bridge);
    Vector2 controll1 = vector_from_bridge(bridge);
    Vector2 controll2 = vector_from_bridge(bridge);
    DrawLineEx(poit1, poit2, thickness, color);
    DrawLineBezierCubic(poit1, poit2, controll1, controll1, thickness, color);
    Bridge_returnEmpty(bridge);
}

void ug_baord_draw_rectangle_fill_transformed(Bridge *bridge)
{
    Rectangle rectangle = rectangle_from_bridge(bridge);
    Color color = color_from_bridge(bridge);
    Vector2 anchor = vector_from_bridge(bridge);
    anchor.x = anchor.x * rectangle.width;
    anchor.y = anchor.y * rectangle.height;
    float rotation = Bridge_receiveNumber(bridge);
    DrawRectanglePro(rectangle, anchor, rotation, color);
    Bridge_returnEmpty(bridge);
}

void ug_baord_draw_rectangle_fill_colorful(Bridge *bridge)
{
    Rectangle rectangle = rectangle_from_bridge(bridge);
    Color leftTop = color_from_bridge(bridge);
    Color leftBottom = color_from_bridge(bridge);
    Color rightBottom = color_from_bridge(bridge);
    Color rightTop = color_from_bridge(bridge);
    DrawRectangleGradientEx(rectangle, leftTop, leftBottom, rightBottom, rightTop);
    Bridge_returnEmpty(bridge);
}

void ug_baord_draw_rectangle_fill_round(Bridge *bridge)
{
    Rectangle rectangle = rectangle_from_bridge(bridge);
    Color color = color_from_bridge(bridge);
    int roundness = Bridge_receiveNumber(bridge);
    DrawRectangleRounded(rectangle, roundness, 0, color);
    Bridge_returnEmpty(bridge);
}

void ug_baord_draw_rectangle_stroke(Bridge *bridge)
{
    Rectangle rectangle = rectangle_from_bridge(bridge);
    Color color = color_from_bridge(bridge);
    double roundness = Bridge_receiveNumber(bridge);
    double thickness = Bridge_receiveNumber(bridge);
    DrawRectangleRoundedLines(rectangle, roundness, 0, thickness, color);
    Bridge_returnEmpty(bridge);
}

void ug_baord_draw_circle_fill(Bridge *bridge)
{
    int centerX = Bridge_receiveNumber(bridge);
    int centerY = Bridge_receiveNumber(bridge);
    int radiusH = Bridge_receiveNumber(bridge);
    int radiusV = Bridge_receiveNumber(bridge);
    Color color = color_from_bridge(bridge);
    DrawEllipse(centerX, centerY, radiusH, radiusV, color);
    Bridge_returnEmpty(bridge);
}

void ug_baord_draw_circle_stroke(Bridge *bridge)
{
    int centerX = Bridge_receiveNumber(bridge);
    int centerY = Bridge_receiveNumber(bridge);
    int radiusH = Bridge_receiveNumber(bridge);
    int radiusV = Bridge_receiveNumber(bridge);
    Color color = color_from_bridge(bridge);
    DrawEllipseLines(centerX, centerY, radiusH, radiusV, color);
    Bridge_returnEmpty(bridge);
}

void ug_baord_draw_ring_fill(Bridge *bridge)
{
    Vector2 center = vector_from_bridge(bridge);
    double innerRadius = Bridge_receiveNumber(bridge);
    double outerRadius = Bridge_receiveNumber(bridge);
    Color color = color_from_bridge(bridge);
    double startAngle = Bridge_receiveNumber(bridge);
    double endAngle = Bridge_receiveNumber(bridge);
    DrawRing(center, innerRadius, outerRadius, startAngle, endAngle, 0, color);
    Bridge_returnEmpty(bridge);
}

void ug_baord_draw_ring_stroke(Bridge *bridge)
{
    Vector2 center = vector_from_bridge(bridge);
    double innerRadius = Bridge_receiveNumber(bridge);
    double outerRadius = Bridge_receiveNumber(bridge);
    Color color = color_from_bridge(bridge);
    double startAngle = Bridge_receiveNumber(bridge);
    double endAngle = Bridge_receiveNumber(bridge);
    DrawRingLines(center, innerRadius, outerRadius, startAngle, endAngle, 0, color);
    Bridge_returnEmpty(bridge);
}

void ug_baord_draw_triangle_fill(Bridge *bridge)
{
    Vector2 point1 = vector_from_bridge(bridge);
    Vector2 point2 = vector_from_bridge(bridge);
    Vector2 point3 = vector_from_bridge(bridge);
    Color color = color_from_bridge(bridge);
    DrawTriangle(point1, point2, point3, color);
    Bridge_returnEmpty(bridge);
}

void ug_baord_draw_triangle_stroke(Bridge *bridge)
{
    Vector2 point1 = vector_from_bridge(bridge);
    Vector2 point2 = vector_from_bridge(bridge);
    Vector2 point3 = vector_from_bridge(bridge);
    Color color = color_from_bridge(bridge);
    DrawTriangleLines(point1, point2, point3, color);
    Bridge_returnEmpty(bridge);
}

void ug_baord_draw_polygon_fill(Bridge *bridge)
{
    Vector2 center = vector_from_bridge(bridge);
    int sides = Bridge_receiveNumber(bridge);
    double radius = Bridge_receiveNumber(bridge);
    double rotation = Bridge_receiveNumber(bridge);
    Color color = color_from_bridge(bridge);
    DrawPoly(center, sides, radius, rotation, color);
    Bridge_returnEmpty(bridge);
}

void ug_baord_draw_polygon_stroke(Bridge *bridge)
{
    Vector2 center = vector_from_bridge(bridge);
    int sides = Bridge_receiveNumber(bridge);
    double radius = Bridge_receiveNumber(bridge);
    double rotation = Bridge_receiveNumber(bridge);
    double thickness = Bridge_receiveNumber(bridge);
    Color color = color_from_bridge(bridge);
    DrawPolyLinesEx(center, sides, radius, rotation, thickness, color);
    Bridge_returnEmpty(bridge);
}

// text

void ug_baord_draw_text(Bridge *bridge)
{
    char *font = Bridge_receiveString(bridge);
    char *text = Bridge_receiveString(bridge);
    float size = Bridge_receiveNumber(bridge);
    float spacing = Bridge_receiveNumber(bridge);
    Color color = color_from_bridge(bridge);
    Vector2 position = vector_from_bridge(bridge);
    Font fnt = raylib_load_font(font);
    DrawTextEx(fnt, text, position, size, spacing, color);
    Bridge_returnEmpty(bridge);
}

void ug_baord_measure_text(Bridge *bridge)
{
    char *font = Bridge_receiveString(bridge);
    char *text = Bridge_receiveString(bridge);
    float size = Bridge_receiveNumber(bridge);
    float spacing = Bridge_receiveNumber(bridge);
    Font fnt = raylib_load_font(font);
    Vector2 space = MeasureTextEx(fnt, text, size, spacing);
    Bridge_returnNumber(bridge, space.x);
}

// image texture

void ug_baord_create_texture_from_image(Bridge *bridge)
{
    char *image = Bridge_receiveString(bridge);
    int x = Bridge_receiveNumber(bridge);
    int y = Bridge_receiveNumber(bridge);
    int w = Bridge_receiveNumber(bridge);
    int h = Bridge_receiveNumber(bridge);
    bool flipX = Bridge_receiveBoolean(bridge);
    bool flipY = Bridge_receiveBoolean(bridge);
    ImgInfo info = (ImgInfo) {image, x, y, w, h, flipX, flipY};
    char *tag = get_texture_tag_for_image(info);
    Texture texture = raylib_create_texture_from_image(info, tag);
    Bridge_returnString(bridge, tag);
    free(tag);
}

// text texture

void ug_baord_create_texture_from_text(Bridge *bridge)
{
    char *font = Bridge_receiveString(bridge);
    char *text = Bridge_receiveString(bridge);
    float size = Bridge_receiveNumber(bridge);
    float spacing = Bridge_receiveNumber(bridge);
    TxtInfo info = (TxtInfo) {font, text, size, spacing};
    char *tag = get_texture_tag_for_text(info);
    Texture texture = raylib_create_texture_from_text(info, tag);
    Bridge_returnString(bridge, tag);
    free(tag);
}

// texture

void ug_baord_draw_texture_by_tag(Bridge *bridge)
{
    char *tag = Bridge_receiveString(bridge);
    int x = Bridge_receiveNumber(bridge);
    int y = Bridge_receiveNumber(bridge);
    float anchorX = Bridge_receiveNumber(bridge);
    float anchorY = Bridge_receiveNumber(bridge);
    Color color = color_from_bridge(bridge);
    int fromX = Bridge_receiveNumber(bridge);
    int fromY = Bridge_receiveNumber(bridge);
    int width = Bridge_receiveNumber(bridge);
    int height = Bridge_receiveNumber(bridge);
    float rotation = Bridge_receiveNumber(bridge);
    float scale = Bridge_receiveNumber(bridge);
    //
    Texture texture = ralib_get_texture_by_tag(tag);
    raylib_draw_texture_by_texture(texture, x, y, anchorX, anchorY, color, fromX, fromY, width, height, rotation, scale);
    //
    Bridge_returnEmpty(bridge);
}

// other

void lib_raylib_painter_register(Bridge *bridge)
{
    //
    Bridge_startBox(bridge, "ressam");
    // draw
    Bridge_bindNative(bridge, "sizishniBashlash", ug_baord_draw_start);
    Bridge_bindNative(bridge, "sizishniTamamlash", ug_baord_draw_end);
    // draw point & line
    Bridge_bindNative(bridge, "nuqtaSizish", ug_baord_draw_pixel);
    Bridge_bindNative(bridge, "siziqSizish", ug_baord_draw_line_no_controll);
    Bridge_bindNative(bridge, "siziqSizishBirKontrolluq", ug_baord_draw_line_one_controll);
    Bridge_bindNative(bridge, "siziqSizishIkkiKontirolluq", ug_baord_draw_line_two_controll);
    // draw rectangle
    Bridge_bindNative(bridge, "rayunToldurushAylandurulghan", ug_baord_draw_rectangle_fill_transformed);
    Bridge_bindNative(bridge, "rayunToldurushRenggareng", ug_baord_draw_rectangle_fill_colorful);
    Bridge_bindNative(bridge, "rayunToldurushSiliqlanghan", ug_baord_draw_rectangle_fill_round);
    Bridge_bindNative(bridge, "rayunSizishSiliqlanghan", ug_baord_draw_rectangle_stroke);
    // draw circle & ring
    Bridge_bindNative(bridge, "chemberToldurush", ug_baord_draw_circle_fill);
    Bridge_bindNative(bridge, "chemberSizish", ug_baord_draw_circle_stroke);
    Bridge_bindNative(bridge, "uzukToldurush", ug_baord_draw_ring_fill);
    Bridge_bindNative(bridge, "uzukSizish", ug_baord_draw_ring_stroke);
    // draw triangle & polygon
    Bridge_bindNative(bridge, "uchTereplikToldurush", ug_baord_draw_triangle_fill);
    Bridge_bindNative(bridge, "uchTereplikSizish", ug_baord_draw_triangle_stroke);
    Bridge_bindNative(bridge, "kopTereplikToldurush", ug_baord_draw_polygon_fill);
    Bridge_bindNative(bridge, "kopTereplikSizish", ug_baord_draw_polygon_stroke);
    // font
    Bridge_bindNative(bridge, "xetYezish", ug_baord_draw_text);
    Bridge_bindNative(bridge, "xetMolcherlesh", ug_baord_measure_text);
    // texture
    Bridge_bindNative(bridge, "resimEkirish", ug_baord_create_texture_from_image);
    Bridge_bindNative(bridge, "xetEkirish", ug_baord_create_texture_from_text);
    Bridge_bindNative(bridge, "tamghaBesish", ug_baord_draw_texture_by_tag);
    //
    Bridge_register(bridge);
}
