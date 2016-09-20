#include "bg_public.h"

minigameState_t bg_minigames[MAX_MINIGAMES];

void BG_InitMinigames() {
	memset(bg_minigames, 0, MAX_MINIGAMES * sizeof(minigameState_t));
}

void BG_ClearMinigames() {
	for (int i = 0; i < MAX_MINIGAMES; i++) {
		if (bg_minigames[i].data) free(bg_minigames[i].data);
	}
	memset(bg_minigames, 0, MAX_MINIGAMES * sizeof(minigameState_t));
}
