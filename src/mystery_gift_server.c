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
    u32 result;
    if (sServer == NULL)
        return SVR_RET_END;
    result = MysteryGiftServer_CallFunc(sServer);
    if (result == SVR_RET_END)
    {
        *endVal = sServer->param;
        MysteryGiftServer_Free(sServer);
        FREE_AND_SET_NULL(sServer);
    }
    return result;
}

static void MysteryGiftServer_Init(struct MysteryGiftServer * svr, const void * script, u32 sendPlayerId, u32 recvPlayerId)
{

}

static void MysteryGiftServer_Free(struct MysteryGiftServer * svr)
{
    Free(svr->card);
    Free(svr->news);
    Free(svr->recvBuffer);
    Free(svr->linkGameData);
}

static void MysteryGiftServer_InitSend(struct MysteryGiftServer * svr, u32 ident, const void * src, u32 size)
{

}

// Given the command pointer parameter and the 'default' normal data.
// If the command's pointer is not empty use that as the send data, otherwise use the default.
static const void * MysteryGiftServer_GetSendData(const void * dynamicData, const void * defaultData)
{
    if (dynamicData != NULL)
        return dynamicData;
    else
        return defaultData;
}

static u32 MysteryGiftServer_Compare(const void * a, const void * b)
{
    if (b < a)
        return 0;
    else if (b == a)
        return 1;
    else
        return 2;
}

static u32 Server_Init(struct MysteryGiftServer * svr)
{
    svr->funcId = FUNC_RUN;
    return SVR_RET_INIT;
}

static u32 Server_Done(struct MysteryGiftServer * svr)
{
    return SVR_RET_END;
}

static u32 Server_Recv(struct MysteryGiftServer * svr)
{
    if (MysteryGiftLink_Recv(&svr->link))
        svr->funcId = FUNC_RUN;
    return SVR_RET_ACTIVE;
}

static u32 Server_Send(struct MysteryGiftServer * svr)
{
    if (MysteryGiftLink_Send(&svr->link))
        svr->funcId = FUNC_RUN;
    return SVR_RET_ACTIVE;
}

static u32 Server_Run(struct MysteryGiftServer * svr)
{

}

static u32 (*const sFuncTable[])(struct MysteryGiftServer *) = {
    [FUNC_INIT] = Server_Init,
    [FUNC_DONE] = Server_Done,
    [FUNC_RECV] = Server_Recv,
    [FUNC_SEND] = Server_Send,
    [FUNC_RUN] = Server_Run
};

static u32 MysteryGiftServer_CallFunc(struct MysteryGiftServer * svr)
{
    u32 response;
    AGB_ASSERT(svr->funcId < ARRAY_COUNT(sFuncTable));
    response = sFuncTable[svr->funcId](svr);
    AGB_ASSERT(svr->funcId < ARRAY_COUNT(sFuncTable));
    return response;
}
