#include "global.h"
#include "util.h"
#include "main.h"
#include "event_data.h"
#include "easy_chat.h"
#include "script.h"
#include "battle_tower.h"
#include "string_util.h"
#include "new_game.h"
#include "mystery_gift.h"
#include "constants/mystery_gift.h"

static EWRAM_DATA bool32 sStatsEnabled = FALSE;

static void ClearSavedWonderNewsMetadata(void);
static void ClearSavedWonderNews(void);
static void ClearSavedWonderCard(void);
static bool32 ValidateWonderNews(const struct WonderNews *);
static bool32 ValidateWonderCard(const struct WonderCard *);
static void ClearSavedTrainerIds(void);
static void IncrementCardStatForNewTrainer(u32, u32, u32 *, int);

void ClearMysteryGift(void)
{

}

struct WonderNews *GetSavedWonderNews(void)
{

}

struct WonderCard *GetSavedWonderCard(void)
{

}

u16 *GetQuestionnaireWordsPtr(void)
{

}

// Equivalent to ClearSavedWonderCardAndRelated, but nothing else to clear
void ClearSavedWonderNewsAndRelated(void)
{

}

bool32 SaveWonderNews(const struct WonderNews *news)
{

}

bool32 ValidateSavedWonderNews(void)
{

}

static bool32 ValidateWonderNews(const struct WonderNews *news)
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

bool32 SaveWonderCard(const struct WonderCard *card)
{

}

bool32 ValidateSavedWonderCard(void)
{

}

static bool32 ValidateWonderCard(const struct WonderCard *card)
{

}

bool32 IsSendingSavedWonderCardAllowed(void)
{

}

static void ClearSavedWonderCard(void)
{

}

u16 GetWonderCardFlagID(void)
{

}

void DisableWonderCardSending(struct WonderCard *card)
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
    u16 value = GetWonderCardFlagID();
    if (!IsWonderCardFlagIDInValidRange(value))
        return FALSE;

    // If flag is set, player has received gift from this card
    if (FlagGet(sReceivedGiftFlags[value - WONDER_CARD_FLAG_OFFSET]) == TRUE)
        return FALSE;

    return TRUE;
}

static int GetNumStampsInMetadata(int size)
{

}

static bool32 IsStampInMetadata(const u16 *stamp, int maxStamps)
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

void MysteryGift_LoadLinkGameData(struct MysteryGiftLinkGameData *data, bool32 isWonderNews)
{

}

bool32 MysteryGift_ValidateLinkGameData(const struct MysteryGiftLinkGameData *data, bool32 isWonderNews)
{

}

u32 MysteryGift_CompareCardFlags(const u16 *flagId, const struct MysteryGiftLinkGameData *data, const void *unused)
{

}

// This is referenced by the Mystery Gift server, but the instruction it's referenced in is never used,
// so the return values here are never checked by anything.
u32 MysteryGift_CheckStamps(const u16 *stamp, const struct MysteryGiftLinkGameData *data, const void *unused)
{

}

bool32 MysteryGift_DoesQuestionnaireMatch(const struct MysteryGiftLinkGameData *data, const u16 *words)
{

}

static int GetNumStampsInLinkData(const struct MysteryGiftLinkGameData *data)
{

}

u16 MysteryGift_GetCardStatFromLinkData(const struct MysteryGiftLinkGameData *data, u32 stat)
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

}

static void IncrementCardStatForNewTrainer(u32 stat, u32 trainerId, u32 *trainerIds, int size)
{

}
