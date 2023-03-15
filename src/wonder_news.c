#include "global.h"
#include "mystery_gift.h"
#include "random.h"
#include "event_data.h"
#include "wonder_news.h"
#include "constants/items.h"

// Every 4th reward for sending Wonder News to a link partner is a "big" reward.
#define MAX_SENT_REWARD 4

// Only up to 5 rewards can be received in a short period. After this the player
// must take 500 steps before any more rewards can be received.
#define MAX_REWARD 5

static u32 GetRewardItem(void);
static u32 GetRewardType(void);
static void IncrementRewardCounter(void);
static void IncrementSentRewardCounter(void);
static void ResetSentRewardCounter(void);

void WonderNews_SetReward(u32 newsType)
{

}

void WonderNews_Reset(void)
{

}

// Only used in FRLG
void WonderNews_IncrementStepCounter(void)
{

}

// Only used in FRLG
u16 WonderNews_GetRewardInfo(void)
{

}

static u32 GetRewardItem(void)
{

}

static void ResetSentRewardCounter(void)
{

}

// Track number of times a reward was received (or attmepted to receive) for sending Wonder News to a link partner.
static void IncrementSentRewardCounter(void)
{

}

static void IncrementRewardCounter(void)
{

}

static u32 GetRewardType(void)
{

}
