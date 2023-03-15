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

bool32 WonderCard_Init(void)
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
    {.titleTextPal = 1, .bodyTextPal = 0, .tiles = sWonderNewsGfx1, .map = sWonderNewsTilemap1, .pal = sWonderNewsPal1},
    {.titleTextPal = 1, .bodyTextPal = 0, .tiles = sWonderNewsGfx2, .map = sWonderNewsTilemap2, .pal = sWonderCardBgPal2},
    {.titleTextPal = 1, .bodyTextPal = 0, .tiles = sWonderNewsGfx3, .map = sWonderNewsTilemap3, .pal = sWonderCardBgPal3},
    {.titleTextPal = 1, .bodyTextPal = 0, .tiles = sWonderNewsGfx3, .map = sWonderNewsTilemap3, .pal = sWonderCardBgPal4},
    {.titleTextPal = 1, .bodyTextPal = 0, .tiles = sWonderNewsGfx3, .map = sWonderNewsTilemap3, .pal = sWonderCardBgPal5},
    {.titleTextPal = 1, .bodyTextPal = 0, .tiles = sWonderNewsGfx3, .map = sWonderNewsTilemap3, .pal = sWonderCardBgPal6},
    {.titleTextPal = 1, .bodyTextPal = 0, .tiles = sWonderNewsGfx7, .map = sWonderNewsTilemap7, .pal = sWonderNewsPal7},
    {.titleTextPal = 1, .bodyTextPal = 0, .tiles = sWonderNewsGfx8, .map = sWonderNewsTilemap8, .pal = sWonderNewsPal8}
};

bool32 WonderNews_Init(void)
{

}

void WonderNews_Destroy(void)
{
    if (sWonderNewsData != NULL)
    {
        *sWonderNewsData = (struct WonderNewsData){};
        FREE_AND_SET_NULL(sWonderNewsData);
    }
}

s32 WonderNews_Enter(void)
{
    if (sWonderNewsData == NULL)
        return -1;

    switch (sWonderNewsData->enterExitState)
    {
    case 0:
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        break;
    case 1:
        if (UpdatePaletteFade())
            return 0;
        ChangeBgY(0, 0, BG_COORD_SET);
        ChangeBgY(1, 0, BG_COORD_SET);
        ChangeBgY(2, 0, BG_COORD_SET);
        ChangeBgY(3, 0, BG_COORD_SET);
        SetGpuReg(REG_OFFSET_WIN0H, WIN_RANGE(0, DISPLAY_WIDTH));
        SetGpuReg(REG_OFFSET_WIN0V, WIN_RANGE(26, 152));
        SetGpuReg(REG_OFFSET_WININ, WININ_WIN0_BG_ALL | WININ_WIN0_OBJ);
        SetGpuReg(REG_OFFSET_WINOUT, WINOUT_WIN01_BG0 | WINOUT_WIN01_BG1 | WINOUT_WIN01_BG3 | WINOUT_WIN01_OBJ);
        SetGpuRegBits(REG_OFFSET_DISPCNT, DISPCNT_WIN0_ON);
        break;
    case 2:
        FillBgTilemapBufferRect_Palette0(0, 0x000, 0, 0, DISPLAY_TILE_WIDTH, DISPLAY_TILE_HEIGHT);
        FillBgTilemapBufferRect_Palette0(1, 0x000, 0, 0, DISPLAY_TILE_WIDTH, DISPLAY_TILE_HEIGHT);
        FillBgTilemapBufferRect_Palette0(2, 0x000, 0, 0, DISPLAY_TILE_WIDTH, DISPLAY_TILE_HEIGHT);
        FillBgTilemapBufferRect_Palette0(3, 0x000, 0, 0, DISPLAY_TILE_WIDTH, DISPLAY_TILE_HEIGHT);
        CopyBgTilemapBufferToVram(0);
        CopyBgTilemapBufferToVram(1);
        CopyBgTilemapBufferToVram(2);
        CopyBgTilemapBufferToVram(3);
        DecompressAndCopyTileDataToVram(3, sWonderNewsData->gfx->tiles, 0, 8, 0);
        sWonderNewsData->windowIds[NEWS_WIN_TITLE] = AddWindow(&sNews_WindowTemplates[NEWS_WIN_TITLE]);
        sWonderNewsData->windowIds[NEWS_WIN_BODY] = AddWindow(&sNews_WindowTemplates[NEWS_WIN_BODY]);
        break;
    case 3:
        if (FreeTempTileDataBuffersIfPossible())
            return 0;
        LoadPalette(GetTextWindowPalette(1), BG_PLTT_ID(2), PLTT_SIZE_4BPP);
        gPaletteFade.bufferTransferDisabled = TRUE;
        LoadPalette(sWonderNewsData->gfx->pal, BG_PLTT_ID(1), PLTT_SIZE_4BPP);
        LZ77UnCompWram(sWonderNewsData->gfx->map, sWonderNewsData->bgTilemapBuffer);
        CopyRectToBgTilemapBufferRect(1, sWonderNewsData->bgTilemapBuffer, 0, 0, DISPLAY_TILE_WIDTH, 3, 0, 0, DISPLAY_TILE_WIDTH, 3, 1, 8, 0);
        CopyRectToBgTilemapBufferRect(3, sWonderNewsData->bgTilemapBuffer, 0, 3, DISPLAY_TILE_WIDTH, 3 + DISPLAY_TILE_HEIGHT, 0, 3, DISPLAY_TILE_WIDTH, 3 + DISPLAY_TILE_HEIGHT, 1, 8, 0);
        CopyBgTilemapBufferToVram(1);
        CopyBgTilemapBufferToVram(3);
        break;
    case 4:
        BufferNewsText();
        break;
    case 5:
        DrawNewsWindows();
        CopyBgTilemapBufferToVram(0);
        CopyBgTilemapBufferToVram(2);
        break;
    case 6:
        ShowBg(1);
        ShowBg(2);
        ShowBg(3);
        gPaletteFade.bufferTransferDisabled = FALSE;
        sWonderNewsData->arrowTaskId = AddScrollIndicatorArrowPair(&sWonderNewsData->arrowsTemplate, &sWonderNewsData->scrollOffset);
        BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
        UpdatePaletteFade();
        break;
    default:
        if (UpdatePaletteFade())
            return 0;
        sWonderNewsData->enterExitState = 0;
        return 1;
    }

    sWonderNewsData->enterExitState++;
    return 0;
}

s32 WonderNews_Exit(bool32 useCancel)
{
    if (sWonderNewsData == NULL)
        return -1;
    switch (sWonderNewsData->enterExitState)
    {
    case 0:
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        break;
    case 1:
        if (UpdatePaletteFade())
            return 0;
        ChangeBgY(2, 0, BG_COORD_SET);
        SetGpuReg(REG_OFFSET_WIN0H, 0);
        SetGpuReg(REG_OFFSET_WIN0V, 0);
        SetGpuReg(REG_OFFSET_WININ, 0);
        SetGpuReg(REG_OFFSET_WINOUT, 0);
        ClearGpuRegBits(REG_OFFSET_DISPCNT, DISPCNT_WIN0_ON);
        break;
    case 2:
        FillBgTilemapBufferRect_Palette0(0, 0x000, 0, 0, DISPLAY_TILE_WIDTH, DISPLAY_TILE_HEIGHT);
        FillBgTilemapBufferRect_Palette0(1, 0x000, 0, 0, DISPLAY_TILE_WIDTH, DISPLAY_TILE_HEIGHT);
        FillBgTilemapBufferRect_Palette0(2, 0x000, 0, 0, DISPLAY_TILE_WIDTH, DISPLAY_TILE_HEIGHT + 4);
        FillBgTilemapBufferRect_Palette0(3, 0x000, 0, 0, DISPLAY_TILE_WIDTH, DISPLAY_TILE_HEIGHT + 4);
        CopyBgTilemapBufferToVram(0);
        CopyBgTilemapBufferToVram(1);
        CopyBgTilemapBufferToVram(2);
        CopyBgTilemapBufferToVram(3);
        break;
    case 3:
        HideBg(1);
        HideBg(2);
        RemoveWindow(sWonderNewsData->windowIds[NEWS_WIN_BODY]);
        RemoveWindow(sWonderNewsData->windowIds[NEWS_WIN_TITLE]);
        break;
    case 4:
        ChangeBgY(2, 0, BG_COORD_SET);
        ChangeBgY(3, 0, BG_COORD_SET);
        if (sWonderNewsData->arrowTaskId != TASK_NONE)
        {
            RemoveScrollIndicatorArrowPair(sWonderNewsData->arrowTaskId);
            sWonderNewsData->arrowTaskId = TASK_NONE;
        }
        break;
    case 5:
        PrintMysteryGiftOrEReaderHeader(gGiftIsFromEReader, useCancel);
        MG_DrawCheckerboardPattern(3);
        CopyBgTilemapBufferToVram(0);
        CopyBgTilemapBufferToVram(3);
        BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
        break;
    default:
        if (UpdatePaletteFade())
            return 0;
        sWonderNewsData->enterExitState = 0;
        return 1;
    }
    sWonderNewsData->enterExitState++;
    return 0;
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
