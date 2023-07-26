#include <a_samp>
#include <YSI_Visual\y_commands>

#include "text.inc"

main() 
{
	
	return;
}

public OnPlayerSpawn(playerid)
{
	
	return 1;
}

CMD:pm(playerid, params[])
{
	if(!Message_CanPlayerSendPrivate(playerid))
	{
		return SendClientMessage(playerid, -1, "not allowed");
	}
	Message_SendPrivate(playerid, 0, -1, "HeyHeyHey");
	return 1;
}

CMD:allow(playerid, params[])
{
	if(!Message_CanPlayerSendPrivate(playerid))
	{
		return Message_AllowPlayerPrivate(playerid, true);
	}
	return Message_AllowPlayerPrivate(playerid, false);
}

CMD:messagelevel(playerid, params[])
{
	return Message_Send(playerid, MESSAGE_ANNOUNCEMENT, "Test %d", 35);
}
 
CMD:messageleveltoall(playerid, params[])
{
	Message_SendToAll(MESSAGE_ERROR, "Test error %d %d", 44, 4444);
	return COMMAND_OK;
}

public OnPlayerText(playerid, text[])
{
	new output[128 + 1]; // or whatever you want
	Message_ParseMentions(output, text);
	SendClientMessageToAll(0xFFFFFFAA, output);
	return 0;
}

public OnPlayerMessageMentioned(playerid, senderid)
{
    if (mentionSound)
    {
        // Play "DING" message
        PlayerPlaySound(playerid, 17802, 0.0, 0.0, 0.0);
    }
}