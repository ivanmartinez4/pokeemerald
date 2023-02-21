#include "global.h"
#include "malloc.h"
#include "script.h"
#include "mystery_gift.h"
#include "mystery_gift_server.h"
#include "mystery_gift_link.h"

enum {
    FUNC_INIT,
    FUNC_DONE,
    FUNC_RECV,
    FUNC_SEND,
    FUNC_RUN,
};

EWRAM_DATA static struct MysteryGiftServer * sServer = NULL;

static void MysteryGiftServer_Init(struct MysteryGiftServer *, const void *, u32, u32);
static void MysteryGiftServer_Free(struct MysteryGiftServer *);
static u32 MysteryGiftServer_CallFunc(struct MysteryGiftServer *);

extern const struct MysteryGiftServerCmd gMysteryGiftServerScript_SendWonderNews[];
extern const struct MysteryGiftServerCmd gMysteryGiftServerScript_SendWonderCard[];

void MysterGiftServer_CreateForNews(void)
{

}

void MysterGiftServer_CreateForCard(void)
{

}

u32 MysterGiftServer_Run(u16 * endVal)
{

}

static void MysteryGiftServer_Init(struct MysteryGiftServer * svr, const void * script, u32 sendPlayerId, u32 recvPlayerId)
{

}

static void MysteryGiftServer_Free(struct MysteryGiftServer * svr)
{

}

static void MysteryGiftServer_InitSend(struct MysteryGiftServer * svr, u32 ident, const void * src, u32 size)
{

}

// Given the command pointer parameter and the 'default' normal data.
// If the command's pointer is not empty use that as the send data, otherwise use the default.
static const void * MysteryGiftServer_GetSendData(const void * dynamicData, const void * defaultData)
{

}

static u32 MysteryGiftServer_Compare(const void * a, const void * b)
{

}

static u32 Server_Init(struct MysteryGiftServer * svr)
{

}

static u32 Server_Done(struct MysteryGiftServer * svr)
{

}

static u32 Server_Recv(struct MysteryGiftServer * svr)
{

}

static u32 Server_Send(struct MysteryGiftServer * svr)
{

}

static u32 Server_Run(struct MysteryGiftServer * svr)
{

}

static u32 (*const sFuncTable[])(struct MysteryGiftServer *) = {

};

static u32 MysteryGiftServer_CallFunc(struct MysteryGiftServer * svr)
{

}
