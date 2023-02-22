#include "global.h"
#include "main.h"
#include "overworld.h"
#include "fldeff.h"
#include "field_specials.h"
#include "pokeblock.h"
#include "event_data.h"
#include "script.h"
#include "random.h"
#include "string_util.h"
#include "item.h"
#include "constants/items.h"
#include "item_menu.h"
#include "text.h"
#include "easy_chat.h"
#include "lilycove_lady.h"
#include "contest.h"
#include "strings.h"
#include "constants/lilycove_lady.h"

#include "data/lilycove_lady.h"

static void InitLilycoveQuizLady(void);
static void InitLilycoveFavorLady(void);
static void InitLilycoveContestLady(void);
static void ResetQuizLadyForRecordMix(void);
static void ResetFavorLadyForRecordMix(void);
static void ResetContestLadyForRecordMix(void);
static u8 BufferQuizAuthorName(void);
static bool8 IsQuizTrainerIdNotPlayer(void);
static u8 GetPlayerNameLength(const u8 *);

extern EWRAM_DATA u16 gSpecialVar_ItemId;

u8 GetLilycoveLadyId(void)
{

}

void SetLilycoveLadyGfx(void)
{

}

void InitLilycoveLady(void)
{
    u16 id = ((gSaveBlock2Ptr->playerTrainerId[1] << 8) | gSaveBlock2Ptr->playerTrainerId[0]);
    id %= 6;
    id >>= 1;
    switch (id)
    {
    case LILYCOVE_LADY_QUIZ:
        InitLilycoveQuizLady();
        break;
    case LILYCOVE_LADY_FAVOR:
        InitLilycoveFavorLady();
        break;
    case LILYCOVE_LADY_CONTEST:
        InitLilycoveContestLady();
        break;
    }
}

void ResetLilycoveLadyForRecordMix(void)
{
    switch (GetLilycoveLadyId())
    {
    case LILYCOVE_LADY_QUIZ:
        ResetQuizLadyForRecordMix();
        break;
    case LILYCOVE_LADY_FAVOR:
        ResetFavorLadyForRecordMix();
        break;
    case LILYCOVE_LADY_CONTEST:
        ResetContestLadyForRecordMix();
        break;
    }
}

// Unused
void InitLilycoveLadyRandomly(void)
{
    u8 lady = Random() % LILYCOVE_LADY_COUNT;

    switch (lady)
    {
    case LILYCOVE_LADY_QUIZ:
        InitLilycoveQuizLady();
        break;
    case LILYCOVE_LADY_FAVOR:
        InitLilycoveFavorLady();
        break;
    case LILYCOVE_LADY_CONTEST:
        InitLilycoveContestLady();
        break;
    }
}

void Script_GetLilycoveLadyId(void)
{
    gSpecialVar_Result = GetLilycoveLadyId();
}

static u8 GetNumAcceptedItems(const u16 *itemsArray)
{
    u8 numItems;

    for (numItems = 0; *itemsArray != ITEM_NONE; numItems++, itemsArray++);
    return numItems;
}

static void FavorLadyPickFavorAndBestItem(void)
{

}

static void InitLilycoveFavorLady(void)
{

}

static void ResetFavorLadyForRecordMix(void)
{

}

u8 GetFavorLadyState(void)
{

}

static const u8 *GetFavorLadyRequest(u8 idx)
{
    return sFavorLadyRequests[idx];
}

void BufferFavorLadyRequest(void)
{

}

bool8 HasAnotherPlayerGivenFavorLadyItem(void)
{

}

static void BufferItemName(u8 *dest, u16 itemId)
{
    StringCopy(dest, ItemId_GetName(itemId));
}

void BufferFavorLadyItemName(void)
{

}

static void SetFavorLadyPlayerName(const u8 *src, u8 *dest)
{
    memset(dest, EOS, PLAYER_NAME_LENGTH + 1);
    StringCopy_PlayerName(dest, src);
}

void BufferFavorLadyPlayerName(void)
{

}

// Only used to determine if a record-mixed player had given her an item she liked
bool8 DidFavorLadyLikeItem(void)
{

}

void Script_FavorLadyOpenBagMenu(void)
{
    FavorLadyOpenBagMenu();
}

static bool8 DoesFavorLadyLikeItem(u16 itemId)
{

}

bool8 Script_DoesFavorLadyLikeItem(void)
{
    return DoesFavorLadyLikeItem(gSpecialVar_ItemId);
}

bool8 IsFavorLadyThresholdMet(void)
{

}

static void FavorLadyBufferPrizeName(u16 prize)
{
    BufferItemName(gStringVar2, prize);
}

u16 FavorLadyGetPrize(void)
{

}

void SetFavorLadyState_Complete(void)
{

}

void FieldCallback_FavorLadyEnableScriptContexts(void)
{
    ScriptContext_Enable();
}

static void QuizLadyPickQuestion(void)
{

}

static void InitLilycoveQuizLady(void)
{

}

static void ResetQuizLadyForRecordMix(void)
{

}

u8 GetQuizLadyState(void)
{

}

u8 GetQuizAuthor(void)
{

}

static u8 BufferQuizAuthorName(void)
{

}

static bool8 IsQuizTrainerIdNotPlayer(void)
{

}

static u8 GetPlayerNameLength(const u8 *playerName)
{
    u8 len;
    const u8 *ptr;

    for (len = 0, ptr = playerName; *ptr != EOS; len++, ptr++);
    return len;
}

void BufferQuizPrizeName(void)
{

}

bool8 BufferQuizAuthorNameAndCheckIfLady(void)
{

}

bool8 IsQuizLadyWaitingForChallenger(void)
{

}

void QuizLadyGetPlayerAnswer(void)
{
    ShowEasyChatScreen();
}

bool8 IsQuizAnswerCorrect(void)
{

}

void BufferQuizPrizeItem(void)
{

}

void SetQuizLadyState_Complete(void)
{

}

void SetQuizLadyState_GivePrize(void)
{

}

void ClearQuizLadyPlayerAnswer(void)
{

}

void Script_QuizLadyOpenBagMenu(void)
{
    QuizLadyOpenBagMenu();
}

void QuizLadyPickNewQuestion(void)
{

}

void ClearQuizLadyQuestionAndAnswer(void)
{

}

void QuizLadySetCustomQuestion(void)
{
    gSpecialVar_0x8004 = EASY_CHAT_TYPE_QUIZ_SET_QUESTION;
    ShowEasyChatScreen();
}

void QuizLadyTakePrizeForCustomQuiz(void)
{
    RemoveBagItem(gSpecialVar_ItemId, 1);
}

void QuizLadyRecordCustomQuizData(void)
{

}

void QuizLadySetWaitingForChallenger(void)
{

}

void BufferQuizCorrectAnswer(void)
{

}


void FieldCallback_QuizLadyEnableScriptContexts(void)
{
    ScriptContext_Enable();
}

void QuizLadyClearQuestionForRecordMix(u8 unused)
{

}

static void ResetContestLadyContestData(void)
{

}

static void InitLilycoveContestLady(void)
{

}

static void ResetContestLadyForRecordMix(void)
{

}

static void ContestLadySavePlayerNameIfHighSheen(u8 sheen)
{

}

bool8 GivePokeblockToContestLady(struct Pokeblock *pokeblock)
{

}

static void BufferContestLadyCategoryAndMonName(u8 *category, u8 *nickname)
{

}

void BufferContestLadyMonName(u8 *category, u8 *nickname)
{

}

void BufferContestLadyPlayerName(u8 *dest)
{

}

void BufferContestLadyLanguage(u8 *dest)
{

}

void BufferContestName(u8 *dest, u8 category)
{
    StringCopy(dest, sContestNames[category]);
}

// Used by the Contest Lady's TV show to determine how well she performed
u8 GetContestLadyPokeblockState(void)
{

}


bool8 HasPlayerGivenContestLadyPokeblock(void)
{

}

bool8 ShouldContestLadyShowGoOnAir(void)
{

}

void Script_BufferContestLadyCategoryAndMonName(void)
{
    BufferContestLadyCategoryAndMonName(gStringVar2, gStringVar1);
}

void OpenPokeblockCaseForContestLady(void)
{
    OpenPokeblockCase(PBLOCK_CASE_GIVE, CB2_ReturnToField);
}

void SetContestLadyGivenPokeblock(void)
{

}

void GetContestLadyMonSpecies(void)
{

}

u8 GetContestLadyCategory(void)
{

}
