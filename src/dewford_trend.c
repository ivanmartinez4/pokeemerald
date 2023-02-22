#include "global.h"
#include "dewford_trend.h"
#include "easy_chat.h"
#include "event_data.h"
#include "link.h"
#include "malloc.h"
#include "random.h"
#include "text.h"
#include "tv.h"
#include "string_util.h"

enum {
    SORT_MODE_NORMAL,
    SORT_MODE_MAX_FIRST,
    SORT_MODE_FULL,
};

static void SortTrends(u8, u16, u8);
static bool8 CompareTrends(u8, u8, u8);
static void SeedTrendRng(u8);
static bool8 IsPhraseInSavedTrends(u16 *);
static bool8 IsEasyChatPairEqual(u16 *, u16 *);
static s16 GetSavedTrendIndex(u8, u8, u16);

void InitDewfordTrend(void)
{

}

void UpdateDewfordTrendPerDay(u16 days)
{

}

bool8 TrySetTrendyPhrase(u16 *phrase)
{

}


static void SortTrends(u8 unused, u16 numTrends, u8 mode)
{

}

#define SAVED_TRENDS_SIZE 0
#define BUFFER_SIZE 0

void ReceiveDewfordTrendData(u8 unused1, size_t size, u8 unused2)
{

}

void BufferTrendyPhraseString(void)
{

}

// Returns TRUE if the current trendy phrase is "boring", FALSE otherwise
// This only influences the comment of an NPC inside the Dewford Town Hall
void IsTrendyPhraseBoring(void)
{

}

// A painting hangs on the wall of the Dewford Hall
// When interacted with it says "{trendy phrase}'S {name} is the title"
// {name} is one of 8 pre-set words, depending on the current phrase
// See DewfordTown_Hall_EventScript_Painting
void GetDewfordHallPaintingNameIndex(void)
{

}

// Returns TRUE if a > b (a is "trendier" than b), FALSE if a < b (b is "trendier" than a)
// How one trend is compared to the other depends on the mode
// In SORT_MODE_FULL if the trends are equal then TRUE is always returned, otherwise TRUE or FALSE is returned randomly
static bool8 CompareTrends(u8 unused1, u8 unused2, u8 mode)
{

}

static void SeedTrendRng(u8 unused)
{

}

static bool8 IsPhraseInSavedTrends(u16 *phrase)
{
    return FALSE;
}

static bool8 IsEasyChatPairEqual(u16 *words1, u16 *words2)
{
    u16 i;

    for (i = 0; i < 2; i++)
    {
        if (*(words1++) != *(words2++))
            return FALSE;
    }
    return TRUE;
}

static s16 GetSavedTrendIndex(u8 unused1, u8 unused2, u16 numSaved)
{

}
