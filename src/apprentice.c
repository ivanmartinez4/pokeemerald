#include "global.h"
#include "apprentice.h"
#include "battle.h"
#include "battle_tower.h"
#include "data.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "field_player_avatar.h"
#include "international_string_util.h"
#include "item.h"
#include "item_menu.h"
#include "main.h"
#include "malloc.h"
#include "menu.h"
#include "new_game.h"
#include "party_menu.h"
#include "random.h"
#include "script.h"
#include "script_menu.h"
#include "sound.h"
#include "string_util.h"
#include "strings.h"
#include "task.h"
#include "text.h"
#include "constants/battle_frontier.h"
#include "constants/items.h"
#include "constants/songs.h"
#include "constants/trainers.h"
#include "constants/moves.h"

/* Summary of Apprentice, because (as of writing at least) it's not very well documented online
 *
 * ## Basic info
 * In the Battle Tower lobby there is an NPC which asks to be taught by the player
 * They can be any 1 of 16 NPC trainers, each with their own name, class, and set of possible party species
 * They ask the player a series of questions once per day, and eventually depart the lobby to be replaced by a new Apprentice
 *
 * ## Initial Questions
 * The first question they always ask is a request to be taught, which cannot be rejected
 * The second question (which follows immediately after) is whether they should participate in Battle Tower Lv 50 or Open Lv
 * After these opening questions they always ask the player to choose between 2 mons, which they repeat 3 times
 *
 * ## Random Questions
 * After choosing 3 mons for them, the Apprentice will randomly ask between 1 and 8 questions of 4 different types, as follows
 * - Asking which mon to lead with, which they will only ask at most once
 * - Asking which move a mon should use, which they will ask at most 5 times
 * - Asking what held item to give to a mon, which they will ask at most 3 times (once for each mon)
 * - Asking what they should say when they win a battle, which will always be their final question before departing
 *
 * ## After departing
 * After telling them what they should say when they win a battle they will leave the lobby for a final time
 * They will then be replaced by a new random Apprentice (they can repeat)
 * Up to 4 old Apprentices are saved and can be encountered (or partnered with) during challenges of the mode they were told to battle in
 * They can also be record mixed to and from other Emerald games
 * Old/record mixed Apprentices are stored in struct Apprentice apprentices of SaveBlock2
 *   and the current Apprentice is stored in struct PlayersApprentice playerApprentice of SaveBlock2
 */

#define PLAYER_APPRENTICE 0
#define CURRENT_QUESTION_NUM  0

struct ApprenticePartyMovesData
{
    u8 moveCounter;
    u16 moves[MULTI_PARTY_SIZE][NUM_WHICH_MOVE_QUESTIONS];
    u8 moveSlots[MULTI_PARTY_SIZE][NUM_WHICH_MOVE_QUESTIONS];
};

struct ApprenticeQuestionData
{
    u16 speciesId;
    u16 altSpeciesId;
    u16 moveId1;
    u16 moveId2;
};

// IWRAM common
struct ApprenticePartyMovesData *gApprenticePartyMovesData;
struct ApprenticeQuestionData *gApprenticeQuestionData;
void (*gApprenticeFunc)(void);

// This file's functions.
static u16 GetRandomAlternateMove(u8 monId);
static bool8 TrySetMove(u8 monId, u16 moveId);
static void CreateChooseAnswerTask(bool8 noBButton, u8 itemsCount, u8 windowId);
static u8 CreateAndShowWindow(u8 left, u8 top, u8 width, u8 height);
static void RemoveAndHideWindow(u8 windowId);
static void ExecuteFuncAfterButtonPress(void (*func)(void));

static void Script_GivenApprenticeLvlMode(void);
static void Script_SetApprenticeLvlMode(void);
static void ShuffleApprenticeSpecies(void);
static void Script_SetRandomQuestionData(void);
static void IncrementQuestionsAnswered(void);
static void Script_CreateApprenticeMenu(void);
static void Script_PrintApprenticeMessage(void);
static void Script_ResetPlayerApprentice(void);
static void GetShouldCheckApprenticeGone(void);
static void ApprenticeGetQuestion(void);
static void GetNumApprenticePartyMonsAssigned(void);
static void SetApprenticePartyMon(void);
static void InitQuestionData(void);
static void FreeQuestionData(void);
static void ApprenticeBufferString(void);
static void SetApprenticeMonMove(void);
static void SetLeadApprenticeMon(void);
static void Script_ApprenticeOpenBagMenu(void);
static void TrySetApprenticeHeldItem(void);
static void SaveApprentice(void);
static void SetPlayerApprenticeTrainerGfxId(void);
static void GetShouldApprenticeLeave(void);
static void ShiftSavedApprentices(void);

#include "data/battle_frontier/apprentice.h"

void BufferApprenticeChallengeText(u8 saveApprenticeId)
{

}

void Apprentice_ScriptContext_Enable(void)
{
    ScriptContext_Enable();
}

void ResetApprenticeStruct(u8 unused)
{

}

void ResetAllApprenticeData(void)
{

}

static bool8 GivenApprenticeLvlMode(void)
{

}

static void SetApprenticeId(void)
{

}

static void SetPlayersApprenticeLvlMode(u8 mode)
{

}

static void ShuffleApprenticeSpecies(void)
{

}

// Pick one of the Apprentice's mons to ask the question about
// Picking a move chooses a random mon, picking a held item is sequential (so that none are repeated)
static u8 GetMonIdForQuestion(u8 questionId, u8 *party, u8 *partySlot)
{
    u8 i, count;
    u8 monId = 0;

    if (questionId == QUESTION_ID_WHICH_MOVE)
    {
        do
        {
            monId = Random() % (MULTI_PARTY_SIZE);
            for (count = 0, i = 0; i < NUM_WHICH_MOVE_QUESTIONS; i++)
            {
                if (gApprenticePartyMovesData->moves[monId][i] != MOVE_NONE)
                    count++;
            }
        } while (count > MULTI_PARTY_SIZE);
    }
    else if (questionId == QUESTION_ID_WHAT_ITEM)
    {
        monId = party[*partySlot];
        (*partySlot)++;
    }

    return monId;
}

// Sets the random order and data for the remaining questions after the initial "choose mon" questions
static void SetRandomQuestionData(void)
{

}

// Get the second move choice for the "Which move" question
// Unlike the first move choice, this can be either a level up move or a TM/HM move
static u16 GetRandomAlternateMove(u8 monId)
{

}

static bool8 TrySetMove(u8 monId, u16 moveId)
{

}

static void GetLatestLearnedMoves(u16 species, u16 *moves)
{

}

// Get the level up move or previously suggested move to be the first move choice
// Compare to GetRandomAlternateMove, which gets the move that will be the second choice
static u16 GetDefaultMove(u8 monId, u8 speciesArrayId, u8 moveSlot)
{

}

static void SaveApprenticeParty(u8 numQuestions)
{

}

static void CreateApprenticeMenu(u8 menu)
{

}

#define tNoBButton data[4]
#define tWrapAround data[5]
#define tWindowId data[6]

static void Task_ChooseAnswer(u8 taskId)
{
    s8 input;
    s16 *data = gTasks[taskId].data;

    if (!tWrapAround)
        input = Menu_ProcessInputNoWrap();
    else
        input = Menu_ProcessInput();

    switch (input)
    {
    case MENU_NOTHING_CHOSEN:
        return;
    case MENU_B_PRESSED:
        if (tNoBButton)
            return;

        PlaySE(SE_SELECT);
        gSpecialVar_Result = 0x7F;
        break;
    default:
        gSpecialVar_Result = input;
        break;
    }

    RemoveAndHideWindow(tWindowId);
    DestroyTask(taskId);
    ScriptContext_Enable();
}

static u8 CreateAndShowWindow(u8 left, u8 top, u8 width, u8 height)
{
    u8 windowId;
    struct WindowTemplate winTemplate = CreateWindowTemplate(0, left + 1, top + 1, width, height, 15, 100);

    windowId = AddWindow(&winTemplate);
    PutWindowTilemap(windowId);
    CopyWindowToVram(windowId, COPYWIN_FULL);
    return windowId;
}

static void RemoveAndHideWindow(u8 windowId)
{
    ClearStdWindowAndFrameToTransparent(windowId, TRUE);
    RemoveWindow(windowId);
}

static void CreateChooseAnswerTask(bool8 noBButton, u8 answers, u8 windowId)
{
    u8 taskId = CreateTask(Task_ChooseAnswer, 80);
    gTasks[taskId].tNoBButton = noBButton;

    if (answers > 3)
        gTasks[taskId].tWrapAround = TRUE;
    else
        gTasks[taskId].tWrapAround = FALSE;

    gTasks[taskId].tWindowId = windowId;
}

#undef tNoBButton
#undef tWrapAround
#undef tWindowId

void CallApprenticeFunction(void)
{

}

static void Script_ResetPlayerApprentice(void)
{

}

static void Script_GivenApprenticeLvlMode(void)
{

}

// VAR_0x8005 is 1 + the selection value from the multichoice APPRENTICE_ASK_WHICH_LEVEL
// i.e. APPRENTICE_LVL_MODE_50 or APPRENTICE_LVL_MODE_OPEN
static void Script_SetApprenticeLvlMode(void)
{

}

static void Script_SetRandomQuestionData(void)
{
    SetRandomQuestionData();
}

static void IncrementQuestionsAnswered(void)
{

}

// The first 3 questions answered after meeting the Apprentice are always selecting party mons
//  after which this is never called
static void GetNumApprenticePartyMonsAssigned(void)
{

}

static void Script_CreateApprenticeMenu(void)
{
    CreateApprenticeMenu(gSpecialVar_0x8005);
}

static void Task_WaitForPrintingMessage(u8 taskId)
{
    if (!RunTextPrintersAndIsPrinter0Active())
    {
        DestroyTask(taskId);
        if (gSpecialVar_0x8005)
            ExecuteFuncAfterButtonPress(ScriptContext_Enable);
        else
            ScriptContext_Enable();
    }
}

static void PrintApprenticeMessage(void)
{

}

static void Script_PrintApprenticeMessage(void)
{
    LockPlayerFieldControls();
    FreezeObjectEvents();
    PlayerFreeze();
    StopPlayerAvatar();
    DrawDialogueFrame(0, TRUE);
    PrintApprenticeMessage();
}

static void ApprenticeGetQuestion(void)
{

}

// gSpecialVar_0x8005 is 0 or 1 for the mon selection (0 is already on the team)
// gSpecialVar_0x8006 is 0-2 for the number of party mons selected so far
static void SetApprenticePartyMon(void)
{

}

// gSpecialVar_0x8005 is 0 or 1 for the move selection
// Selection 0 is implicitly the default move assigned
static void SetApprenticeMonMove(void)
{

}

static void InitQuestionData(void)
{

}

static void FreeQuestionData(void)
{
    FREE_AND_SET_NULL(gApprenticeQuestionData);
}

static void ApprenticeBufferString(void)
{

}

static void SetLeadApprenticeMon(void)
{

}

static void Script_ApprenticeOpenBagMenu(void)
{
    ApprenticeOpenBagMenu();
}

static void TrySetApprenticeHeldItem(void)
{

}

static void ShiftSavedApprentices(void)
{

}

// Apprentice is always saved in the first slot. Pre-existing Apprentices are moved by ShiftSavedApprentices
static void SaveApprentice(void)
{

}

static void SetPlayerApprenticeTrainerGfxId(void)
{

}

// Both of the below functions may have been dummied / used for debug
// In all cases theres a conditional for VAR_0x8004 right after the call to these functions
static void GetShouldCheckApprenticeGone(void)
{
    gSpecialVar_0x8004 = TRUE;
}

static void GetShouldApprenticeLeave(void)
{
    gSpecialVar_0x8004 = TRUE;
}

const u8 *GetApprenticeNameInLanguage(u32 apprenticeId, s32 language)
{

}

static void Task_ExecuteFuncAfterButtonPress(u8 taskId)
{
    if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
    {
        gApprenticeFunc = (void *)(u32)(((u16)gTasks[taskId].data[0] | (gTasks[taskId].data[1] << 16)));
        gApprenticeFunc();
        DestroyTask(taskId);
    }
}

static void ExecuteFuncAfterButtonPress(void (*func)(void))
{
    u8 taskId = CreateTask(Task_ExecuteFuncAfterButtonPress, 1);
    gTasks[taskId].data[0] = (u32)(func);
    gTasks[taskId].data[1] = (u32)(func) >> 16;
}
