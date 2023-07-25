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

Message_ParseMentions(string:output[], const string:message[], len = sizeof(len))
{
	new 
		uids[12];

	strcopy(output, text, len);

	if (!sscanf(text, "p<@>A<u>(-1)[12]", uids))
	{
		for (new i = 1; i < 12; i ++)
		{
			printf("%d = %d", uids[i], i);
			if (IsPlayerConnected(uids[i]))
			{
				new cur = -1;
				cur = strfind(output, "@", .pos = cur + 1);

				if (cur == -1)
				{
					print("Unfortunately we cannot find more @ for some reason");
					return 1;
				}

				new endPos = strfind(output, " ", .pos = cur + 1);
				if (endPos == -1)
				{
					endPos = strlen(output); // we're just going to assume that the endpos for the `@` is in the end of string
				}

				// cool trick to reuse variable
				new	string:fixedTag[48];
				GetPlayerName(uids[i], fixedTag, MAX_PLAYER_NAME);
				format(fixedTag, sizeof(fixedTag), "{33BDFF}@%s{FFFFFF}", fixedTag);

				// Do the replacement here
				strdel(output, cur, endPos);
				strins(output, fixedTag, cur);
			}
		}
	}
}
