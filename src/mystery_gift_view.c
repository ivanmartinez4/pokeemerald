#include "global.h"
#include "bg.h"
#include "gpu_regs.h"
#include "palette.h"
#include "decompress.h"
#include "malloc.h"
#include "menu.h"
#include "pokemon_icon.h"
#include "union_room.h"
#include "list_menu.h"
#include "text_window.h"
#include "string_util.h"
#include "link_rfu.h"
#include "mystery_gift.h"
#include "mystery_gift_menu.h"
#include "mystery_gift_view.h"
#include "constants/rgb.h"
#include "constants/mystery_gift.h"

struct WonderGraphics
{
    u8 titleTextPal:4;
    u8 bodyTextPal:4;
    u8 footerTextPal:4; // Card only
    u8 stampShadowPal:4; // Card only
    const u32 * tiles;
    const u32 * map;
    const u16 * pal;
};

//======================
//    Wonder Cards
//======================

enum {
    CARD_WIN_HEADER,
    CARD_WIN_BODY,
    CARD_WIN_FOOTER,
    CARD_WIN_COUNT
};

#define TAG_STAMP_SHADOW 0x8000

struct CardStatTextData
{
    u8 width;
    u8 statText[WONDER_CARD_TEXT_LENGTH + 1];
    u8 statNumberText[4];
};

struct WonderCardData
{
    /*0170*/ const struct WonderGraphics * gfx;
    /*0174*/ u8 enterExitState;
    /*0175*/ u8 statFooterWidth;
    /*0176*/ u16 windowIds[CARD_WIN_COUNT];
    /*017C*/ u8 monIconSpriteId;
    /*017D*/ u8 stampSpriteIds[MAX_STAMP_CARD_STAMPS][2]; // 2 sprites each, 1 for the shadow and 1 for the Pokémon
    /*018B*/ u8 titleText[WONDER_CARD_TEXT_LENGTH + 1];
    /*01B4*/ u8 subtitleText[WONDER_CARD_TEXT_LENGTH + 1];
    /*01DD*/ u8 idNumberText[7];
    /*01E4*/ u8 bodyText[WONDER_CARD_BODY_TEXT_LINES][WONDER_CARD_TEXT_LENGTH + 1];
    /*0288*/ u8 footerLine1Text[WONDER_CARD_TEXT_LENGTH + 1];
    /*02B1*/ u8 giftText[WONDER_CARD_TEXT_LENGTH + 1];
    /*02DC*/ struct CardStatTextData statTextData[8];
    /*045C*/ u8 bgTilemapBuffer[0x1000];
};

EWRAM_DATA static struct WonderCardData * sWonderCardData = NULL;

static void BufferCardText(void);
static void DrawCardWindow(u8 whichWindow);
static void CreateCardSprites(void);
static void DestroyCardSprites(void);

extern const struct OamData gOamData_AffineOff_ObjNormal_32x16;

static const u8 sCard_TextColorTable[][3] = {
    {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_DARK_GRAY, TEXT_COLOR_LIGHT_GRAY},
    {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_WHITE, TEXT_COLOR_DARK_GRAY}
};

static const u8 ALIGNED(4) sCard_FooterTextOffsets[CARD_TYPE_COUNT] =
{
    [CARD_TYPE_GIFT] = 7,
    [CARD_TYPE_STAMP] = 4,
    [CARD_TYPE_LINK_STAT] = 7
};

static const struct WindowTemplate sCard_WindowTemplates[] = {
    [CARD_WIN_HEADER] = {
        .bg = 1,
        .tilemapLeft = 1,
        .tilemapTop = 1,
        .width = 25,
        .height = 4,
        .paletteNum = 2,
        .baseBlock = 0x029c
    },
    [CARD_WIN_BODY] = {
        .bg = 1,
        .tilemapLeft = 1,
        .tilemapTop = 6,
        .width = 28,
        .height = 8,
        .paletteNum = 2,
        .baseBlock = 0x01bc
    },
    [CARD_WIN_FOOTER] = {
        .bg = 1,
        .tilemapLeft = 1,
        .tilemapTop = 14,
        .width = 28,
        .height = 5,
        .paletteNum = 2,
        .baseBlock = 0x0130
    }
};

static const u16 sWonderCardBgPal1[]     = INCBIN_U16("graphics/wonder_card/bg1.gbapal");
static const u16 sWonderCardBgPal2[]     = INCBIN_U16("graphics/wonder_card/bg2.gbapal");
static const u16 sWonderCardBgPal3[]     = INCBIN_U16("graphics/wonder_card/bg3.gbapal");
static const u16 sWonderCardBgPal4[]     = INCBIN_U16("graphics/wonder_card/bg4.gbapal");
static const u16 sWonderCardBgPal5[]     = INCBIN_U16("graphics/wonder_card/bg5.gbapal");
static const u16 sWonderCardBgPal6[]     = INCBIN_U16("graphics/wonder_card/bg6.gbapal");
static const u16 sWonderCardBgPal7[]     = INCBIN_U16("graphics/wonder_card/bg7.gbapal");
static const u16 sWonderCardBgPal8[]     = INCBIN_U16("graphics/wonder_card/bg8.gbapal");
static const u32 sWonderCardBgGfx1[]     = INCBIN_U32("graphics/wonder_card/bg1.4bpp.lz");
static const u32 sWonderCardBgTilemap1[] = INCBIN_U32("graphics/wonder_card/bg1.bin.lz");
static const u32 sWonderCardBgGfx2[]     = INCBIN_U32("graphics/wonder_card/bg2.4bpp.lz");
static const u32 sWonderCardBgTilemap2[] = INCBIN_U32("graphics/wonder_card/bg2.bin.lz");
static const u32 sWonderCardBgGfx3[]     = INCBIN_U32("graphics/wonder_card/bg3.4bpp.lz");
static const u32 sWonderCardBgTilemap3[] = INCBIN_U32("graphics/wonder_card/bg3.bin.lz");
static const u32 sWonderCardBgGfx7[]     = INCBIN_U32("graphics/wonder_card/bg7.4bpp.lz");
static const u32 sWonderCardBgTilemap7[] = INCBIN_U32("graphics/wonder_card/bg7.bin.lz");
static const u32 sWonderCardBgGfx8[]     = INCBIN_U32("graphics/wonder_card/bg8.4bpp.lz");
static const u32 sWonderCardBgTilemap8[] = INCBIN_U32("graphics/wonder_card/bg8.bin.lz");
static const u16 sStampShadowPal1[]      = INCBIN_U16("graphics/wonder_card/stamp_shadow_1.gbapal");
static const u16 sStampShadowPal2[]      = INCBIN_U16("graphics/wonder_card/stamp_shadow_2.gbapal");
static const u16 sStampShadowPal3[]      = INCBIN_U16("graphics/wonder_card/stamp_shadow_3.gbapal");
static const u16 sStampShadowPal4[]      = INCBIN_U16("graphics/wonder_card/stamp_shadow_4.gbapal");
static const u16 sStampShadowPal5[]      = INCBIN_U16("graphics/wonder_card/stamp_shadow_5.gbapal");
static const u16 sStampShadowPal6[]      = INCBIN_U16("graphics/wonder_card/stamp_shadow_6.gbapal");
static const u16 sStampShadowPal7[]      = INCBIN_U16("graphics/wonder_card/stamp_shadow_7.gbapal");
static const u16 sStampShadowPal8[]      = INCBIN_U16("graphics/wonder_card/stamp_shadow_8.gbapal");
static const u32 sStampShadowGfx[]       = INCBIN_U32("graphics/wonder_card/stamp_shadow.4bpp.lz");

static const struct CompressedSpriteSheet sSpriteSheet_StampShadow = {
    sStampShadowGfx, 0x100, TAG_STAMP_SHADOW
};

static const struct SpritePalette sSpritePalettes_StampShadow[] = {
    {sStampShadowPal1, TAG_STAMP_SHADOW},
    {sStampShadowPal2, TAG_STAMP_SHADOW},
    {sStampShadowPal3, TAG_STAMP_SHADOW},
    {sStampShadowPal4, TAG_STAMP_SHADOW},
    {sStampShadowPal5, TAG_STAMP_SHADOW},
    {sStampShadowPal6, TAG_STAMP_SHADOW},
    {sStampShadowPal7, TAG_STAMP_SHADOW},
    {sStampShadowPal8, TAG_STAMP_SHADOW}
};

static const struct SpriteTemplate sSpriteTemplate_StampShadow =
{
    .tileTag = TAG_STAMP_SHADOW,
    .paletteTag = TAG_STAMP_SHADOW,
    .oam = &gOamData_AffineOff_ObjNormal_32x16,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy
};

static const struct WonderGraphics sCardGraphics[NUM_WONDER_BGS] = {
    {.titleTextPal = 1, .bodyTextPal = 0, .footerTextPal = 0, .stampShadowPal = 0, .tiles = sWonderCardBgGfx1, .map = sWonderCardBgTilemap1, .pal = sWonderCardBgPal1},
    {.titleTextPal = 1, .bodyTextPal = 0, .footerTextPal = 0, .stampShadowPal = 1, .tiles = sWonderCardBgGfx2, .map = sWonderCardBgTilemap2, .pal = sWonderCardBgPal2},
    {.titleTextPal = 1, .bodyTextPal = 0, .footerTextPal = 0, .stampShadowPal = 2, .tiles = sWonderCardBgGfx3, .map = sWonderCardBgTilemap3, .pal = sWonderCardBgPal3},
    {.titleTextPal = 1, .bodyTextPal = 0, .footerTextPal = 0, .stampShadowPal = 3, .tiles = sWonderCardBgGfx3, .map = sWonderCardBgTilemap3, .pal = sWonderCardBgPal4},
    {.titleTextPal = 1, .bodyTextPal = 0, .footerTextPal = 0, .stampShadowPal = 4, .tiles = sWonderCardBgGfx3, .map = sWonderCardBgTilemap3, .pal = sWonderCardBgPal5},
    {.titleTextPal = 1, .bodyTextPal = 0, .footerTextPal = 0, .stampShadowPal = 5, .tiles = sWonderCardBgGfx3, .map = sWonderCardBgTilemap3, .pal = sWonderCardBgPal6},
    {.titleTextPal = 1, .bodyTextPal = 0, .footerTextPal = 0, .stampShadowPal = 6, .tiles = sWonderCardBgGfx7, .map = sWonderCardBgTilemap7, .pal = sWonderCardBgPal7},
    {.titleTextPal = 1, .bodyTextPal = 0, .footerTextPal = 0, .stampShadowPal = 7, .tiles = sWonderCardBgGfx8, .map = sWonderCardBgTilemap8, .pal = sWonderCardBgPal8}
};

bool32 WonderCard_Init(u8 unused)
{

}

void WonderCard_Destroy(void)
{

}

s32 WonderCard_Enter(void)
{

}

s32 WonderCard_Exit(bool32 useCancel)
{

}

static void BufferCardText(void)
{

}

static void DrawCardWindow(u8 whichWindow)
{

}

static void CreateCardSprites(void)
{

}

static void DestroyCardSprites(void)
{

}

//======================
//    Wonder News
//======================

enum {
    NEWS_WIN_TITLE,
    NEWS_WIN_BODY,
    NEWS_WIN_COUNT
};

#define TAG_ARROWS 0x1000

struct WonderNewsData
{
    /*01bc*/ const struct WonderGraphics * gfx;
    /*01c0*/ u8 arrowsRemoved:1;
             u8 enterExitState:7;
    /*01c1*/ u8 arrowTaskId;
    /*01c2*/ bool8 scrolling:1;
             u8 scrollIncrement:7;
    /*01c3*/ bool8 scrollingDown:1;
             u8 scrollTotal:7;
    /*01c4*/ u16 scrollEnd;
    /*01c6*/ u16 scrollOffset;
    /*01c8*/ u16 windowIds[NEWS_WIN_COUNT];
    /*01cc*/ u8 unused[2];
    /*01ce*/ u8 titleText[WONDER_NEWS_TEXT_LENGTH + 1];
    /*01f7*/ u8 bodyText[WONDER_NEWS_BODY_TEXT_LINES][WONDER_NEWS_TEXT_LENGTH + 1];
    /*0394*/ struct ScrollArrowsTemplate arrowsTemplate;
    /*03a4*/ u8 bgTilemapBuffer[0x1000];
};

EWRAM_DATA static struct WonderNewsData * sWonderNewsData = NULL;

static void BufferNewsText(void);
static void DrawNewsWindows(void);
static void UpdateNewsScroll(void);

static const u8 sNews_TextColorTable[][3] = {
    {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_DARK_GRAY, TEXT_COLOR_LIGHT_GRAY},
    {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_WHITE,     TEXT_COLOR_DARK_GRAY}
};

static const struct WindowTemplate sNews_WindowTemplates[] = {
    [NEWS_WIN_TITLE] = {
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 0,
        .width = 28,
        .height = 3,
        .paletteNum = 2,
        .baseBlock = 0x2AC
    },
    [NEWS_WIN_BODY] = {
        .bg = 2,
        .tilemapLeft = 1,
        .tilemapTop = 3,
        .width = 28,
        .height = DISPLAY_TILE_HEIGHT,
        .paletteNum = 2,
        .baseBlock = 0x07C
    }
};

static const struct ScrollArrowsTemplate sNews_ArrowsTemplate = {
    .firstArrowType = SCROLL_ARROW_UP,
    .firstX = 232,
    .firstY = 24,
    .secondArrowType = SCROLL_ARROW_DOWN,
    .secondX = 232,
    .secondY = 152,
    .fullyUpThreshold = 0,
    .fullyDownThreshold = 2,
    .tileTag = TAG_ARROWS,
    .palTag = TAG_ARROWS,
    .palNum = 0
};

static const u16 sWonderNewsPal1[]     = INCBIN_U16("graphics/wonder_news/bg1.gbapal");
static const u16 sWonderNewsPal7[]     = INCBIN_U16("graphics/wonder_news/bg7.gbapal");
static const u16 sWonderNewsPal8[]     = INCBIN_U16("graphics/wonder_news/bg8.gbapal");
static const u32 sWonderNewsGfx1[]     = INCBIN_U32("graphics/wonder_news/bg1.4bpp.lz");
static const u32 sWonderNewsTilemap1[] = INCBIN_U32("graphics/wonder_news/bg1.bin.lz");
static const u32 sWonderNewsGfx2[]     = INCBIN_U32("graphics/wonder_news/bg2.4bpp.lz");
static const u32 sWonderNewsTilemap2[] = INCBIN_U32("graphics/wonder_news/bg2.bin.lz");
static const u32 sWonderNewsGfx3[]     = INCBIN_U32("graphics/wonder_news/bg3.4bpp.lz");
static const u32 sWonderNewsTilemap3[] = INCBIN_U32("graphics/wonder_news/bg3.bin.lz");
static const u32 sWonderNewsGfx7[]     = INCBIN_U32("graphics/wonder_news/bg7.4bpp.lz");
static const u32 sWonderNewsTilemap7[] = INCBIN_U32("graphics/wonder_news/bg7.bin.lz");
static const u32 sWonderNewsGfx8[]     = INCBIN_U32("graphics/wonder_news/bg8.4bpp.lz");
static const u32 sWonderNewsTilemap8[] = INCBIN_U32("graphics/wonder_news/bg8.bin.lz");

static const struct WonderGraphics sNewsGraphics[NUM_WONDER_BGS] = {

};

bool32 WonderNews_Init(u8 unused)
{

}

void WonderNews_Destroy(void)
{

}

s32 WonderNews_Enter(void)
{

}

s32 WonderNews_Exit(bool32 useCancel)
{

}

void WonderNews_RemoveScrollIndicatorArrowPair(void)
{

}


void WonderNews_AddScrollIndicatorArrowPair(void)
{

}

u32 WonderNews_GetInput(u16 input)
{

}

static void BufferNewsText(void)
{

}

static void DrawNewsWindows(void)
{

}

static void UpdateNewsScroll(void)
{

}
