# text

[![sampctl](https://img.shields.io/badge/sampctl-text-2f2f2f.svg?style=for-the-badge)](https://github.com/Mergevos/text)

This package is part of warboot project open packages. 
Library allows you to do all things with various text functions (in game related), such as `SendClientMessage(ToAll)`, having message levels, random messages, customized messages, message channels, mentions and hashtags, private messages..
## Installation

Simply install to your project:

```bash
sampctl package install Mergevos/text
```

Include in your code and begin using the library:

```pawn
#include <text>
```

## Usage

<!--
Write your code documentation or examples here. If your library is documented in
the source code, direct users there. If not, list your API and describe it well
in this section. If your library is passive and has no API, simply omit this
section.
-->

## Testing

<!--
Depending on whether your package is tested via in-game "demo tests" or
y_testing unit-tests, you should indicate to readers what to expect below here.
-->

To test, simply run the package:

```bash
sampctl package run
```




#if defined _warboot_texts_included
	#endinput
#endif
#define _warboot_texts_included

#include <open.mp>
#include <sscanf2>

#include <YSI_Coding\y_hooks>
#include <YSI_Coding\y_va>

#include <logger>

// --
// Naming compatibility
// --

// --
// Entry
// --

#define ERROR_MSG_COLOR             0xFC2C03FF
#define WARNING_MSG_COLOR           0xFCE803FF
#define SUCCESS_MSG_COLOR           0xA1FC03FF
#define USAGE_MSG_COLOR             0xFC9003FF
#define HELP_MSG_COLOR              0x75231DFF
#define ANNOUNCEMENT_MSG_COLOR      0x688C8AFF

#if !defined MAX_MENTIONED_MESSAGE  
    #define MAX_MENTIONED_MESSAGE   (3)
#endif

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

#define TEXT_CONFIG: __TAG(TEXT_CONFIG):
enum TEXT_CONFIG:__TEXT_CONFIG
{
    TEXT_CONFIG_CALL_OPM,
    TEXT_CONFIG_CALL_OPRPM,
    TEXT_CONFIG_ALLOW_MENTIONS
}
static stock TEXT_CONFIG:_@TEXT_CONFIG() { return __TEXT_CONFIG; } 

#define TEXT_LOG_OPTION: __TAG(TEXT_LOG_OPTION):
enum TEXT_LOG_OPTION:__TEXT_LOG_OPTION(<<=1)
{
    TEXT_LOG_NONE = 0,
    TEXT_LOG_WARNINGS = 1,
    TEXT_LOG_ERRORS, 
    TEXT_LOG_DEBUG
}
static stock TEXT_LOG_OPTION:_@TEXT_LOG_OPTION() { return __TEXT_LOG_OPTION; } 

static 
    TEXT_LOG_OPTION: Text_LogLevel,
    bool: Text_BooleanConfig[TEXT_CONFIG:__TEXT_CONFIG];

// --
// Header
// --

forward OnPlayerReceivePrivateMessage(playerid, receiverid, string:text[]);
forward OnPlayerMessageMentioned(playerid, senderid);

// --
// API
// --

// --
// Settings
// --

stock void: Text_Log(TEXT_LOG_OPTION: option, bool: set, bool: save = true)
{
    if(set) 
    {
        Text_LogLevel |= option;
    }
    else if(!set)
    {
        Text_LogLevel &= ~option;
    }
    if(save)
    {
        // json
    }
}

stock Text_SetConfiguration(TEXT_CONFIG: config, bool: value, bool: save = true)
{
    Text_BooleanConfig[config] = value;
    if(save)
    {
        //json
    }
}

stock bool: Text_GetConfiguration(TEXT_CONFIG: config)
{
    return Text_BooleanConfig[config];
}

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
    if(!IsPlayerConnected(forplayerid))
	{
		return false;
	}
	SendClientMessage(playerid, color, ">> %s(%i): %s", ReturnPlayerName(forplayerid), forplayerid, text);
	SendClientMessage(forplayerid, color, "** %s(%i): %s", ReturnPlayerName(playerid), playerid, text);
    if(Text_BooleanConfig[TEXT_CONFIG_CALL_OPRPM])
    {
        CallRemoteFunction("OnPlayerReceivePrivateMessage", "iis", playerid, forplayerid, text);
    }
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

stock bool: Message_ParseMention(string: output[], const string: message[], mentionColor = 0x33BDFF, senderid = INVALID_PLAYER_ID, len = sizeof(output))
{
    if (IsNull(message))
    {   
        print("1");
        return false;
    }
    strcopy(output, message, len);

    new posStart = -1, maxMentioned = 0;
    print("12");
    for (new i = 0, j = strlen(output); i < j + 1; i ++)
    {
        print("123");
        if (posStart != -1)
        {
            print("1234");
            new string:extractedString[32];
            new c = output[i];
            if (!(c == ',' || c == '.' || c == ' ' || c == '?' || c == '!' || c == EOS) || i == j)
            {
                continue;
            }
            print("12345");
            if (maxMentioned >= MAX_MENTIONED_MESSAGE)
            {
                break;
            }

            strmid(extractedString, output, posStart + 1, i, len);

            new playerIds;
            print("123456");
            if (sscanf(extractedString, "r", playerIds))
            {
                posStart = -1;
                continue;
            }
                
            new playerName[35];
            new nameLength;

            nameLength = GetPlayerName(playerIds, playerName, MAX_PLAYER_NAME);
            print("1234567");
            if (!nameLength)
            {
                printf("Error: Player %d might be leaving or he's having invalid name", playerIds);
                continue;
            }
            print("12345678");

            format(playerName, sizeof(playerName), "{%x}@%s{FFFFFF}", mentionColor, playerName);

            strdel(output, posStart, i);
            strins(output, playerName, posStart, len);
            print("123456789");
            CallRemoteFunction(#OnPlayerMessageMentioned, "ii", playerIds, senderid);

            // recalculate the loop size and skip
            i = posStart + nameLength + 1;
            posStart = -1;
            j = strlen(output);
            maxMentioned ++;
            continue;
            print("12345678910");
        }
        if (output[i] == '@' && posStart == -1)
        {
           posStart = i;
           continue; 
        }
        print("1234567891011");
    }
    return true;
}

// --
// Hashtag
// --


// --
// Hooks
// --
