#include "bg_public.h"

//================================================================
//----------------------------------------------------------------
//================================================================



//================================================================
//----------------------------------------------------------------
//================================================================

typedef struct minigame_tictactoe_data_s {
	char board [3][3];
} minigame_tictactoe_data_t;

void minigame_tictactoe_new(minigameState_t * state) {
	
}

qboolean minigame_tictactoe_cmdparse(minigameState_t * state, char const * cmd) {
	COM_BeginParseSession("tic-tac-toe cmd");
	int x, y, v;
	if (COM_ParseInt(&cmd, &x) || x < 0 || x > 2) {
		return qfalse;
	}
	if (COM_ParseInt(&cmd, &y) || y < 0 || y > 2) {
		return qfalse;
	}
	if (COM_ParseInt(&cmd, &v) || v < 0 || v > 2) {
		return qfalse;
	}
	((minigame_tictactoe_data_t *)state->data)->board[x][y] = v;
	return qtrue;
}

char const * minigame_tictactoe_print(minigameState_t * state) {
	minigame_tictactoe_data_t * sdat = state->data;
	return va( "%c|%c|%c\n-+-+-\n%c|%c|%c\n-+-+-\n%c|%c|%c",
		sdat->board[0][0] == 0 ? ' ' : sdat->board[0][0] == 1 ? 'X' : 'O',
		sdat->board[0][1] == 0 ? ' ' : sdat->board[0][1] == 1 ? 'X' : 'O',
		sdat->board[0][2] == 0 ? ' ' : sdat->board[0][2] == 1 ? 'X' : 'O',
		sdat->board[1][0] == 0 ? ' ' : sdat->board[1][0] == 1 ? 'X' : 'O',
		sdat->board[1][1] == 0 ? ' ' : sdat->board[1][1] == 1 ? 'X' : 'O',
		sdat->board[1][2] == 0 ? ' ' : sdat->board[1][2] == 1 ? 'X' : 'O',
		sdat->board[2][0] == 0 ? ' ' : sdat->board[2][0] == 1 ? 'X' : 'O',
		sdat->board[2][1] == 0 ? ' ' : sdat->board[2][1] == 1 ? 'X' : 'O',
		sdat->board[2][2] == 0 ? ' ' : sdat->board[2][2] == 1 ? 'X' : 'O'
	);
}

//================================================================
//----------------------------------------------------------------
//================================================================

minigameState_t minigames[MAX_MINIGAMES];

minigameType_t minigameTypes [] = {
	
	{"tic-tac-toe", sizeof(minigame_tictactoe_data_t), minigame_tictactoe_new, minigame_tictactoe_cmdparse, minigame_tictactoe_print},
	
};
size_t minigameTypesNum = ARRAY_LEN(minigameTypes);

minigameType_t * BG_FindMinigame(char const * type) {
	if (!type) return NULL;
	for (size_t i = 0; i < minigameTypesNum; i++) {
		if (!strncmp(minigameTypes[i].name, type, MAX_MINIGAME_TYPE_SIZE)) return minigameTypes + i;
	}
	return NULL;
}

//================================================================
//----------------------------------------------------------------
//================================================================

void BG_InitMinigames() {
	memset(minigames, 0, MAX_MINIGAMES * sizeof(minigameState_t));
}

void BG_ClearMinigames() {
	for (int i = 0; i < MAX_MINIGAMES; i++) {
		if (minigames[i].data) free(minigames[i].data);
	}
	memset(minigames, 0, MAX_MINIGAMES * sizeof(minigameState_t));
}

void BG_MinigameReset(int index) {
	if (minigames[index].data) free(minigames[index].data);
	memset(minigames + index, 0, sizeof(minigameState_t));
}

void BG_MinigameNew(int index, minigameType_t * type) {
	minigameState_t * mg = minigames + index;
	mg->active = qtrue;
	strncpy(mg->type, type->name, MAX_MINIGAME_TYPE_SIZE);
	mg->data_size = type->data_size;
	mg->data = realloc(mg->data, type->data_size);
	memset(mg->data, 0, type->data_size);
	type->gameNew(mg);
}

//================================================================
//----------------------------------------------------------------
//================================================================
