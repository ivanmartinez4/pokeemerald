#include "global.h"
#include "malloc.h"
#include "random.h"
#include "constants/items.h"
#include "text.h"
#include "item.h"
#include "task.h"
#include "save.h"
#include "load_save.h"
#include "pokemon.h"
#include "cable_club.h"
#include "link.h"
#include "link_rfu.h"
#include "tv.h"
#include "battle_tower.h"
#include "window.h"
#include "mystery_event_script.h"
#include "secret_base.h"
#include "mauville_old_man.h"
#include "sound.h"
#include "constants/songs.h"
#include "menu.h"
#include "overworld.h"
#include "field_screen_effect.h"
#include "fldeff_misc.h"
#include "script.h"
#include "event_data.h"
#include "lilycove_lady.h"
#include "strings.h"
#include "string_util.h"
#include "record_mixing.h"
#include "new_game.h"
#include "daycare.h"
#include "international_string_util.h"
#include "constants/battle_frontier.h"
#include "dewford_trend.h"

// Number of bytes of the record transferred at a time
#define BUFFER_CHUNK_SIZE 200

#define NUM_SWAP_COMBOS 3

// Used by several tasks in this file
#define tState        data[0]

struct PlayerRecordRS
{
    struct SecretBase secretBases[SECRET_BASES_COUNT];
    struct RSBattleTowerRecord battleTowerRecord;
    u16 giftItem;
    u16 filler[50];
};

struct PlayerRecordEmerald
{
    /* 0x0000 */ struct SecretBase secretBases[SECRET_BASES_COUNT];
    /* 0x1124 */ struct EmeraldBattleTowerRecord battleTowerRecord;
    /* 0x1210 */ u16 giftItem;
    /* 0x1434 */ u8 filler_1434[16];
}; // 0x1444

union PlayerRecord
{
    struct PlayerRecordRS ruby;
    struct PlayerRecordEmerald emerald;
};

static bool8 sReadyToReceive;
static struct SecretBase *sSecretBasesSave;
static void *sBattleTowerSave;
static void *sApprenticesSave;
static u32 sRecordStructSize;
static u8 sDaycareMailRandSum;
static EWRAM_DATA union PlayerRecord *sReceivedRecords = NULL;
static EWRAM_DATA union PlayerRecord *sSentRecord = NULL;

static void Task_RecordMixing_Main(u8);
static void Task_MixingRecordsRecv(u8);
static void Task_SendPacket(u8);
static void Task_CopyReceiveBuffer(u8);
static void Task_SendPacket_SwitchToReceive(u8);
static void *LoadPtrFromTaskData(const u16 *);
static void StorePtrInTaskData(void *, u16 *);
static u8 GetMultiplayerId_(void);
static void *GetPlayerRecvBuffer(u8);
static void ReceiveOldManData(u8, size_t, u8);
static void ReceiveBattleTowerData(void *, size_t, u8);
static void ReceiveLilycoveLadyData(u8, size_t, u8);
static void CalculateDaycareMailRandSum(const u8 *);
static void ReceiveDaycareMailData(u8, size_t, u8, u8);
static void ReceiveGiftItem(u16 *, u8 );
static void Task_DoRecordMixing(u8);
static void GetSavedApprentices(u8, u8);
static void ReceiveApprenticeData(u8, size_t, u32);
static void ReceiveRankingHallRecords(u8, size_t, u32);
static void GetRecordMixingDaycareMail(u8);
static void SanitizeDaycareMailForRuby(u8);
static void SanitizeEmeraldBattleTowerRecord(struct EmeraldBattleTowerRecord *);
static void SanitizeRubyBattleTowerRecord(struct RSBattleTowerRecord *);

static const u8 sPlayerIdxOrders_2Player[] = {1, 0};

static const u8 sPlayerIdxOrders_3Player[][3] =
{
    {1, 2, 0},
    {2, 0, 1},
};

static const u8 sPlayerIdxOrders_4Player[][4] =
{
    {1, 0, 3, 2},
    {3, 0, 1, 2},
    {2, 0, 3, 1},
    {1, 3, 0, 2},
    {2, 3, 0, 1},
    {3, 2, 0, 1},
    {1, 2, 3, 0},
    {2, 3, 1, 0},
    {3, 2, 1, 0},
};

// When 3 players can swap mail 2 players are randomly selected and the 3rd is left out
static const u8 sDaycareMailSwapIds_3Player[NUM_SWAP_COMBOS][2] =
{
    {0, 1},
    {1, 2},
    {2, 0},
};

static const u8 sDaycareMailSwapIds_4Player[NUM_SWAP_COMBOS][4] =
{
    {0, 1,   2, 3}, // 0 swaps with 1, 2 swaps with 3
    {0, 2,   1, 3},
    {0, 3,   2, 1},
};

void RecordMixingPlayerSpotTriggered(void)
{
    CreateTask_EnterCableClubSeat(Task_RecordMixing_Main);
}

// these variables were const in R/S, but had to become changeable because of saveblocks changing RAM position
static void SetSrcLookupPointers(void)
{

}

static void PrepareUnknownExchangePacket(struct PlayerRecordRS *dest)
{

}

static void PrepareExchangePacketForRubySapphire(struct PlayerRecordRS *dest)
{

}

static void PrepareExchangePacket(void)
{

}

static void ReceiveExchangePacket(u32 multiplayerId)
{

}

static void PrintTextOnRecordMixing(const u8 *src)
{
    DrawDialogueFrame(0, FALSE);
    AddTextPrinterParameterized(0, FONT_NORMAL, src, 0, 1, 0, NULL);
    CopyWindowToVram(0, COPYWIN_FULL);
}

#define tCounter data[0]

static void Task_RecordMixing_SoundEffect(u8 taskId)
{
    if (++gTasks[taskId].tCounter == 50)
    {
        PlaySE(SE_M_ATTRACT);
        gTasks[taskId].tCounter = 0;
    }
}

#undef tCounter

#define tTimer       data[8]
#define tLinkTaskId  data[10]
#define tSoundTaskId data[15]

// Note: gSpecialVar_0x8005 here contains the player's spot id.
static void Task_RecordMixing_Main(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    switch (tState)
    {
    case 0: // init
        sSentRecord = Alloc(sizeof(*sSentRecord));
        sReceivedRecords = Alloc(sizeof(*sReceivedRecords) * MAX_LINK_PLAYERS);
        SetLocalLinkPlayerId(gSpecialVar_0x8005);
        VarSet(VAR_TEMP_0, 1);
        sReadyToReceive = FALSE;
        PrepareExchangePacket();
        CreateRecordMixingLights();
        tState = 1;
        tLinkTaskId = CreateTask(Task_MixingRecordsRecv, 80);
        tSoundTaskId = CreateTask(Task_RecordMixing_SoundEffect, 81);
        break;
    case 1: // wait for Task_MixingRecordsRecv
        if (!gTasks[tLinkTaskId].isActive)
        {
            tState = 2;
            FlagSet(FLAG_SYS_MIX_RECORD);
            DestroyRecordMixingLights();
            DestroyTask(tSoundTaskId);
        }
        break;
    case 2:
        tLinkTaskId = CreateTask(Task_DoRecordMixing, 10);
        tState = 3;
        PlaySE(SE_M_BATON_PASS);
        break;
    case 3: // wait for Task_DoRecordMixing
        if (!gTasks[tLinkTaskId].isActive)
        {
            tState = 4;
            if (gWirelessCommType == 0)
                tLinkTaskId = CreateTask_ReestablishCableClubLink();

            PrintTextOnRecordMixing(gText_RecordMixingComplete);
            tTimer = 0;
        }
        break;
    case 4: // wait 60 frames
        if (++tTimer > 60)
            tState = 5;
        break;
    case 5: // Wait for the task created by CreateTask_ReestablishCableClubLink
        if (!gTasks[tLinkTaskId].isActive)
        {
            Free(sReceivedRecords);
            Free(sSentRecord);
            SetLinkWaitingForScript();
            if (gWirelessCommType != 0)
                CreateTask(Task_ReturnToFieldRecordMixing, 10);
            ClearDialogWindowAndFrame(0, TRUE);
            DestroyTask(taskId);
            ScriptContext_Enable();
        }
        break;
    }
}

#undef tTimer
#undef tLinkTaskId
#undef tSoundTaskId

// Task data for Task_MixingRecordsRecv and subsequent tasks
#define tSentRecord    data[2] // Used to store a ptr, so data[2] and data[3]
#define tNumChunksSent data[4]
#define tMultiplayerId data[5]
#define tCopyTaskId    data[10]

// Task data for Task_CopyReceiveBuffer
#define tParentTaskId     data[0]
#define tNumChunksRecv(i) data[1 + (i)] // Number of chunks of the record received per player
#define tRecvRecords      data[5] // Used to store a ptr, so data[5] and data[6]

static void Task_MixingRecordsRecv(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    switch (task->tState)
    {
    case 0:
        PrintTextOnRecordMixing(gText_MixingRecords);
        task->data[8] = 0x708;
        task->tState = 400;
        ClearLinkCallback_2();
        break;
    case 100: // wait 20 frames
        if (++task->data[12] > 20)
        {
            task->data[12] = 0;
            task->tState = 101;
        }
        break;
    case 101:
        {
            u8 players = GetLinkPlayerCount_2();
            if (IsLinkMaster() == TRUE)
            {
                if (players == GetSavedPlayerCount())
                {
                    PlaySE(SE_PIN);
                    task->tState = 201;
                    task->data[12] = 0;
                }
            }
            else
            {
                PlaySE(SE_BOO);
                task->tState = 301;
            }
        }
        break;
    case 201:
        // We're the link master. Delay for 30 frames per connected player.
        if (GetSavedPlayerCount() == GetLinkPlayerCount_2() && ++task->data[12] > (GetLinkPlayerCount_2() * 30))
        {
            CheckShouldAdvanceLinkState();
            task->tState = 1;
        }
        break;
    case 301:
        if (GetSavedPlayerCount() == GetLinkPlayerCount_2())
            task->tState = 1;
        break;
    case 400: // wait 20 frames
        if (++task->data[12] > 20)
        {
            task->tState = 1;
            task->data[12] = 0;
        }
        break;
    case 1: // wait for handshake
        if (gReceivedRemoteLinkPlayers)
        {
            ConvertIntToDecimalStringN(gStringVar1, GetMultiplayerId_(), STR_CONV_MODE_LEADING_ZEROS, 2);
            task->tState = 5;
        }
        break;
    case 2:
        {
            u8 subTaskId;

            task->data[6] = GetLinkPlayerCount_2();
            task->tState = 0;
            task->tMultiplayerId = GetMultiplayerId_();
            task->func = Task_SendPacket;
            if (Link_AnyPartnersPlayingRubyOrSapphire())
            {
                StorePtrInTaskData(sSentRecord, &task->tSentRecord);
                subTaskId = CreateTask(Task_CopyReceiveBuffer, 80);
                task->tCopyTaskId = subTaskId;
                gTasks[subTaskId].tParentTaskId = taskId;
                StorePtrInTaskData(sReceivedRecords, &gTasks[subTaskId].tRecvRecords);
                sRecordStructSize = sizeof(struct PlayerRecordRS);
            }
            else
            {
                StorePtrInTaskData(sSentRecord, &task->tSentRecord);
                subTaskId = CreateTask(Task_CopyReceiveBuffer, 80);
                task->tCopyTaskId = subTaskId;
                gTasks[subTaskId].tParentTaskId = taskId;
                StorePtrInTaskData(sReceivedRecords, &gTasks[subTaskId].tRecvRecords);
                sRecordStructSize = sizeof(struct PlayerRecordEmerald);
            }
        }
        break;
    case 5: // wait 60 frames
        if (++task->data[10] > 60)
        {
            task->data[10] = 0;
            task->tState = 2;
        }
        break;
    }
}

static void Task_SendPacket(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    switch (task->tState)
    {
    case 0: // Copy record data chunk to send buffer
        {
            void *recordData = LoadPtrFromTaskData(&task->tSentRecord) + task->tNumChunksSent * BUFFER_CHUNK_SIZE;

            memcpy(gBlockSendBuffer, recordData, BUFFER_CHUNK_SIZE);
            task->tState++;
        }
        break;
    case 1:
        if (GetMultiplayerId() == 0)
            SendBlockRequest(BLOCK_REQ_SIZE_200);
        task->tState++;
        break;
    case 2:
        break;
    case 3:
        // If sent final chunk of record, move on to next state.
        // Otherwise return to first state and send next chunk.
        task->tNumChunksSent++;
        if (task->tNumChunksSent == sRecordStructSize / BUFFER_CHUNK_SIZE + 1)
            task->tState++;
        else
            task->tState = 0;
        break;
    case 4:
        if (!gTasks[task->tCopyTaskId].isActive)
            task->func = Task_SendPacket_SwitchToReceive;
        break;
    }
}

static void Task_CopyReceiveBuffer(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    u8 status = GetBlockReceivedStatus();
    u8 handledPlayers = 0;

    if (status == GetLinkPlayerCountAsBitFlags())
    {
        u8 i;
        for (i = 0; i < GetLinkPlayerCount(); i++)
        {
            if ((status >> i) & 1)
            {
                void *dest = LoadPtrFromTaskData(&task->tRecvRecords) + task->tNumChunksRecv(i) * BUFFER_CHUNK_SIZE + sRecordStructSize * i;
                void *src = GetPlayerRecvBuffer(i);
                if ((task->tNumChunksRecv(i) + 1) * BUFFER_CHUNK_SIZE > sRecordStructSize)
                    memcpy(dest, src, sRecordStructSize - task->tNumChunksRecv(i) * BUFFER_CHUNK_SIZE);
                else
                    memcpy(dest, src, BUFFER_CHUNK_SIZE);
                ResetBlockReceivedFlag(i);
                task->tNumChunksRecv(i)++;
                if (task->tNumChunksRecv(i) == sRecordStructSize / BUFFER_CHUNK_SIZE + 1)
                    handledPlayers++;
            }
        }
        gTasks[task->tParentTaskId].tState++;
    }

    if (handledPlayers == GetLinkPlayerCount())
        DestroyTask(taskId);
}

static void Task_WaitReceivePacket(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    // Wait for Task_CopyReceiveBuffer to finish
    if (!gTasks[task->tCopyTaskId].isActive)
        DestroyTask(taskId);
}

static void Task_ReceivePacket(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    task->func = Task_WaitReceivePacket;
    if (sReadyToReceive == TRUE)
        ReceiveExchangePacket(task->tMultiplayerId);
}

static void Task_SendPacket_SwitchToReceive(u8 taskId)
{
    gTasks[taskId].func = Task_ReceivePacket;
    sReadyToReceive = TRUE;
}

static void *LoadPtrFromTaskData(const u16 *asShort)
{
    return (void *)(asShort[0] | (asShort[1] << 16));
}

static void StorePtrInTaskData(void *records, u16 *asShort)
{
    asShort[0] = (u32)records;
    asShort[1] = ((u32)records >> 16);
}

static u8 GetMultiplayerId_(void)
{
    return GetMultiplayerId();
}

static void *GetPlayerRecvBuffer(u8 id)
{
    return gBlockRecvBuffer[id];
}

static void ShufflePlayerIndices(u32 *data)
{
    u32 i;
    u32 linkTrainerId;
    u32 players = GetLinkPlayerCount();

    switch (players)
    {
    case 2:
        for (i = 0; i < ARRAY_COUNT(sPlayerIdxOrders_2Player); i++)
            data[i] = sPlayerIdxOrders_2Player[i];
        break;
    case 3:
        linkTrainerId = GetLinkPlayerTrainerId(0) % ARRAY_COUNT(sPlayerIdxOrders_3Player);
        for (i = 0; i < ARRAY_COUNT(sPlayerIdxOrders_3Player[0]); i++)
            data[i] = sPlayerIdxOrders_3Player[linkTrainerId][i];
        break;
    case 4:
        linkTrainerId = GetLinkPlayerTrainerId(0) % ARRAY_COUNT(sPlayerIdxOrders_4Player);
        for (i = 0; i < ARRAY_COUNT(sPlayerIdxOrders_4Player[0]); i++)
            data[i] = sPlayerIdxOrders_4Player[linkTrainerId][i];
        break;
    }
}

static void ReceiveOldManData(u8 unused, size_t recordSize, u8 multiplayerId)
{

}

static void ReceiveBattleTowerData(void *records, size_t recordSize, u8 multiplayerId)
{
    struct EmeraldBattleTowerRecord *battleTowerRecord;
    struct BattleTowerPokemon *btPokemon;
    u32 mixIndices[MAX_LINK_PLAYERS];
    s32 i;

    ShufflePlayerIndices(mixIndices);
    if (Link_AnyPartnersPlayingRubyOrSapphire())
    {
        if (RubyBattleTowerRecordToEmerald((void *)records + recordSize * mixIndices[multiplayerId], (void *)records + recordSize * multiplayerId) == TRUE)
        {
            battleTowerRecord = (void *)records + recordSize * multiplayerId;
            battleTowerRecord->language = gLinkPlayers[mixIndices[multiplayerId]].language;
            CalcEmeraldBattleTowerChecksum(battleTowerRecord);
        }
    }
    else
    {
        memcpy((void *)records + recordSize * multiplayerId, (void *)records + recordSize * mixIndices[multiplayerId], sizeof(struct EmeraldBattleTowerRecord));
        battleTowerRecord = (void *)records + recordSize * multiplayerId;
        for (i = 0; i < MAX_FRONTIER_PARTY_SIZE; i++)
        {
            btPokemon = &battleTowerRecord->party[i];
            if (btPokemon->species != SPECIES_NONE && IsStringJapanese(btPokemon->nickname))
                ConvertInternationalString(btPokemon->nickname, LANGUAGE_JAPANESE);
        }
        CalcEmeraldBattleTowerChecksum(battleTowerRecord);
    }
    PutNewBattleTowerRecord((void *)records + recordSize * multiplayerId);
}

static void ReceiveLilycoveLadyData(u8 unused, size_t recordSize, u8 multiplayerId)
{

}

static u8 GetDaycareMailItemId(u8 unused)
{

}

// Indexes for a 2 element array used to store the multiplayer id and daycare
// slot that correspond to a daycare Pokémon that can hold an item.
enum {
    MULTIPLAYER_ID,
    DAYCARE_SLOT,
};

static void SwapDaycareMail(u8 unused, size_t recordSize, u8 (*idxs)[2], u8 playerSlot1, u8 playerSlot2)
{

}

// This sum is used to determine which players will swap daycare mail if there are more than 2 players who can.
// The TV show data is used to calculate this sum.
static void CalculateDaycareMailRandSum(const u8 *src)
{
    u8 sum;
    s32 i;

    sum = 0;
    for (i = 0; i < 256; i++)
        sum += src[i];

    sDaycareMailRandSum = sum;
}

static u8 GetDaycareMailRandSum(void)
{
    return sDaycareMailRandSum;
}

static void ReceiveDaycareMailData(u8 unused1, size_t recordSize, u8 multiplayerId, u8 unused2)
{

}


static void ReceiveGiftItem(u16 *item, u8 multiplayerId)
{
    if (multiplayerId != 0 && *item != ITEM_NONE && GetPocketByItemId(*item) == POCKET_KEY_ITEMS)
    {
        if (!CheckBagHasItem(*item, 1) && !CheckPCHasItem(*item, 1) && AddBagItem(*item, 1))
        {
            VarSet(VAR_TEMP_1, *item);
            StringCopy(gStringVar1, gLinkPlayers[0].name);
            if (*item == ITEM_EON_TICKET)
                FlagSet(FLAG_ENABLE_SHIP_SOUTHERN_ISLAND);
        }
        else
        {
            VarSet(VAR_TEMP_1, ITEM_NONE);
        }
    }
}

static void Task_DoRecordMixing(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    switch (task->tState)
    {
    case 0:
        task->tState++;
        break;
    case 1:
        if (Link_AnyPartnersPlayingRubyOrSapphire())
            task->tState++;
        else
            task->tState = 6;
        break;
    case 2:
        // Mixing Ruby/Sapphire records.
        SetContinueGameWarpStatusToDynamicWarp();
        WriteSaveBlock2();
        task->tState++;
        break;
    case 3:
        if (WriteSaveBlock1Sector())
        {
            ClearContinueGameWarpStatus2();
            task->tState = 4;
            task->data[1] = 0;
        }
        break;
    case 4: // Wait 10 frames
        if (++task->data[1] > 10)
        {
            SetCloseLinkCallback();
            task->tState++;
        }
        break;
    case 5:
        // Finish mixing Ruby/Sapphire records
        if (gReceivedRemoteLinkPlayers == FALSE)
            DestroyTask(taskId);
        break;

    // Mixing Emerald records.
    case 6:
        if (!Rfu_SetLinkRecovery(FALSE))
        {
            CreateTask(Task_LinkFullSave, 5);
            task->tState++;
        }
        break;
    case 7: // wait for Task_LinkFullSave to finish.
        if (!FuncIsActiveTask(Task_LinkFullSave))
        {
            if (gWirelessCommType)
            {
                Rfu_SetLinkRecovery(TRUE);
                task->tState = 8;
            }
            else
            {
                task->tState = 4;
            }
        }
        break;
    case 8:
        SetLinkStandbyCallback();
        task->tState++;
        break;
    case 9:
        if (IsLinkTaskFinished())
            DestroyTask(taskId);
        break;
    }
}

static void GetSavedApprentices(u8 unused1, u8 unused2)
{

}

void GetPlayerHallRecords(u8 unused)
{

}

static bool32 IsApprenticeAlreadySaved(u8 unused1, u8 unused2)
{

}

static void ReceiveApprenticeData(u8 unused, size_t recordSize, u32 multiplayerId)
{

}

static void GetNewHallRecords(u8 unused, void *records, size_t recordSize, u32 multiplayerId, s32 linkPlayerCount)
{

}

static void FillWinStreakRecords1P(u8 unused1, u8 unused2)
{

}

static void FillWinStreakRecords2P(u8 unused1, u8 unused2)
{

}

static void SaveHighestWinStreakRecords(u8 unused)
{

}

static void ReceiveRankingHallRecords(u8 unused, size_t recordSize, u32 multiplayerId)
{

}

static void GetRecordMixingDaycareMail(u8 unused)
{

}

static void SanitizeDaycareMailForRuby(u8 unused)
{

}

static void SanitizeRubyBattleTowerRecord(struct RSBattleTowerRecord *src)
{

}

static void SanitizeEmeraldBattleTowerRecord(struct EmeraldBattleTowerRecord *dst)
{
    s32 i;

    for (i = 0; i < MAX_FRONTIER_PARTY_SIZE; i++)
    {
        struct BattleTowerPokemon *towerMon = &dst->party[i];
        if (towerMon->species != SPECIES_NONE)
            StripExtCtrlCodes(towerMon->nickname);
    }

    CalcEmeraldBattleTowerChecksum(dst);
}
