#if defined _warboot_texts_included
	#endinput
#endif
#define _warboot_texts_included

#include <open.mp>
#include <sscanf2>

#include <YSI_Coding\y_hooks>
#include <YSI_Coding\y_va>

// #include <Pawn.Regex>

// --
// Naming compactibility
// --

// --
// Entry
// --

#define ERROR_MSG_COLOR         0xFC2C03FF
#define WARNING_MSG_COLOR       0xFCE803FF
#define SUCCESS_MSG_COLOR       0xA1FC03FF
#define USAGE_MSG_COLOR         0xFC9003FF
#define HELP_MSG_COLOR          0x75231DFF
#define ANNOUNCEMENT_MSG_COLOR  0x688C8AFF

#define MESSAGE_LEVEL: __TAG(MESSAGE_LEVEL):
enum MESSAGE_LEVEL:__MESSAGE_LEVEL
{
    MESSAGE_ERROR = 0,
    MESSAGE_WARNING,
    MESSAGE_USAGE,
    MESSAGE_HELP,
    MESSAGE_SUCCESS,
    MESSAGE_ANNOUNCEMENT
}
static stock MESSAGE_LEVEL:_@MESSAGE_LEVEL() { return __MESSAGE_LEVEL; } 

// --
// Header
// --

forward OnPlayerReceivePrivateMessage(playerid, receiverid, string:text[]);

// --
// API
// --

// --
// Private
// --

/* Private messages are meant to be sent by a player for a certain player and is only visible by player who received the message. */

stock bool: Message_AllowPlayerPrivate(playerid, bool: allow)
{
	if(!IsPlayerConnected(playerid))
	{
		return false;
	}
	SetPVarInt(playerid, "__TEXT_AllowPrivate", (allow ? true : false));
	return true;
}

stock bool: Message_CanPlayerSendPrivate(playerid)
{
	if(!IsPlayerConnected(playerid))
	{
		return false;
	}
	return (GetPVarInt(playerid, "__TEXT_AllowPrivate") == 1 ? true : false);
}

stock bool: Message_SendPrivate(playerid, forplayerid, color, const string: text[110])
{
    if(!IsPlayerConnected(forplayerid) || playerid == forplayerid)
	{
		return false;
	}
	SendClientMessage(playerid, color, ">> %s(%i): %s", ReturnPlayerName(forplayerid), forplayerid, text);
	SendClientMessage(forplayerid, color, "** %s(%i): %s", ReturnPlayerName(playerid), playerid, text);
	CallRemoteFunction("OnPlayerReceivePrivateMessage", "iis", playerid, forplayerid, text);
	return true;
}

// --
// Message levels
// --


/** 
 * <library>messages</library>
 * <summary> Sends a message to player. </summary>
 * <param name="playerid"> playerid we're sending message to.</param>
 * <param name="level"> level of the message, error, warning...</param>
 * <param name="str"> Message text. </param>
 * <seealso name="Message_SendToAll" />
 * <returns>SendClientMessage Values</returns>
 */
stock bool: Message_Send(playerid, MESSAGE_LEVEL: level, const string: str[], ...)
{
    new 
        va_string[144],
        info[30],
        color;

    switch(level) 
    {
        case MESSAGE_ERROR: 
        {
			info = ">> Error: ";
            color = ERROR_MSG_COLOR;
        }
        case MESSAGE_WARNING: 
        {
			info = ">> Warning: ";
            color = WARNING_MSG_COLOR;
        }
        case MESSAGE_USAGE:
        {
			info = ">> Usage: ";
            color = USAGE_MSG_COLOR;
        }
        case MESSAGE_HELP: 
        {
			info = ">> Help: ";
            color = HELP_MSG_COLOR;
        }
        case MESSAGE_SUCCESS: 
        {
			info = ">> Success: ";
            color = SUCCESS_MSG_COLOR;
        }
        case MESSAGE_ANNOUNCEMENT:
        {
			info = ">> Announcement: ";
            color = ANNOUNCEMENT_MSG_COLOR;
        }
    }
    va_format(va_string, sizeof(va_string), "%s%s", info, str);
    return SendClientMessage(playerid, color, va_string, ___(3));
}

/** 
 * <library>messages</library>
 * <summary> Sends a message to all players. </summary>
 * <param name="level"> level of the message, error, warning...</param>
 * <param name="str"> Message text. </param>
 * <seealso name="Message_Send" />
 * <returns>This doesn't return any value.</returns>
 */
stock void: Message_SendToAll(MESSAGE_LEVEL: level, const string: str[], va_args<>)
{
    new 
        va_string[144],
        info[30],
        color;

    switch(level) 
    {
        case MESSAGE_ERROR: 
        {
			info = ">> Error: ";
            color = ERROR_MSG_COLOR;
        }
        case MESSAGE_WARNING: 
        {
			info = ">> Warning: ";
            color = WARNING_MSG_COLOR;
        }
        case MESSAGE_USAGE:
        {
			info = ">> Usage: ";
            color = USAGE_MSG_COLOR;
        }
        case MESSAGE_HELP: 
        {
			info = ">> Help: ";
            color = HELP_MSG_COLOR;
        }
        case MESSAGE_SUCCESS: 
        {
			info = ">> Success: ";
            color = SUCCESS_MSG_COLOR;
        }
        case MESSAGE_ANNOUNCEMENT:
        {
			info = ">> Announcement: ";
            color = ANNOUNCEMENT_MSG_COLOR;
        }
    }
    va_format(va_string, sizeof(va_string), "%s%s", info, str);
    SendClientMessageToAll(color, va_string, ___(2));
}

// --
// Mention
// --
stock bool: Message_ParseMentions(string: output[], const string: message[], mentionColor = 0x33BDFF, bool:mentionSound = true, len = sizeof(output))
{
    new 
        uids[12];

    strcopy(output, message, len);

    new startPos = -1, endPos = -1;
    if (sscanf(message, "p<@>A<u>(-1)[12]", uids))
    {
        return false;
    }

    for (new i = 1; i < 12; i ++)
    {
        printf("%d = %d", uids[i], i);
        if (IsPlayerConnected(uids[i]))
        {
            startPos = strfind(output, "@", .pos = startPos + 1);

            if (startPos == -1)
            {
                print("Unfortunately we cannot find more @ for some reason");
                return false;
            }

            endPos = strfind(output, " ", .pos = startPos + 1);
            if (endPos == -1)
            {
                endPos = strlen(output); // we're just going to assume that the endpos for the `@` is in the end of string
            }

            // cool trick to reuse variable
            new string:fixedTag[48];
            GetPlayerName(uids[i], fixedTag, MAX_PLAYER_NAME);
            format(fixedTag, sizeof(fixedTag), "{%x}@%s{FFFFFF}", mentionColor, fixedTag);

            // Do the replacement here
            strdel(output, startPos, endPos);
            strins(output, fixedTag, startPos, len);

            if (mentionSound)
            {
                // Play "DING" message
                PlayerPlaySound(uids[i], 17802, 0.0, 0.0, 0.0);
            }
        }
    }
    return true;
}

// --
// Hashtag
// --


// --
// Hooks
// --