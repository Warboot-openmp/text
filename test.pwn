#include <a_samp>
#include <YSI_Visual\y_commands>

#include "text.inc"


main() 
{
	return;
}

new RandomMessage: message1;
new Channel: channeltest;

public OnGameModeInit()
{

	new RandomMessage: message = Message_AddRandom("Hej ti, kisa mi niz lice {FF33AA}pada{FFFFFF}.");
	message1 = Message_AddRandom("Hej ti, kisa mi niz lice {FF33AA}pada.");
	Message_SendRandomToAll(-1, 11000, 5);
	print("Testing");

	channeltest = ChatGroup_Create("Test Channel");

	return 1;
}


CMD:addto(playerid, params[])
{
	ChatGroup_AddMember(channeltest, playerid);
	return 1;
}

CMD:removefrom(playerid, params[])
{
	ChatGroup_RemoveMember(channeltest, playerid);
	return 1;
}

CMD:channelmsg(playerid, params[])
{
	ChatGroup_SendMessage(channeltest, 0xFFAAFFFF, "Text message");
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

CMD:pmset(playerid, params[])
{
	if(!Text_GetConfiguration(TEXT_CONFIG_CALL_OPRPM))
	{
		Text_SetConfiguration(TEXT_CONFIG_CALL_OPRPM, true);
	}
	else
	{
		Text_SetConfiguration(TEXT_CONFIG_CALL_OPRPM, false);
	}
	return COMMAND_OK;
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
	Message_ParseMention(output, text, .senderid = playerid, .len = 144);
	SendClientMessageToAll(-1, "{FFA500}%s: {FFFFFF}%s", ReturnPlayerName(playerid), output);
	return 1;
}

public OnPlayerMessageMentioned(playerid, senderid)
{
    // Play "DING" message
	SendClientMessage(playerid, -1, "AAAAAAAAAAAA");
    PlayerPlaySound(playerid, 17802, 0.0, 0.0, 0.0);
}

public OnPlayerReceivePrivateMessage(playerid, receiverid, string:text[])
{
	SendClientMessage(playerid, -1, "Sent message %s", text);
	return 1;
}

public OnPlayerSpawn(playerid)
{

	//Message_SendSpecificToClient(playerid, message1, -1, 5000, 3);
	Message_SendRandomToClient(playerid, -1, 5000, 3);
	return 1;
}