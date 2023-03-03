#include "global.h"
#include "util.h"
#include "main.h"
#include "event_data.h"
#include "easy_chat.h"
#include "script.h"
#include "battle_tower.h"
#include "wonder_news.h"
#include "string_util.h"
#include "new_game.h"
#include "mystery_gift.h"
#include "constants/mystery_gift.h"

static EWRAM_DATA bool32 sStatsEnabled = FALSE;

static void ClearSavedWonderNewsMetadata(void);
static void ClearSavedWonderNews(void);
static void ClearSavedWonderCard(void);
static bool32 ValidateWonderNews(u8);
static bool32 ValidateWonderCard(u8);
static void ClearSavedWonderCardMetadata(void);
static void ClearSavedTrainerIds(void);
static void IncrementCardStatForNewTrainer(u32, u32, u32 *, int);

#define CALC_CRC(data) CalcCRC16WithTable((void *)&(data), sizeof(data))

void ClearMysteryGift(void)
{

}

struct WonderNews *GetSavedWonderNews(void)
{

}

struct WonderCard *GetSavedWonderCard(void)
{

}

struct WonderCardMetadata *GetSavedWonderCardMetadata(void)
{

}

struct WonderNewsMetadata *GetSavedWonderNewsMetadata(void)
{

}

u16 *GetQuestionnaireWordsPtr(void)
{

}

// Equivalent to ClearSavedWonderCardAndRelated, but nothing else to clear
void ClearSavedWonderNewsAndRelated(void)
{

}

bool32 SaveWonderNews(u8 unused)
{

}

bool32 ValidateSavedWonderNews(void)
{

}

static bool32 ValidateWonderNews(u8 unused)
{

}

bool32 IsSendingSavedWonderNewsAllowed(void)
{

}

static void ClearSavedWonderNews(void)
{

}

static void ClearSavedWonderNewsMetadata(void)
{

}

bool32 IsWonderNewsSameAsSaved(const u8 *news)
{

}

void ClearSavedWonderCardAndRelated(void)
{

}

bool32 SaveWonderCard(u8 unused)
{

}

bool32 ValidateSavedWonderCard(void)
{

}

static bool32 ValidateWonderCard(u8 unused)
{

}

bool32 IsSendingSavedWonderCardAllowed(void)
{

}

static void ClearSavedWonderCard(void)
{

}

static void ClearSavedWonderCardMetadata(void)
{

}

u16 GetWonderCardFlagID(void)
{

}

void DisableWonderCardSending(u8 unused)
{

}

static bool32 IsWonderCardFlagIDInValidRange(u16 flagId)
{

}

static const u16 sReceivedGiftFlags[] =
{

};

bool32 IsSavedWonderCardGiftNotReceived(void)
{

}

static int GetNumStampsInMetadata(u8 unused)
{

}

static bool32 IsStampInMetadata(u8 unused)
{

}

static bool32 ValidateStamp(const u16 *stamp)
{

}

static int GetNumStampsInSavedCard(void)
{

}

bool32 MysteryGift_TrySaveStamp(const u16 *stamp)
{

}

#define GAME_DATA_VALID_VAR 0x101
#define GAME_DATA_VALID_GIFT_TYPE_1 (1 << 2)
#define GAME_DATA_VALID_GIFT_TYPE_2 (1 << 9)

void MysteryGift_LoadLinkGameData(u8 unused, bool32 isWonderNews)
{

}

bool32 MysteryGift_ValidateLinkGameData(u8 unused, bool32 isWonderNews)
{

}

u32 MysteryGift_CompareCardFlags(const u16 *flagId, const void *unused)
{

}

// This is referenced by the Mystery Gift server, but the instruction it's referenced in is never used,
// so the return values here are never checked by anything.
u32 MysteryGift_CheckStamps(const u16 *stamp, const void *unused)
{

}

bool32 MysteryGift_DoesQuestionnaireMatch(u8 unused, const u16 *words)
{

}

static int GetNumStampsInLinkData(u8 unused)
{

}

u16 MysteryGift_GetCardStatFromLinkData(u8 unused,u32 stat)
{

}

static void IncrementCardStat(u32 statType)
{

}

u16 MysteryGift_GetCardStat(u32 stat)
{

}

void MysteryGift_DisableStats(void)
{

}

bool32 MysteryGift_TryEnableStatsByFlagId(u16 flagId)
{

}

void MysteryGift_TryIncrementStat(u32 stat, u32 trainerId)
{

}

static void ClearSavedTrainerIds(void)
{

}

// Returns TRUE if it's a new trainer id, FALSE if an existing one.
// In either case the given trainerId is saved in element 0
static bool32 RecordTrainerId(u32 trainerId, u32 *trainerIds, int size)
{
    int i, j;

    for (i = 0; i < size; i++)
    {
        if (trainerIds[i] == trainerId)
            break;
    }

    if (i == size)
    {
        // New trainer, shift array and insert new id at front
        for (j = size - 1; j > 0; j--)
            trainerIds[j] = trainerIds[j - 1];

        trainerIds[0] = trainerId;
        return TRUE;
    }
    else
    {
        // Existing trainer, shift back to old slot and move id to front
        for (j = i; j > 0; j--)
            trainerIds[j] = trainerIds[j - 1];

        trainerIds[0] = trainerId;
        return FALSE;
    }
}

static void IncrementCardStatForNewTrainer(u32 stat, u32 trainerId, u32 *trainerIds, int size)
{
    if (RecordTrainerId(trainerId, trainerIds, size))
        IncrementCardStat(stat);
}
