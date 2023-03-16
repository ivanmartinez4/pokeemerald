#include "global.h"
#include "malloc.h"
#include "battle.h"
#include "battle_setup.h"
#include "decoration.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "event_scripts.h"
#include "field_camera.h"
#include "field_effect.h"
#include "field_player_avatar.h"
#include "field_screen_effect.h"
#include "field_specials.h"
#include "field_weather.h"
#include "fieldmap.h"
#include "fldeff.h"
#include "fldeff_misc.h"
#include "international_string_util.h"
#include "item_menu.h"
#include "link.h"
#include "list_menu.h"
#include "main.h"
#include "map_name_popup.h"
#include "menu.h"
#include "menu_helpers.h"
#include "metatile_behavior.h"
#include "overworld.h"
#include "palette.h"
#include "script.h"
#include "secret_base.h"
#include "sound.h"
#include "string_util.h"
#include "strings.h"
#include "task.h"
#include "tv.h"
#include "window.h"
#include "constants/event_bg.h"
#include "constants/decorations.h"
#include "constants/event_objects.h"
#include "constants/field_specials.h"
#include "constants/items.h"
#include "constants/map_types.h"
#include "constants/metatile_behaviors.h"
#include "constants/metatile_labels.h"
#include "constants/moves.h"
#include "constants/secret_bases.h"
#include "constants/songs.h"
#include "constants/trainers.h"

// Values for registryStatus
enum {
    UNREGISTERED,
    REGISTERED,
    NEW, // Temporary, so new secret bases mixed at the same time don't overwrite each other
};

struct SecretBaseRegistryMenu
{
    struct ListMenuItem items[11];
    u8 names[11][32];
};

struct SecretBaseRecordMixer
{
    struct SecretBase *secretBases;
    u32 version;
    u32 language;
};

struct SecretBaseEntranceMetatiles
{
    u16 closedMetatileId;
    u16 openMetatileId;
};

static EWRAM_DATA u8 sCurSecretBaseId = 0;
static EWRAM_DATA bool8 sInFriendSecretBase = FALSE;
static EWRAM_DATA struct SecretBaseRegistryMenu *sRegistryMenu = NULL;

static void Task_ShowSecretBaseRegistryMenu(u8);
static void BuildRegistryMenuItems(u8);
static void RegistryMenu_OnCursorMove(s32, bool8, struct ListMenu *);
static void FinalizeRegistryMenu(u8);
static void AddRegistryMenuScrollArrows(u8);
static void HandleRegistryMenuInput(u8);
static void ShowRegistryMenuActions(u8);
static void HandleRegistryMenuActionsInput(u8);
static void ShowRegistryMenuDeleteConfirmation(u8);
static void ShowRegistryMenuDeleteYesNo(u8);
static void DeleteRegistry_Yes(u8);
static void DeleteRegistry_No(u8);
static void ReturnToMainRegistryMenu(u8);
static void GoToSecretBasePCRegisterMenu(u8);
static u8 GetSecretBaseOwnerType(u8);

static const struct SecretBaseEntranceMetatiles sSecretBaseEntranceMetatiles[] =
{
    {.closedMetatileId = METATILE_General_SecretBase_TreeLeft,  .openMetatileId = METATILE_General_SecretBase_VineLeft},
    {.closedMetatileId = METATILE_General_SecretBase_TreeRight, .openMetatileId = METATILE_General_SecretBase_VineRight},
    {.closedMetatileId = METATILE_General_RedCaveIndent,        .openMetatileId = METATILE_General_RedCaveOpen},
    {.closedMetatileId = METATILE_General_YellowCaveIndent,     .openMetatileId = METATILE_General_YellowCaveOpen},
    {.closedMetatileId = METATILE_General_BlueCaveIndent,       .openMetatileId = METATILE_General_BlueCaveOpen},
    {.closedMetatileId = METATILE_Fallarbor_BrownCaveIndent,    .openMetatileId = METATILE_Fallarbor_BrownCaveOpen},
    {.closedMetatileId = METATILE_Fortree_SecretBase_Shrub,     .openMetatileId = METATILE_Fortree_SecretBase_ShrubOpen},
};

// mapNum, warpId, x, y
// x, y positions are for when the player warps in for the first time (in front of the computer)
static const u8 sSecretBaseEntrancePositions[NUM_SECRET_BASE_GROUPS * 4] =
{
    [SECRET_BASE_RED_CAVE1]    = MAP_NUM(SECRET_BASE_RED_CAVE1),    0,  1,  3,
    [SECRET_BASE_RED_CAVE2]    = MAP_NUM(SECRET_BASE_RED_CAVE2),    0,  5,  9,
    [SECRET_BASE_RED_CAVE3]    = MAP_NUM(SECRET_BASE_RED_CAVE3),    0,  1,  3,
    [SECRET_BASE_RED_CAVE4]    = MAP_NUM(SECRET_BASE_RED_CAVE4),    0,  7, 13,
    [SECRET_BASE_BROWN_CAVE1]  = MAP_NUM(SECRET_BASE_BROWN_CAVE1),  0,  2,  3,
    [SECRET_BASE_BROWN_CAVE2]  = MAP_NUM(SECRET_BASE_BROWN_CAVE2),  0,  9,  2,
    [SECRET_BASE_BROWN_CAVE3]  = MAP_NUM(SECRET_BASE_BROWN_CAVE3),  0, 13,  4,
    [SECRET_BASE_BROWN_CAVE4]  = MAP_NUM(SECRET_BASE_BROWN_CAVE4),  0,  1,  2,
    [SECRET_BASE_BLUE_CAVE1]   = MAP_NUM(SECRET_BASE_BLUE_CAVE1),   0,  1,  3,
    [SECRET_BASE_BLUE_CAVE2]   = MAP_NUM(SECRET_BASE_BLUE_CAVE2),   0,  1,  2,
    [SECRET_BASE_BLUE_CAVE3]   = MAP_NUM(SECRET_BASE_BLUE_CAVE3),   0,  3, 15,
    [SECRET_BASE_BLUE_CAVE4]   = MAP_NUM(SECRET_BASE_BLUE_CAVE4),   0,  3, 14,
    [SECRET_BASE_YELLOW_CAVE1] = MAP_NUM(SECRET_BASE_YELLOW_CAVE1), 0,  9,  3,
    [SECRET_BASE_YELLOW_CAVE2] = MAP_NUM(SECRET_BASE_YELLOW_CAVE2), 0,  8,  7,
    [SECRET_BASE_YELLOW_CAVE3] = MAP_NUM(SECRET_BASE_YELLOW_CAVE3), 0,  3,  6,
    [SECRET_BASE_YELLOW_CAVE4] = MAP_NUM(SECRET_BASE_YELLOW_CAVE4), 0,  5,  9,
    [SECRET_BASE_TREE1]        = MAP_NUM(SECRET_BASE_TREE1),        0,  2,  3,
    [SECRET_BASE_TREE2]        = MAP_NUM(SECRET_BASE_TREE2),        0,  5,  6,
    [SECRET_BASE_TREE3]        = MAP_NUM(SECRET_BASE_TREE3),        0, 15,  3,
    [SECRET_BASE_TREE4]        = MAP_NUM(SECRET_BASE_TREE4),        0,  4, 10,
    [SECRET_BASE_SHRUB1]       = MAP_NUM(SECRET_BASE_SHRUB1),       0,  3,  3,
    [SECRET_BASE_SHRUB2]       = MAP_NUM(SECRET_BASE_SHRUB2),       0,  1,  2,
    [SECRET_BASE_SHRUB3]       = MAP_NUM(SECRET_BASE_SHRUB3),       0,  7,  8,
    [SECRET_BASE_SHRUB4]       = MAP_NUM(SECRET_BASE_SHRUB4),       0,  9,  6,
};

#define GET_BASE_MAP_NUM(group)    (sSecretBaseEntrancePositions[(group) + 0])
#define GET_BASE_WARP_ID(group)    (sSecretBaseEntrancePositions[(group) + 1])
#define GET_BASE_COMPUTER_X(group) (sSecretBaseEntrancePositions[(group) + 2])
#define GET_BASE_COMPUTER_Y(group) (sSecretBaseEntrancePositions[(group) + 3])

static const struct MenuAction sRegistryMenuActions[] =
{
    {
        .text = gText_DelRegist,
        .func = { .void_u8 = ShowRegistryMenuDeleteConfirmation },
    },
    {
        .text = gText_Cancel,
        .func = { .void_u8 = ReturnToMainRegistryMenu },
    },
};

static const struct YesNoFuncTable sDeleteRegistryYesNoFuncs =
{
    .yesFunc = DeleteRegistry_Yes,
    .noFunc = DeleteRegistry_No,
};

static const u16 sSecretBaseOwnerGfxIds[10] =
{
    // Male
    OBJ_EVENT_GFX_YOUNGSTER,
    OBJ_EVENT_GFX_BUG_CATCHER,
    OBJ_EVENT_GFX_RICH_BOY,
    OBJ_EVENT_GFX_CAMPER,
    OBJ_EVENT_GFX_MAN_3,
    // Female
    OBJ_EVENT_GFX_LASS,
    OBJ_EVENT_GFX_GIRL_3,
    OBJ_EVENT_GFX_WOMAN_2,
    OBJ_EVENT_GFX_PICNICKER,
    OBJ_EVENT_GFX_WOMAN_5,
};

static const struct WindowTemplate sRegistryWindowTemplates[] =
{
    {
        .bg = 0,
        .tilemapLeft = 18,
        .tilemapTop = 1,
        .width = 11,
        .height = 18,
        .paletteNum = 15,
        .baseBlock = 0x01,
    },
    {
        .bg = 0,
        .tilemapLeft = 2,
        .tilemapTop = 1,
        .width = 28,
        .height = 4,
        .paletteNum = 15,
        .baseBlock = 0xc7,
    }
};

static const struct ListMenuTemplate sRegistryListMenuTemplate =
{
    .items = NULL,
    .moveCursorFunc = RegistryMenu_OnCursorMove,
    .itemPrintFunc = NULL,
    .totalItems = 0,
    .maxShowed = 0,
    .windowId = 0,
    .header_X = 0,
    .item_X = 8,
    .cursor_X = 0,
    .upText_Y = 9,
    .cursorPal = 2,
    .fillValue = 1,
    .cursorShadowPal = 3,
    .lettersSpacing = 0,
    .itemVerticalPadding = 0,
    .scrollMultiple = LIST_NO_MULTIPLE_SCROLL,
    .fontId = FONT_NORMAL,
    .cursorKind = CURSOR_BLACK_ARROW,
};

static void ClearSecretBase(void)
{

}

void ClearSecretBases(void)
{

}

static void SetCurSecretBaseId(void)
{

}

void TrySetCurSecretBaseIndex(void)
{

}

void CheckPlayerHasSecretBase(void)
{

}

static u8 GetSecretBaseTypeInFrontOfPlayer_(void)
{

}

void GetSecretBaseTypeInFrontOfPlayer(void)
{
    gSpecialVar_0x8007 = GetSecretBaseTypeInFrontOfPlayer_();
}

static void FindMetatileIdMapCoords(s16 *x, s16 *y, u16 metatileId)
{
    s16 i, j;
    const struct MapLayout *mapLayout = gMapHeader.mapLayout;

    for (j = 0; j < mapLayout->height; j++)
    {
        for (i = 0; i < mapLayout->width; i++)
        {
            if ((mapLayout->map[j * mapLayout->width + i] & MAPGRID_METATILE_ID_MASK) == metatileId)
            {
                *x = i;
                *y = j;
                return;
            }
        }
    }
}

// Opens or closes the secret base entrance metatile in front of the player.
void ToggleSecretBaseEntranceMetatile(void)
{
    u16 i;
    s16 x, y;
    s16 metatileId;

    GetXYCoordsOneStepInFrontOfPlayer(&x, &y);
    metatileId = MapGridGetMetatileIdAt(x, y);

    // Look for entrance metatiles to open
    for (i = 0; i < ARRAY_COUNT(sSecretBaseEntranceMetatiles); i++)
    {
        if (sSecretBaseEntranceMetatiles[i].closedMetatileId == metatileId)
        {
            MapGridSetMetatileIdAt(x, y, sSecretBaseEntranceMetatiles[i].openMetatileId | MAPGRID_COLLISION_MASK);
            CurrentMapDrawMetatileAt(x, y);
            return;
        }
    }

    // Look for entrance metatiles to close
    for (i = 0; i < ARRAY_COUNT(sSecretBaseEntranceMetatiles); i++)
    {
        if (sSecretBaseEntranceMetatiles[i].openMetatileId == metatileId)
        {
            MapGridSetMetatileIdAt(x, y, sSecretBaseEntranceMetatiles[i].closedMetatileId | MAPGRID_COLLISION_MASK);
            CurrentMapDrawMetatileAt(x, y);
            return;
        }
    }
}

static u8 GetNameLength(const u8 *secretBaseOwnerName)
{
    u8 i;
    for (i = 0; i < PLAYER_NAME_LENGTH; i++)
    {
        if (secretBaseOwnerName[i] == EOS)
            return i;
    }

    return PLAYER_NAME_LENGTH;
}

void SetPlayerSecretBase(void)
{

}

// Set the 'open' entrance metatile for any occupied secret base on this map
void SetOccupiedSecretBaseEntranceMetatiles(struct MapEvents const *events)
{

}

static void SetSecretBaseWarpDestination(void)
{
    s8 secretBaseGroup = SECRET_BASE_ID_TO_GROUP(sCurSecretBaseId);
    SetWarpDestinationToMapWarp(MAP_GROUP(SECRET_BASE_RED_CAVE1), GET_BASE_MAP_NUM(secretBaseGroup), GET_BASE_WARP_ID(secretBaseGroup));
}

#define tState data[0]

static void Task_EnterSecretBase(u8 taskId)
{

}

#undef tState

void EnterSecretBase(void)
{
    CreateTask(Task_EnterSecretBase, 0);
    FadeScreen(FADE_TO_BLACK, 0);
    SetDynamicWarp(0, gSaveBlock1Ptr->location.mapGroup, gSaveBlock1Ptr->location.mapNum, WARP_ID_NONE);
}

bool8 SecretBaseMapPopupEnabled(void)
{
    if (gMapHeader.mapType == MAP_TYPE_SECRET_BASE && VarGet(VAR_INIT_SECRET_BASE) == 0)
        return FALSE;

    return TRUE;
}

static void EnterNewlyCreatedSecretBase_WaitFadeIn(u8 taskId)
{
    ObjectEventTurn(&gObjectEvents[gPlayerAvatar.objectEventId], DIR_NORTH);
    if (IsWeatherNotFadingIn() == TRUE)
    {
        ScriptContext_Enable();
        DestroyTask(taskId);
    }
}

static void EnterNewlyCreatedSecretBase_StartFadeIn(void)
{
    s16 x, y;

    LockPlayerFieldControls();
    HideMapNamePopUpWindow();
    FindMetatileIdMapCoords(&x, &y, METATILE_SecretBase_PC);
    x += MAP_OFFSET;
    y += MAP_OFFSET;
    MapGridSetMetatileIdAt(x, y, METATILE_SecretBase_PC | MAPGRID_COLLISION_MASK);
    CurrentMapDrawMetatileAt(x, y);
    FadeInFromBlack();
    CreateTask(EnterNewlyCreatedSecretBase_WaitFadeIn, 0);
}

static void Task_EnterNewlyCreatedSecretBase(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        s8 secretBaseGroup = SECRET_BASE_ID_TO_GROUP(sCurSecretBaseId);
        SetWarpDestination(
            gSaveBlock1Ptr->location.mapGroup,
            gSaveBlock1Ptr->location.mapNum,
            WARP_ID_NONE,
            GET_BASE_COMPUTER_X(secretBaseGroup),
            GET_BASE_COMPUTER_Y(secretBaseGroup));
        WarpIntoMap();
        gFieldCallback = EnterNewlyCreatedSecretBase_StartFadeIn;
        SetMainCallback2(CB2_LoadMap);
        DestroyTask(taskId);
    }
}

void EnterNewlyCreatedSecretBase(void)
{
    CreateTask(Task_EnterNewlyCreatedSecretBase, 0);
    FadeScreen(FADE_TO_BLACK, 0);
}

bool8 CurMapIsSecretBase(void)
{
    if (gSaveBlock1Ptr->location.mapGroup == MAP_GROUP(SECRET_BASE_RED_CAVE1)
     && (u8)gSaveBlock1Ptr->location.mapNum <= MAP_NUM(SECRET_BASE_SHRUB4))
        return TRUE;
    else
        return FALSE;
}

void InitSecretBaseAppearance(bool8 hidePC)
{

}

void InitSecretBaseDecorationSprites(void)
{

}

void HideSecretBaseDecorationSprites(void)
{
    u8 objectEventId;
    u16 flag;

    for (objectEventId = 0; objectEventId < gMapHeader.events->objectEventCount; objectEventId++)
    {
        flag = gMapHeader.events->objectEvents[objectEventId].flagId;
        if (flag >= FLAG_DECORATION_1 && flag <= FLAG_DECORATION_14)
        {
            RemoveObjectEventByLocalIdAndMap(
                gMapHeader.events->objectEvents[objectEventId].localId,
                gSaveBlock1Ptr->location.mapNum,
                gSaveBlock1Ptr->location.mapGroup);
            FlagSet(flag);
        }
    }
}

void SetSecretBaseOwnerGfxId(void)
{
    VarSet(VAR_OBJ_GFX_ID_F, sSecretBaseOwnerGfxIds[GetSecretBaseOwnerType(VarGet(VAR_CURRENT_SECRET_BASE))]);
}

void SetCurSecretBaseIdFromPosition(const struct MapPosition *position, const struct MapEvents *events)
{
    s16 i;
    for (i = 0; i < events->bgEventCount; i++)
    {
        if (events->bgEvents[i].kind == BG_EVENT_SECRET_BASE
          && position->x == events->bgEvents[i].x + MAP_OFFSET
          && position->y == events->bgEvents[i].y + MAP_OFFSET)
        {
            sCurSecretBaseId = events->bgEvents[i].bgUnion.secretBaseId;
            break;
        }
    }
}

void WarpIntoSecretBase(const struct MapPosition *position, const struct MapEvents *events)
{
    SetCurSecretBaseIdFromPosition(position, events);
    TrySetCurSecretBaseIndex();
    ScriptContext_SetupScript(SecretBase_EventScript_Enter);
}

bool8 TrySetCurSecretBase(void)
{
    SetCurSecretBaseId();
    TrySetCurSecretBaseIndex();
    if (gSpecialVar_Result == TRUE)
        return FALSE;

    return TRUE;
}

static void Task_WarpOutOfSecretBase(u8 taskId)
{
    switch (gTasks[taskId].data[0])
    {
    case 0:
        LockPlayerFieldControls();
        gTasks[taskId].data[0] = 1;
        break;
    case 1:
        if (!gPaletteFade.active)
            gTasks[taskId].data[0] = 2;
        break;
    case 2:
        SetWarpDestinationToDynamicWarp(WARP_ID_SECRET_BASE);
        WarpIntoMap();
        gFieldCallback = FieldCB_DefaultWarpExit;
        SetMainCallback2(CB2_LoadMap);
        UnlockPlayerFieldControls();
        DestroyTask(taskId);
        break;
    }
}

static void WarpOutOfSecretBase(void)
{
    CreateTask(Task_WarpOutOfSecretBase, 0);
    FadeScreen(FADE_TO_BLACK, 0);
}

void IsCurSecretBaseOwnedByAnotherPlayer(void)
{

}

static u8 *GetSecretBaseName(u8 *dest, u8 secretBaseIdx)
{

}

u8 *GetSecretBaseMapName(u8 *dest)
{

}

void CopyCurSecretBaseOwnerName_StrVar1(void)
{

}

static bool8 IsSecretBaseRegistered(u8 secretBaseIdx)
{

}

static u8 GetAverageEVs(struct Pokemon *pokemon)
{
    u16 evTotal;
    evTotal  = GetMonData(pokemon, MON_DATA_HP_EV);
    evTotal += GetMonData(pokemon, MON_DATA_ATK_EV);
    evTotal += GetMonData(pokemon, MON_DATA_DEF_EV);
    evTotal += GetMonData(pokemon, MON_DATA_SPEED_EV);
    evTotal += GetMonData(pokemon, MON_DATA_SPATK_EV);
    evTotal += GetMonData(pokemon, MON_DATA_SPDEF_EV);
    return evTotal / 6;
}

void SetPlayerSecretBaseParty(void)
{

}

void ClearAndLeaveSecretBase(void)
{

}

void MoveOutOfSecretBase(void)
{
    IncrementGameStat(GAME_STAT_MOVED_SECRET_BASE);
    ClearAndLeaveSecretBase();
}

static void ClosePlayerSecretBaseEntrance(void)
{

}

// When the player moves to a new secret base by interacting with a new secret base
// entrance in the overworld.
void MoveOutOfSecretBaseFromOutside(void)
{

}

static u8 GetNumRegisteredSecretBases(void)
{
    s16 i;
    u8 count = 0;
    for (i = 1; i < SECRET_BASES_COUNT; i++)
    {
        if (IsSecretBaseRegistered(i) == TRUE)
            count++;
    }

    return count;
}

void GetCurSecretBaseRegistrationValidity(void)
{
    if (IsSecretBaseRegistered(VarGet(VAR_CURRENT_SECRET_BASE)) == TRUE)
        gSpecialVar_Result = 1;
    else if (GetNumRegisteredSecretBases() >= 10)
        gSpecialVar_Result = 2;
    else
        gSpecialVar_Result = 0;
}

void ToggleCurSecretBaseRegistry(void)
{

}

void ShowSecretBaseDecorationMenu(void)
{
    CreateTask(DoSecretBaseDecorationMenu, 0);
}

void ShowSecretBaseRegistryMenu(void)
{
    CreateTask(Task_ShowSecretBaseRegistryMenu, 0);
}

#define tNumBases       data[0]
#define tSelectedRow    data[1]
#define tScrollOffset   data[2]
#define tMaxShownItems  data[3]
#define tSelectedBaseId data[4]
#define tListTaskId     data[5]
#define tMainWindowId   data[6]
#define tActionWindowId data[7]
#define tArrowTaskId    data[8]

static void Task_ShowSecretBaseRegistryMenu(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    LockPlayerFieldControls();
    tNumBases = GetNumRegisteredSecretBases();
    if (tNumBases != 0)
    {
        tSelectedRow = 0;
        tScrollOffset = 0;
        ClearDialogWindowAndFrame(0, FALSE);
        sRegistryMenu = AllocZeroed(sizeof(*sRegistryMenu));
        tMainWindowId = AddWindow(&sRegistryWindowTemplates[0]);
        BuildRegistryMenuItems(taskId);
        FinalizeRegistryMenu(taskId);
        gTasks[taskId].func = HandleRegistryMenuInput;
    }
    else
    {
        DisplayItemMessageOnField(taskId, gText_NoRegistry, GoToSecretBasePCRegisterMenu);
    }
}

static void BuildRegistryMenuItems(u8 taskId)
{
    s16 *data;
    u8 i;
    u8 count;

    data = gTasks[taskId].data;
    count = 0;
    for (i = 1; i < SECRET_BASES_COUNT; i++)
    {
        if (IsSecretBaseRegistered(i))
        {
            GetSecretBaseName(sRegistryMenu->names[count], i);
            sRegistryMenu->items[count].name = sRegistryMenu->names[count];
            sRegistryMenu->items[count].id = i;
            count++;
        }
    }

    sRegistryMenu->items[count].name = gText_Cancel;
    sRegistryMenu->items[count].id = LIST_CANCEL;
    tNumBases = count + 1;
    if (tNumBases < 8)
        tMaxShownItems = tNumBases;
    else
        tMaxShownItems = 8;

    gMultiuseListMenuTemplate = sRegistryListMenuTemplate;
    gMultiuseListMenuTemplate.windowId = tMainWindowId;
    gMultiuseListMenuTemplate.totalItems = tNumBases;
    gMultiuseListMenuTemplate.items = sRegistryMenu->items;
    gMultiuseListMenuTemplate.maxShowed = tMaxShownItems;
}

static void RegistryMenu_OnCursorMove(s32 unused, bool8 flag, struct ListMenu *menu)
{
    if (flag != TRUE)
        PlaySE(SE_SELECT);
}

static void FinalizeRegistryMenu(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    SetStandardWindowBorderStyle(tMainWindowId, FALSE);
    tListTaskId = ListMenuInit(&gMultiuseListMenuTemplate, tScrollOffset, tSelectedRow);
    AddRegistryMenuScrollArrows(taskId);
    ScheduleBgCopyTilemapToVram(0);
}

static void AddRegistryMenuScrollArrows(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    tArrowTaskId = AddScrollIndicatorArrowPairParameterized(SCROLL_ARROW_UP, 188, 12, 148, tNumBases - tMaxShownItems, 0x13f8, 0x13f8, &tScrollOffset);
}

static void HandleRegistryMenuInput(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    s32 input = ListMenu_ProcessInput(tListTaskId);
    ListMenuGetScrollAndRow(tListTaskId, &tScrollOffset, &tSelectedRow);

    switch (input)
    {
    case LIST_NOTHING_CHOSEN:
        break;
    case LIST_CANCEL:
        PlaySE(SE_SELECT);
        DestroyListMenuTask(tListTaskId, NULL, NULL);
        RemoveScrollIndicatorArrowPair(tArrowTaskId);
        ClearStdWindowAndFrame(tMainWindowId, FALSE);
        ClearWindowTilemap(tMainWindowId);
        RemoveWindow(tMainWindowId);
        ScheduleBgCopyTilemapToVram(0);
        Free(sRegistryMenu);
        GoToSecretBasePCRegisterMenu(taskId);
        break;
    default:
        PlaySE(SE_SELECT);
        tSelectedBaseId = input;
        ShowRegistryMenuActions(taskId);
        break;
    }
}

static void ShowRegistryMenuActions(u8 taskId)
{
    struct WindowTemplate template;
    s16 *data = gTasks[taskId].data;
    RemoveScrollIndicatorArrowPair(tArrowTaskId);
    template = sRegistryWindowTemplates[1];
    template.width = GetMaxWidthInMenuTable(sRegistryMenuActions, 2);
    tActionWindowId = AddWindow(&template);
    SetStandardWindowBorderStyle(tActionWindowId, FALSE);
    PrintMenuTable(tActionWindowId, ARRAY_COUNT(sRegistryMenuActions), sRegistryMenuActions);
    InitMenuInUpperLeftCornerNormal(tActionWindowId, ARRAY_COUNT(sRegistryMenuActions), 0);
    ScheduleBgCopyTilemapToVram(0);
    gTasks[taskId].func = HandleRegistryMenuActionsInput;
}

static void HandleRegistryMenuActionsInput(u8 taskId)
{
    s8 input = Menu_ProcessInputNoWrap();
    switch (input)
    {
    case MENU_B_PRESSED:
        PlaySE(SE_SELECT);
        ReturnToMainRegistryMenu(taskId);
        break;
    case MENU_NOTHING_CHOSEN:
        break;
    default:
        PlaySE(SE_SELECT);
        sRegistryMenuActions[input].func.void_u8(taskId);
        break;
    }
}

static void ShowRegistryMenuDeleteConfirmation(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    ClearStdWindowAndFrame(tMainWindowId, FALSE);
    ClearStdWindowAndFrame(tActionWindowId, FALSE);
    ClearWindowTilemap(tMainWindowId);
    ClearWindowTilemap(tActionWindowId);
    RemoveWindow(tActionWindowId);
    ScheduleBgCopyTilemapToVram(0);
    GetSecretBaseName(gStringVar1, tSelectedBaseId);
    StringExpandPlaceholders(gStringVar4, gText_OkayToDeleteFromRegistry);
    DisplayItemMessageOnField(taskId, gStringVar4, ShowRegistryMenuDeleteYesNo);
}

static void ShowRegistryMenuDeleteYesNo(u8 taskId)
{
    DisplayYesNoMenuDefaultYes();
    DoYesNoFuncWithChoice(taskId, &sDeleteRegistryYesNoFuncs);
}

void DeleteRegistry_Yes_Callback(u8 taskId)
{

}

static void DeleteRegistry_Yes(u8 taskId)
{
    DisplayItemMessageOnField(taskId, gText_RegisteredDataDeleted, DeleteRegistry_Yes_Callback);
}

static void DeleteRegistry_No(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    ClearDialogWindowAndFrame(0, FALSE);
    DestroyListMenuTask(tListTaskId, &tScrollOffset, &tSelectedRow);
    FinalizeRegistryMenu(taskId);
    gTasks[taskId].func = HandleRegistryMenuInput;
}

static void ReturnToMainRegistryMenu(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    AddRegistryMenuScrollArrows(taskId);
    ClearStdWindowAndFrame(tActionWindowId, FALSE);
    ClearWindowTilemap(tActionWindowId);
    RemoveWindow(tActionWindowId);
    ScheduleBgCopyTilemapToVram(0);
    gTasks[taskId].func = HandleRegistryMenuInput;
}

static void GoToSecretBasePCRegisterMenu(u8 taskId)
{
    if (VarGet(VAR_CURRENT_SECRET_BASE) == 0)
        ScriptContext_SetupScript(SecretBase_EventScript_PCCancel);
    else
        ScriptContext_SetupScript(SecretBase_EventScript_ShowRegisterMenu);

    DestroyTask(taskId);
}

#undef tNumBases
#undef tSelectedRow
#undef tScrollOffset
#undef tMaxShownItems
#undef tSelectedBaseId
#undef tListTaskId
#undef tMainWindowId
#undef tActionWindowId
#undef tArrowTaskId

static u8 GetSecretBaseOwnerType(u8 secretBaseIdx)
{

}

const u8 *GetSecretBaseTrainerLoseText(void)
{
    u8 ownerType = GetSecretBaseOwnerType(VarGet(VAR_CURRENT_SECRET_BASE));
    if (ownerType == 0)
        return SecretBase_Text_Trainer0Defeated;
    else if (ownerType == 1)
        return SecretBase_Text_Trainer1Defeated;
    else if (ownerType == 2)
        return SecretBase_Text_Trainer2Defeated;
    else if (ownerType == 3)
        return SecretBase_Text_Trainer3Defeated;
    else if (ownerType == 4)
        return SecretBase_Text_Trainer4Defeated;
    else if (ownerType == 5)
        return SecretBase_Text_Trainer5Defeated;
    else if (ownerType == 6)
        return SecretBase_Text_Trainer6Defeated;
    else if (ownerType == 7)
        return SecretBase_Text_Trainer7Defeated;
    else if (ownerType == 8)
        return SecretBase_Text_Trainer8Defeated;
    else
        return SecretBase_Text_Trainer9Defeated;
}

void PrepSecretBaseBattleFlags(void)
{
    TryGainNewFanFromCounter(FANCOUNTER_BATTLED_AT_BASE);
    gTrainerBattleOpponent_A = TRAINER_SECRET_BASE;
    gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_SECRET_BASE;
}

void SetBattledOwnerFromResult(void)
{

}

void GetSecretBaseOwnerAndState(void)
{

}

#define tStepCb  data[0] // See Task_RunPerStepCallback
#define tState   data[1]
#define tPlayerX data[2]
#define tPlayerY data[3]
#define tFldEff  data[4]

void SecretBasePerStepCallback(u8 taskId)
{
    s16 x;
    s16 y;
    u8 behavior;
    u16 tileId;
    s16 *data;

    data = gTasks[taskId].data;
    switch (tState)
    {
    case 0:
        if (VarGet(VAR_CURRENT_SECRET_BASE))
            sInFriendSecretBase = TRUE;
        else
            sInFriendSecretBase = FALSE;

        PlayerGetDestCoords(&tPlayerX, &tPlayerY);
        tState = 1;
        break;
    case 1:
        // End if player hasn't moved
        PlayerGetDestCoords(&x, &y);
        if (x == tPlayerX && y == tPlayerY)
            return;

        tPlayerX = x;
        tPlayerY = y;
        VarSet(VAR_SECRET_BASE_STEP_COUNTER, VarGet(VAR_SECRET_BASE_STEP_COUNTER) + 1);
        behavior = MapGridGetMetatileBehaviorAt(x, y);
        tileId = MapGridGetMetatileIdAt(x, y);
        if (tileId == METATILE_SecretBase_SolidBoard_Top || tileId == METATILE_SecretBase_SolidBoard_Bottom)
        {
            if (sInFriendSecretBase == TRUE)
                VarSet(VAR_SECRET_BASE_HIGH_TV_FLAGS, VarGet(VAR_SECRET_BASE_HIGH_TV_FLAGS) | SECRET_BASE_USED_SOLID_BOARD);
        }
        else if (tileId == METATILE_SecretBase_SmallChair
              || tileId == METATILE_SecretBase_PokemonChair
              || tileId == METATILE_SecretBase_HeavyChair
              || tileId == METATILE_SecretBase_PrettyChair
              || tileId == METATILE_SecretBase_ComfortChair
              || tileId == METATILE_SecretBase_RaggedChair
              || tileId == METATILE_SecretBase_BrickChair
              || tileId == METATILE_SecretBase_CampChair
              || tileId == METATILE_SecretBase_HardChair)
        {
            if (sInFriendSecretBase == TRUE)
                VarSet(VAR_SECRET_BASE_LOW_TV_FLAGS, VarGet(VAR_SECRET_BASE_LOW_TV_FLAGS) | SECRET_BASE_USED_CHAIR);
        }
        else if (tileId == METATILE_SecretBase_RedTent_DoorTop
              || tileId == METATILE_SecretBase_RedTent_Door
              || tileId == METATILE_SecretBase_BlueTent_DoorTop
              || tileId == METATILE_SecretBase_BlueTent_Door)
        {
            if (sInFriendSecretBase == TRUE)
                VarSet(VAR_SECRET_BASE_LOW_TV_FLAGS, VarGet(VAR_SECRET_BASE_LOW_TV_FLAGS) | SECRET_BASE_USED_TENT);
        }
        else if ((behavior == MB_IMPASSABLE_NORTHEAST && tileId == METATILE_SecretBase_Stand_CornerRight)
              || (behavior == MB_IMPASSABLE_NORTHWEST && MapGridGetMetatileIdAt(x, y) == METATILE_SecretBase_Stand_CornerLeft))
        {
            if (sInFriendSecretBase == TRUE)
                VarSet(VAR_SECRET_BASE_HIGH_TV_FLAGS, VarGet(VAR_SECRET_BASE_HIGH_TV_FLAGS) | SECRET_BASE_USED_STAND);
        }
        else if (behavior == MB_IMPASSABLE_WEST_AND_EAST && tileId == METATILE_SecretBase_Slide_StairLanding)
        {
            if (sInFriendSecretBase == TRUE)
            {
                VarSet(VAR_SECRET_BASE_HIGH_TV_FLAGS, VarGet(VAR_SECRET_BASE_HIGH_TV_FLAGS) ^ SECRET_BASE_USED_SLIDE);
                VarSet(VAR_SECRET_BASE_HIGH_TV_FLAGS, VarGet(VAR_SECRET_BASE_HIGH_TV_FLAGS) | SECRET_BASE_DECLINED_SLIDE);
            }
        }
        else if (behavior == MB_SLIDE_SOUTH && tileId == METATILE_SecretBase_Slide_SlideTop)
        {
            if (sInFriendSecretBase == TRUE)
            {
                VarSet(VAR_SECRET_BASE_HIGH_TV_FLAGS, VarGet(VAR_SECRET_BASE_HIGH_TV_FLAGS) | SECRET_BASE_USED_SLIDE);
                VarSet(VAR_SECRET_BASE_HIGH_TV_FLAGS, VarGet(VAR_SECRET_BASE_HIGH_TV_FLAGS) ^ SECRET_BASE_DECLINED_SLIDE);
            }
        }
        else if (MetatileBehavior_IsSecretBaseGlitterMat(behavior) == TRUE)
        {
            if (sInFriendSecretBase == TRUE)
                VarSet(VAR_SECRET_BASE_HIGH_TV_FLAGS, VarGet(VAR_SECRET_BASE_HIGH_TV_FLAGS) | SECRET_BASE_USED_GLITTER_MAT);
        }
        else if (MetatileBehavior_IsSecretBaseBalloon(behavior) == TRUE)
        {
            PopSecretBaseBalloon(MapGridGetMetatileIdAt(x, y), x, y);
            if (sInFriendSecretBase == TRUE)
            {
                switch ((int)MapGridGetMetatileIdAt(x, y))
                {
                case METATILE_SecretBase_RedBalloon:
                case METATILE_SecretBase_BlueBalloon:
                case METATILE_SecretBase_YellowBalloon:
                    VarSet(VAR_SECRET_BASE_LOW_TV_FLAGS, VarGet(VAR_SECRET_BASE_LOW_TV_FLAGS) | SECRET_BASE_USED_BALLOON);
                    break;
                case METATILE_SecretBase_MudBall:
                    VarSet(VAR_SECRET_BASE_LOW_TV_FLAGS, VarGet(VAR_SECRET_BASE_LOW_TV_FLAGS) | SECRET_BASE_USED_MUD_BALL);
                    break;
                }
            }
        }
        else if (MetatileBehavior_IsSecretBaseBreakableDoor(behavior) == TRUE)
        {
            if (sInFriendSecretBase == TRUE)
                VarSet(VAR_SECRET_BASE_HIGH_TV_FLAGS, VarGet(VAR_SECRET_BASE_HIGH_TV_FLAGS) | SECRET_BASE_USED_BREAKABLE_DOOR);

            ShatterSecretBaseBreakableDoor(x, y);
        }
        else if (MetatileBehavior_IsSecretBaseSoundMat(behavior) == TRUE){
            if (sInFriendSecretBase == TRUE)
                VarSet(VAR_SECRET_BASE_LOW_TV_FLAGS, VarGet(VAR_SECRET_BASE_LOW_TV_FLAGS) | SECRET_BASE_USED_NOTE_MAT);
        }
        else if (MetatileBehavior_IsSecretBaseJumpMat(behavior) == TRUE)
        {
            if (sInFriendSecretBase == TRUE)
                VarSet(VAR_SECRET_BASE_HIGH_TV_FLAGS, VarGet(VAR_SECRET_BASE_HIGH_TV_FLAGS) | SECRET_BASE_USED_JUMP_MAT);
        }
        else if (MetatileBehavior_IsSecretBaseSpinMat(behavior) == TRUE)
        {
            if (sInFriendSecretBase == TRUE)
                VarSet(VAR_SECRET_BASE_HIGH_TV_FLAGS, VarGet(VAR_SECRET_BASE_HIGH_TV_FLAGS) | SECRET_BASE_USED_SPIN_MAT);
        }
        break;
    case 2:
        // This state is never reached, and tFldEff is never set
        if (!FieldEffectActiveListContains(tFldEff))
            tState = 1;
        break;
    }
}

#undef tStepCb
#undef tState
#undef tPlayerX
#undef tPlayerY
#undef tFldEff

static void SaveSecretBase(u8 secretBaseIdx, u32 version, u32 language)
{

}

static bool8 SecretBasesHaveSameTrainerId(void)
{

}

static bool8 SecretBasesHaveSameTrainerName(void)
{

}

static bool8 SecretBasesBelongToSamePlayer(void)
{

}

static s16 GetSecretBaseIndexFromId(u8 secretBaseId)
{

}

static u8 FindAvailableSecretBaseIndex(void)
{

}

static u8 FindUnregisteredSecretBaseIndex(void)
{

}

static u8 TrySaveFriendsSecretBase(u32 version, u32 language)
{

}

// Moves the registered secret bases to the beginning of the array, so that
// they won't be forgotten during record mixing.
static void SortSecretBasesByRegistryStatus(void)
{

}

// Used to save a record mixing friends' bases other than their own
// registryStatus is so registered bases can be attempted first
static void TrySaveFriendsSecretBases(u8 registryStatus)
{

}

static bool8 SecretBaseBelongsToPlayer(void)
{

}

#define DELETED_BASE_A  (1 << 0)
#define DELETED_BASE_B  (1 << 1)
#define DELETED_BASE_C  (1 << 2)

static void DeleteFirstOldBaseFromPlayerInRecordMixingFriendsRecords(void)
{

}

#undef DELETED_BASE_A
#undef DELETED_BASE_B
#undef DELETED_BASE_C

// returns TRUE if secretBase was deleted, FALSE otherwise
static bool8 ClearDuplicateOwnedSecretBase(void)
{

}

static void ClearDuplicateOwnedSecretBases(void)
{

}

static void TrySaveRegisteredDuplicate(u32 version, u32 language)
{

}

static void TrySaveRegisteredDuplicates(void)
{

}

static void SaveRecordMixBases(void)
{

}

#define INIT_SECRET_BASE_RECORD_MIXER(linkId1, linkId2, linkId3)        \
            mixers[0].secretBases = secretBases + linkId1 * recordSize; \
            mixers[0].version = gLinkPlayers[linkId1].version & 0xFF;   \
            mixers[0].language = gLinkPlayers[linkId1].language;        \
            mixers[1].secretBases = secretBases + linkId2 * recordSize; \
            mixers[1].version = gLinkPlayers[linkId2].version & 0xFF;   \
            mixers[1].language = gLinkPlayers[linkId2].language;        \
            mixers[2].secretBases = secretBases + linkId3 * recordSize; \
            mixers[2].version = gLinkPlayers[linkId3].version & 0xFF;   \
            mixers[2].language = gLinkPlayers[linkId3].language;

void ReceiveSecretBasesData(void *secretBases, size_t recordSize, u8 linkIdx)
{

}

void ClearJapaneseSecretBases(void)
{

}

void InitSecretBaseVars(void)
{
    VarSet(VAR_SECRET_BASE_STEP_COUNTER, 0);
    VarSet(VAR_SECRET_BASE_LAST_ITEM_USED, 0);
    VarSet(VAR_SECRET_BASE_LOW_TV_FLAGS, 0);
    VarSet(VAR_SECRET_BASE_HIGH_TV_FLAGS, 0);
    if (VarGet(VAR_CURRENT_SECRET_BASE) != 0)
        VarSet(VAR_SECRET_BASE_IS_NOT_LOCAL, TRUE);
    else
        VarSet(VAR_SECRET_BASE_IS_NOT_LOCAL, FALSE);

    sInFriendSecretBase = FALSE;
}

void CheckLeftFriendsSecretBase(void)
{
    if (VarGet(VAR_SECRET_BASE_IS_NOT_LOCAL) && sInFriendSecretBase == TRUE && !CurMapIsSecretBase())
    {
        VarSet(VAR_SECRET_BASE_IS_NOT_LOCAL, FALSE);
        sInFriendSecretBase = FALSE;
        TryPutSecretBaseSecretsOnAir();
        VarSet(VAR_SECRET_BASE_STEP_COUNTER, 0);
        VarSet(VAR_SECRET_BASE_LAST_ITEM_USED, 0);
        VarSet(VAR_SECRET_BASE_LOW_TV_FLAGS, 0);
        VarSet(VAR_SECRET_BASE_HIGH_TV_FLAGS, 0);
        VarSet(VAR_SECRET_BASE_IS_NOT_LOCAL, FALSE);
    }
}

void CheckInteractedWithFriendsDollDecor(void)
{
    if (VarGet(VAR_CURRENT_SECRET_BASE) != 0)
        VarSet(VAR_SECRET_BASE_HIGH_TV_FLAGS, VarGet(VAR_SECRET_BASE_HIGH_TV_FLAGS) | SECRET_BASE_USED_DOLL);
}

void CheckInteractedWithFriendsCushionDecor(void)
{
    if (VarGet(VAR_CURRENT_SECRET_BASE) != 0)
        VarSet(VAR_SECRET_BASE_LOW_TV_FLAGS, VarGet(VAR_SECRET_BASE_LOW_TV_FLAGS) | SECRET_BASE_USED_CUSHION);
}

void DeclinedSecretBaseBattle(void)
{
    if (VarGet(VAR_CURRENT_SECRET_BASE) != 0)
    {
        VarSet(VAR_SECRET_BASE_LOW_TV_FLAGS, VarGet(VAR_SECRET_BASE_LOW_TV_FLAGS) & ~(SECRET_BASE_BATTLED_WON | SECRET_BASE_BATTLED_LOST | SECRET_BASE_DECLINED_BATTLE));
        VarSet(VAR_SECRET_BASE_HIGH_TV_FLAGS, VarGet(VAR_SECRET_BASE_HIGH_TV_FLAGS) & ~(SECRET_BASE_BATTLED_DRAW));
        VarSet(VAR_SECRET_BASE_LOW_TV_FLAGS, VarGet(VAR_SECRET_BASE_LOW_TV_FLAGS) | SECRET_BASE_DECLINED_BATTLE);
    }
}

void WonSecretBaseBattle(void)
{
    if (VarGet(VAR_CURRENT_SECRET_BASE) != 0)
    {
        VarSet(VAR_SECRET_BASE_LOW_TV_FLAGS, VarGet(VAR_SECRET_BASE_LOW_TV_FLAGS) & ~(SECRET_BASE_BATTLED_WON | SECRET_BASE_BATTLED_LOST | SECRET_BASE_DECLINED_BATTLE));
        VarSet(VAR_SECRET_BASE_HIGH_TV_FLAGS, VarGet(VAR_SECRET_BASE_HIGH_TV_FLAGS) & ~(SECRET_BASE_BATTLED_DRAW));
        VarSet(VAR_SECRET_BASE_LOW_TV_FLAGS, VarGet(VAR_SECRET_BASE_LOW_TV_FLAGS) | SECRET_BASE_BATTLED_WON);
    }
}

void LostSecretBaseBattle(void)
{
    if (VarGet(VAR_CURRENT_SECRET_BASE) != 0)
    {
        VarSet(VAR_SECRET_BASE_LOW_TV_FLAGS, VarGet(VAR_SECRET_BASE_LOW_TV_FLAGS) & ~(SECRET_BASE_BATTLED_WON | SECRET_BASE_BATTLED_LOST | SECRET_BASE_DECLINED_BATTLE));
        VarSet(VAR_SECRET_BASE_HIGH_TV_FLAGS, VarGet(VAR_SECRET_BASE_HIGH_TV_FLAGS) & ~(SECRET_BASE_BATTLED_DRAW));
        VarSet(VAR_SECRET_BASE_LOW_TV_FLAGS, VarGet(VAR_SECRET_BASE_LOW_TV_FLAGS) | SECRET_BASE_BATTLED_LOST);
    }
}

void DrewSecretBaseBattle(void)
{
    if (VarGet(VAR_CURRENT_SECRET_BASE) != 0)
    {
        VarSet(VAR_SECRET_BASE_LOW_TV_FLAGS, VarGet(VAR_SECRET_BASE_LOW_TV_FLAGS) & ~(SECRET_BASE_BATTLED_WON | SECRET_BASE_BATTLED_LOST | SECRET_BASE_DECLINED_BATTLE));
        VarSet(VAR_SECRET_BASE_HIGH_TV_FLAGS, VarGet(VAR_SECRET_BASE_HIGH_TV_FLAGS) & ~(SECRET_BASE_BATTLED_DRAW));
        VarSet(VAR_SECRET_BASE_HIGH_TV_FLAGS, VarGet(VAR_SECRET_BASE_HIGH_TV_FLAGS) | SECRET_BASE_BATTLED_DRAW);
    }
}

void CheckInteractedWithFriendsPosterDecor(void)
{
    s16 x, y;

    GetXYCoordsOneStepInFrontOfPlayer(&x, &y);
    switch (MapGridGetMetatileIdAt(x, y))
    {
        case METATILE_SecretBase_PikaPoster_Left:
        case METATILE_SecretBase_PikaPoster_Right:
        case METATILE_SecretBase_LongPoster_Left:
        case METATILE_SecretBase_LongPoster_Right:
        case METATILE_SecretBase_SeaPoster_Left:
        case METATILE_SecretBase_SeaPoster_Right:
        case METATILE_SecretBase_SkyPoster_Left:
        case METATILE_SecretBase_SkyPoster_Right:
        case METATILE_SecretBase_KissPoster_Left:
        case METATILE_SecretBase_KissPoster_Right:
        case METATILE_SecretBase_BallPoster:
        case METATILE_SecretBase_GreenPoster:
        case METATILE_SecretBase_RedPoster:
        case METATILE_SecretBase_BluePoster:
        case METATILE_SecretBase_CutePoster:
            if (VarGet(VAR_CURRENT_SECRET_BASE) != 0)
                VarSet(VAR_SECRET_BASE_LOW_TV_FLAGS, VarGet(VAR_SECRET_BASE_LOW_TV_FLAGS) | SECRET_BASE_USED_POSTER);
            break;
    }
}

void CheckInteractedWithFriendsFurnitureBottom(void)
{
    s16 x, y;

    GetXYCoordsOneStepInFrontOfPlayer(&x, &y);
    switch (MapGridGetMetatileIdAt(x, y))
    {
        case METATILE_SecretBase_GlassOrnament_Base1:
        case METATILE_SecretBase_GlassOrnament_Base2:
            if (VarGet(VAR_CURRENT_SECRET_BASE) != 0)
                VarSet(VAR_SECRET_BASE_LOW_TV_FLAGS, VarGet(VAR_SECRET_BASE_LOW_TV_FLAGS) | SECRET_BASE_USED_GLASS_ORNAMENT);
            break;
        case METATILE_SecretBase_RedPlant_Base1:
        case METATILE_SecretBase_RedPlant_Base2:
        case METATILE_SecretBase_TropicalPlant_Base1:
        case METATILE_SecretBase_TropicalPlant_Base2:
        case METATILE_SecretBase_PrettyFlowers_Base1:
        case METATILE_SecretBase_PrettyFlowers_Base2:
        case METATILE_SecretBase_ColorfulPlant_BaseLeft1:
        case METATILE_SecretBase_ColorfulPlant_BaseRight1:
        case METATILE_SecretBase_ColorfulPlant_BaseLeft2:
        case METATILE_SecretBase_ColorfulPlant_BaseRight2:
        case METATILE_SecretBase_BigPlant_BaseLeft1:
        case METATILE_SecretBase_BigPlant_BaseRight1:
        case METATILE_SecretBase_BigPlant_BaseLeft2:
        case METATILE_SecretBase_BigPlant_BaseRight2:
        case METATILE_SecretBase_GorgeousPlant_BaseLeft1:
        case METATILE_SecretBase_GorgeousPlant_BaseRight1:
        case METATILE_SecretBase_GorgeousPlant_BaseLeft2:
        case METATILE_SecretBase_GorgeousPlant_BaseRight2:
            if (VarGet(VAR_CURRENT_SECRET_BASE) != 0)
                VarSet(VAR_SECRET_BASE_LOW_TV_FLAGS, VarGet(VAR_SECRET_BASE_LOW_TV_FLAGS) | SECRET_BASE_USED_PLANT);
            break;
        case METATILE_SecretBase_Fence_Horizontal:
        case METATILE_SecretBase_Fence_Vertical:
            if (VarGet(VAR_CURRENT_SECRET_BASE) != 0)
                VarSet(VAR_SECRET_BASE_HIGH_TV_FLAGS, VarGet(VAR_SECRET_BASE_HIGH_TV_FLAGS) | SECRET_BASE_USED_FENCE);
            break;
        case METATILE_SecretBase_Tire_BottomLeft:
        case METATILE_SecretBase_Tire_BottomRight:
            if (VarGet(VAR_CURRENT_SECRET_BASE) != 0)
                VarSet(VAR_SECRET_BASE_HIGH_TV_FLAGS, VarGet(VAR_SECRET_BASE_HIGH_TV_FLAGS) | SECRET_BASE_USED_TIRE);
            break;
        case METATILE_SecretBase_RedBrick_Bottom:
        case METATILE_SecretBase_YellowBrick_Bottom:
        case METATILE_SecretBase_BlueBrick_Bottom:
            if (VarGet(VAR_CURRENT_SECRET_BASE) != 0)
                VarSet(VAR_SECRET_BASE_HIGH_TV_FLAGS, VarGet(VAR_SECRET_BASE_HIGH_TV_FLAGS) | SECRET_BASE_USED_BRICK);
            break;
        case METATILE_SecretBase_SmallDesk:
        case METATILE_SecretBase_PokemonDesk:
        case METATILE_SecretBase_HeavyDesk_BottomLeft:
        case METATILE_SecretBase_HeavyDesk_BottomMid:
        case METATILE_SecretBase_HeavyDesk_BottomRight:
        case METATILE_SecretBase_RaggedDesk_BottomLeft:
        case METATILE_SecretBase_RaggedDesk_BottomMid:
        case METATILE_SecretBase_RaggedDesk_BottomRight:
        case METATILE_SecretBase_ComfortDesk_BottomLeft:
        case METATILE_SecretBase_ComfortDesk_BottomMid:
        case METATILE_SecretBase_ComfortDesk_BottomRight:
        case METATILE_SecretBase_BrickDesk_BottomLeft:
        case METATILE_SecretBase_BrickDesk_BottomMid:
        case METATILE_SecretBase_BrickDesk_BottomRight:
        case METATILE_SecretBase_CampDesk_BottomLeft:
        case METATILE_SecretBase_CampDesk_BottomMid:
        case METATILE_SecretBase_CampDesk_BottomRight:
        case METATILE_SecretBase_HardDesk_BottomLeft:
        case METATILE_SecretBase_HardDesk_BottomMid:
        case METATILE_SecretBase_HardDesk_BottomRight:
        case METATILE_SecretBase_PrettyDesk_BottomLeft:
        case METATILE_SecretBase_PrettyDesk_BottomMid:
        case METATILE_SecretBase_PrettyDesk_BottomRight:
            if (VarGet(VAR_CURRENT_SECRET_BASE) != 0)
                VarSet(VAR_SECRET_BASE_HIGH_TV_FLAGS, VarGet(VAR_SECRET_BASE_HIGH_TV_FLAGS) | SECRET_BASE_USED_DESK);
            break;
    }
}

void CheckInteractedWithFriendsFurnitureMiddle(void)
{
    s16 x, y;

    GetXYCoordsOneStepInFrontOfPlayer(&x, &y);
    switch (MapGridGetMetatileIdAt(x, y))
    {
        case METATILE_SecretBase_HeavyDesk_TopMid:
        case METATILE_SecretBase_RaggedDesk_TopMid:
        case METATILE_SecretBase_ComfortDesk_TopMid:
        case METATILE_SecretBase_BrickDesk_TopMid:
        case METATILE_SecretBase_BrickDesk_Center:
        case METATILE_SecretBase_CampDesk_TopMid:
        case METATILE_SecretBase_CampDesk_Center:
        case METATILE_SecretBase_HardDesk_TopMid:
        case METATILE_SecretBase_HardDesk_Center:
        case METATILE_SecretBase_PrettyDesk_TopMid:
        case METATILE_SecretBase_PrettyDesk_Center:
            if (VarGet(VAR_CURRENT_SECRET_BASE) != 0)
                VarSet(VAR_SECRET_BASE_HIGH_TV_FLAGS, VarGet(VAR_SECRET_BASE_HIGH_TV_FLAGS) | SECRET_BASE_USED_DESK);
            break;
    }
}

void CheckInteractedWithFriendsFurnitureTop(void)
{
    s16 x, y;

    GetXYCoordsOneStepInFrontOfPlayer(&x, &y);
    switch (MapGridGetMetatileIdAt(x, y))
    {
        case METATILE_SecretBase_HeavyDesk_TopLeft:
        case METATILE_SecretBase_HeavyDesk_TopRight:
        case METATILE_SecretBase_RaggedDesk_TopLeft:
        case METATILE_SecretBase_RaggedDesk_TopRight:
        case METATILE_SecretBase_ComfortDesk_TopLeft:
        case METATILE_SecretBase_ComfortDesk_TopRight:
        case METATILE_SecretBase_BrickDesk_TopLeft:
        case METATILE_SecretBase_BrickDesk_TopRight:
        case METATILE_SecretBase_BrickDesk_MidLeft:
        case METATILE_SecretBase_BrickDesk_MidRight:
        case METATILE_SecretBase_CampDesk_TopLeft:
        case METATILE_SecretBase_CampDesk_TopRight:
        case METATILE_SecretBase_CampDesk_MidLeft:
        case METATILE_SecretBase_CampDesk_MidRight:
        case METATILE_SecretBase_HardDesk_TopLeft:
        case METATILE_SecretBase_HardDesk_TopRight:
        case METATILE_SecretBase_HardDesk_MidLeft:
        case METATILE_SecretBase_HardDesk_MidRight:
        case METATILE_SecretBase_PrettyDesk_TopLeft:
        case METATILE_SecretBase_PrettyDesk_TopRight:
        case METATILE_SecretBase_PrettyDesk_MidLeft:
        case METATILE_SecretBase_PrettyDesk_MidRight:
            if (VarGet(VAR_CURRENT_SECRET_BASE) != 0)
                VarSet(VAR_SECRET_BASE_HIGH_TV_FLAGS, VarGet(VAR_SECRET_BASE_HIGH_TV_FLAGS) | SECRET_BASE_USED_DESK);
            break;
        case METATILE_SecretBase_Tire_TopLeft:
        case METATILE_SecretBase_Tire_TopRight:
            if (VarGet(VAR_CURRENT_SECRET_BASE) != 0)
                VarSet(VAR_SECRET_BASE_HIGH_TV_FLAGS, VarGet(VAR_SECRET_BASE_HIGH_TV_FLAGS) | SECRET_BASE_USED_TIRE);
            break;
        case METATILE_SecretBase_RedBrick_Top:
        case METATILE_SecretBase_YellowBrick_Top:
        case METATILE_SecretBase_BlueBrick_Top:
            if (VarGet(VAR_CURRENT_SECRET_BASE) != 0)
                VarSet(VAR_SECRET_BASE_HIGH_TV_FLAGS, VarGet(VAR_SECRET_BASE_HIGH_TV_FLAGS) | SECRET_BASE_USED_BRICK);
            break;
    }
}

void CheckInteractedWithFriendsSandOrnament(void)
{
    s16 x, y;

    GetXYCoordsOneStepInFrontOfPlayer(&x, &y);
    switch ((int)MapGridGetMetatileIdAt(x, y))
    {
        case METATILE_SecretBase_SandOrnament_Base1:
        case METATILE_SecretBase_SandOrnament_Base2:
            if (VarGet(VAR_CURRENT_SECRET_BASE) != 0)
                VarSet(VAR_SECRET_BASE_HIGH_TV_FLAGS, VarGet(VAR_SECRET_BASE_HIGH_TV_FLAGS) | SECRET_BASE_USED_SAND_ORNAMENT);
            break;
    }
}
