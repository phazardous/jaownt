#include "server.h"
#include "qcommon/cm_public.h"

/*
=======================
SV_WriteMinigameStateToClient
=======================
*/

static void SV_WriteMinigameStateToClient( client_t *client, int minigame_index ) {
	byte msg_buf[MAX_MSGLEN];
	msg_t msg;
	MSG_Init ( &msg, msg_buf, sizeof(msg_buf) );
	
	MSG_WriteLong( &msg, client->lastClientCommand );
	SV_UpdateServerCommandsToClient( client, &msg );
	
	MSG_WriteByte ( &msg, svc_minigamestate );
	MSG_WriteShort( &msg, minigame_index );
	MSG_WriteBits( &msg, sv.minigamestates[minigame_index].active, 1 );
	MSG_WriteString( &msg, sv.minigamestates[minigame_index].type );
	MSG_WriteLong( &msg, sv.minigamestates[minigame_index].data_size );
	if (sv.minigamestates[minigame_index].data_size > 0) {
		MSG_WriteData( &msg, sv.minigamestates[minigame_index].data, sv.minigamestates[minigame_index].data_size );
	}
	SV_SendMessageToClient( &msg, client );
}


/*
=======================
SV_UpdateMinigameState
=======================
*/

void SV_UpdateMinigameState( int minigame_index ) {
	int i;
	client_t * c;
	for ( i = 0, c = svs.clients ; i < sv_maxclients->integer ; i++, c++ ) {
		if (!c->state) {
			continue;
		}
		SV_WriteMinigameStateToClient( c, minigame_index );
	}
}
