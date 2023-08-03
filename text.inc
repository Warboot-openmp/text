#if defined _warboot_texts_included
	#endinput
#endif
#define _warboot_texts_included

#include <open.mp>
#include <sscanf2>

#include <YSI_Data\y_foreach> 
#include <YSI_Coding\y_hooks>
#include <YSI_Coding\y_va>

#include <PawnPlus>

#include <json>
#include <logger>

#define __INC_VERSION__ "1.0.0"

// --
// Naming compatibility
// --

// --
// Entry
// --

#if !defined MAX_RANDOM_MESSAGE_SIZE
    #define MAX_RANDOM_MESSAGE_SIZE 160
#endif

#if !defined MAX_RANDOM_MESSAGES 
    #define MAX_RANDOM_MESSAGES 30
#endif

#define ERROR_MSG_COLOR             0xFC2C03FF
#define WARNING_MSG_COLOR           0xFCE803FF
#define SUCCESS_MSG_COLOR           0xA1FC03FF
#define USAGE_MSG_COLOR             0xFC9003FF
#define HELP_MSG_COLOR              0x75231DFF
#define ANNOUNCEMENT_MSG_COLOR      0x688C8AFF

#if !defined MAX_MENTIONED_MESSAGE  
    #define MAX_MENTIONED_MESSAGE   (3)
#endif

static 
    Random_MessageText[RandomMessage:MAX_RANDOM_MESSAGES][MAX_RANDOM_MESSAGE_SIZE],
    Iterator: Messages_Iterator<MAX_RANDOM_MESSAGES>;

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
    TEXT_CONFIG_CALL_OPMM,
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
    TEXT_LOG_DEBUG,
    TEXT_LOG_ALL = TEXT_LOG_WARNINGS | TEXT_LOG_ERRORS | TEXT_LOG_DEBUG
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

stock void: Text_Log(TEXT_LOG_OPTION: option, bool: set)
{
    if(set) 
    {
        Text_LogLevel |= option;
    }
    else if(!set)
    {
        Text_LogLevel &= ~option;
    }
}

stock Text_SetConfiguration(TEXT_CONFIG: config, bool: value)
{
    Text_BooleanConfig[config] = value;
}

stock bool: Text_GetConfiguration(TEXT_CONFIG: config)
{
    return Text_BooleanConfig[config];
}


// --
// Private
// --

/* Private messages are meant to be sent by a player for a certain player and is only visible by player who received the message. */
stock bool: Message_CanPlayerReceivePrivate(playerid)
{
	if(!IsPlayerConnected(playerid))
	{
        Logger_Dbg("__TEXT_DEBUG", "Player is not connected.", Logger_S("function", "Message_CanPlayerReceivePrivate"));
		return false;
	}
    Logger_Dbg("__TEXT_DEBUG", "Function executed successfully", Logger_S("player", ReturnPlayerName(playerid)), Logger_S("function", "Message_CanPlayerReceivePrivate"));
	return (GetPVarInt(playerid, "__TEXT_Receive") == 0 ? true : false);
}

stock bool: Message_BlockPrivate(playerid, bool: block)
{
	if(!IsPlayerConnected(playerid))
	{
        Logger_Dbg("__TEXT_DEBUG", "Player is not connected.", Logger_S("function", "Message_BlockPrivate"));
		return false;
	}
	SetPVarInt(playerid, "__TEXT_Receive", (block ? true : false));
    Logger_Dbg("__TEXT_DEBUG", "Function executed successfully", Logger_S("player", ReturnPlayerName(playerid)), Logger_S("function", "Message_BlockPrivate"));
	return true;
}

stock bool: Message_AllowPlayerPrivate(playerid, bool: allow)
{
	if(!IsPlayerConnected(playerid))
	{
        Logger_Dbg("__TEXT_DEBUG", "Player is not connected.", Logger_S("function", "Message_AllowPlayerPrivate"));
		return false;
	}
	SetPVarInt(playerid, "__TEXT_AllowPrivate", (allow ? true : false));
    Logger_Dbg("__TEXT_DEBUG", "Function executed successfully", Logger_S("player", ReturnPlayerName(playerid)), Logger_S("function", "Message_AllowPlayerPrivate"));
	return true;
}

stock bool: Message_CanPlayerSendPrivate(playerid)
{
	if(!IsPlayerConnected(playerid))
	{
        Logger_Dbg("__TEXT_DEBUG", "Player is not connected.", Logger_S("function", "Message_CanPlayerSendPrivate"));
		return false;
	}
    Logger_Dbg("__TEXT_DEBUG", "Function executed successfully", Logger_S("player", ReturnPlayerName(playerid)), Logger_S("function", "Message_CanPlayerSendPrivate"));
	return (GetPVarInt(playerid, "__TEXT_AllowPrivate") == 1 ? true : false);
}

stock bool: Message_SendPrivate(playerid, forplayerid, color, const string: text[110])
{
    if(!IsPlayerConnected(forplayerid))
	{
        Logger_Dbg("__TEXT_DEBUG", "Player is not connected.", Logger_S("function", "Message_SendPrivate"));
		return false;
	}
    if(GetPVarInt(playerid, "__TEXT_Receive") == 1)
    {
        return false;
    } 
	SendClientMessage(playerid, color, ">> %s(%i): %s", ReturnPlayerName(forplayerid), forplayerid, text);
	SendClientMessage(forplayerid, color, "** %s(%i): %s", ReturnPlayerName(playerid), playerid, text);
    if(Text_BooleanConfig[TEXT_CONFIG_CALL_OPRPM])
    {
        CallRemoteFunction("OnPlayerReceivePrivateMessage", "iis", playerid, forplayerid, text);
    }
    Logger_Dbg("__TEXT_DEBUG", "Function executed successfully", Logger_S("player", ReturnPlayerName(playerid)), Logger_S("function", "Message_SendPrivate"));
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

    if(!IsPlayerConnected(playerid))
    {
        Logger_Dbg("__TEXT_DEBUG", "Player is not connected.", Logger_S("function", "Message_Send"));
        return false;
    }
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
    SendClientMessage(playerid, color, va_string, ___(3));
    Logger_Dbg("__TEXT_DEBUG", "Function executed successfully.", Logger_S("player", ReturnPlayerName(playerid)), Logger_S("function", "Message_Send"));
    return true;
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
    Logger_Dbg("__TEXT_DEBUG", "Function executed successfully.", Logger_S("function", "Message_SendToAll"));

}

// --
// Random
// --
stock RandomMessage: Message_AddRandom(const string: message[MAX_RANDOM_MESSAGE_SIZE])
{
    new RandomMessage: idx = RandomMessage: Iter_Alloc(Messages_Iterator);
    if(idx == RandomMessage: INVALID_ITERATOR_SLOT)
    {
        return RandomMessage: -1;
    }
    strcopy(Random_MessageText[idx], message);
    return idx;
}

stock bool: Message_DeleteRandom(RandomMessage: id)
{
    if(!Iter_Contains(Messages_Iterator, Random_MessageText[idx]))
    {
        return false;
    }
    Random_MessageText[id][0] = EOS;
    Iter_Remove(Messages_Iterator, Random_MessageText[idx]);
    return true;
}

stock Message_GetNumber()
{
    return Iter_Count(Messages_Iterator);
}

stock bool: Message_RandomGet(RandomMessage: messageid, string: dest[], len = sizeof(dest))
{
    if(!Iter_Contains(Messages_Iterator, _:messageid))
    {
        return false;
    }
    new rdx = Iter_Random(Messages_Iterator);
    strcopy(dest, Random_MessageText[messageid], maxlength = sizeof (dest));
    return true;
}

stock bool: Message_SendRandomToAll(color, time, times = 1)
{
    if(Iter_IsEmpty(Messages_Iterator) || time < 1)
    {
        return false;
    }
    while(times != 0)
    {
        wait_ms(time);
        new RandomMessage: rndx = RandomMessage: Iter_Random(Messages_Iterator);
        SendClientMessageToAll(color, Random_MessageText[rndx]);
        times--;
    }
    return true;
}

stock bool: Message_SendRandomToClient(playerid, color, time, times = 1)
{
    if(Iter_IsEmpty(Messages_Iterator))
    {
        return false;
    }
    if(!IsPlayerConnected(playerid))
    {
        return false;
    }
    while(times != 0)
    {
        wait_ms(time);
        new RandomMessage: rndx = RandomMessage: Iter_Random(Messages_Iterator);
        SendClientMessage(playerid, color, Random_MessageText[rndx]);
        times--;
    }
    return true;
}


stock bool: Message_SendSpecificToClient(playerid, RandomMessage: id, color, time, times = 1)
{
    if(!Iter_Contains(Messages_Iterator, _:id))
    {
        return false;
    }
    if(!IsPlayerConnected(playerid))
    {
        return false;
    }
    inline sendMessage()
    {
        SendClientMessage(playerid, color, Random_MessageText[id]);
    }
    Timer_CreateCallback(using inline sendMessage, time, times);
    return true;
}

stock bool: Message_SendSpecificToAll(playerid, RandomMessage: id, color, time, times = 1)
{
    if(!Iter_Contains(Messages_Iterator, _:id))
    {
        return false;
    }
    while(times != 0)
    {
        wait_ms(time);
        new RandomMessage: rndx = RandomMessage: Iter_Random(Messages_Iterator);
        SendClientMessageToAll(color, Random_MessageText[id]);
        times--;
    }

    return true;
}


// --
// Mention
// --

stock bool: Message_ParseMention(string: output[], const string: message[], mentionColor = 0x33BDFF, senderid = INVALID_PLAYER_ID, len = sizeof(output))
{
    if (IsNull(message))
    {   
        Logger_Dbg("__TEXT_DEBUG", "Message within function is null.", Logger_S("function", "Message_ParseMention"));
        return false;
    }
    strcopy(output, message, len);

    new posStart = -1, maxMentioned = 0;
    for (new i = 0, j = strlen(output); i < j + 1; i ++)
    {
        if (posStart != -1)
        {
            new string:extractedString[32];
            
	        if (!((output[i] == ',' || output[i] == '.' || output[i] == ' ' || output[i] == '?' || output[i] == '!' || output[i] == EOS) || i == j))
            {
                continue;
            }
            if (maxMentioned >= MAX_MENTIONED_MESSAGE)
            {
                break;
            }

            strmid(extractedString, output, posStart + 1, i, len);

            new playerIds;
            if (sscanf(extractedString, "r", playerIds))
            {
                posStart = -1;
                continue;
            }
                
            new playerName[35];
            new nameLength;

            nameLength = GetPlayerName(playerIds, playerName, MAX_PLAYER_NAME);
            if (!nameLength)
            {
                Logger_Dbg("__TEXT_DEBUG", "Player might be leaving or has invalid name.", Logger_I("playerid", playerIds), Logger_S("player's name", ReturnPlayerName(playerIds)));
                continue;
            }
            format(playerName, sizeof(playerName), "{%x}@%s{FFFFFF}", mentionColor, playerName);

            strdel(output, posStart, i);
            strins(output, playerName, posStart, len);
            CallRemoteFunction(#OnPlayerMessageMentioned, "ii", playerIds, senderid);

            // recalculate the loop size and skip
            i = posStart + nameLength + 1;
            posStart = -1;
            j = strlen(output);
            maxMentioned ++;
            continue;
        }
        if (output[i] == '@' && posStart == -1)
        {
           posStart = i;
           continue; 
        }
    }
    return true;
}

// --
static stock bool: json_Parse()
{
    new 
        Node: node,
        bool: data,
        TEXT_LOG_OPTION: option;

    new ret;

    ret = JSON_ParseFile("text.json", node);
    if(ret) 
    {
        Logger_Err("Couldn't parse file.", Logger_S("file", "text.json"));
        return false;
    }

    ret = JSON_GetInt(node, "logging", _:option);
    if(ret)
    {
        Logger_Log("Couldn't find key.", Logger_S("key", "logging"));
        return false;
    }
    Text_LogLevel = option;
    if(Text_LogLevel & TEXT_LOG_DEBUG) 
    {
        Logger_ToggleDebug("__TEXT_DEBUG", true);
        Logger_Dbg("__TEXT_DEBUG", "Debugger turned on.");
    }

    Logger_Dbg("__TEXT_DEBUG", "Parsing data from file...", Logger_S("file", "text.json"));

    Logger_Dbg("__TEXT_DEBUG", "Parsing option.", Logger_S("option", "allow_mentions"));
    ret = JSON_GetBool(node, "allow_mentions", data);
    if(ret)
    {
        Logger_Log("Couldn't parse option.", Logger_S("option", "allow_mentions"));
        return false;
    }
    Text_BooleanConfig[TEXT_CONFIG_ALLOW_MENTIONS] = data;
    Logger_Dbg("__TEXT_DEBUG", "Successfully parsed option.", Logger_S("option", "allow_mentions"));

    Logger_Dbg("__TEXT_DEBUG", "Parsing option.", Logger_S("option", "call_opmm"));
    ret = JSON_GetBool(node, "call_opmm", data);
    if(ret)
    {
        Logger_Log("Couldn't parse option.", Logger_S("option", "call_opmm"));
        return false;
    }
    Text_BooleanConfig[TEXT_CONFIG_CALL_OPMM] = data;
    Logger_Dbg("__TEXT_DEBUG", "Successfully parsed option.", Logger_S("option", "call_opmm"));

    Logger_Dbg("__TEXT_DEBUG", "Parsing option.", Logger_S("option", "call_oprpm"));
    ret = JSON_GetBool(node, "call_oprpm", data);
    if(ret)
    {
        Logger_Log("Couldn't parse option.", Logger_S("option", "call_oprpm"));
        return false;
    }
    Text_BooleanConfig[TEXT_CONFIG_CALL_OPRPM] = data;
    Logger_Dbg("__TEXT_DEBUG", "Successfully parsed option.", Logger_S("option", "call_oprpm"));

    Logger_Dbg("__TEXT_DEBUG", "Finished parsing from file.", Logger_S("file", "text.json"));
    printf("[warboot-text-module] Version %s successfully loaded.", __INC_VERSION__);
    return true;
}

static stock json_Save()
{
    Logger_Dbg("__TEXT_DEBUG", "Storing data to file...", Logger_S("file", "text.json"));
    new Node:node = JSON_Object(
        "logging", JSON_Int(Text_LogLevel),
        "call_opmm", JSON_Bool(bool:Text_BooleanConfig[TEXT_CONFIG_CALL_OPMM]),
        "allow_mentions", JSON_Bool(bool:Text_BooleanConfig[TEXT_CONFIG_ALLOW_MENTIONS]),
        "call_oprpm", JSON_Bool(bool:Text_BooleanConfig[TEXT_CONFIG_CALL_OPRPM])
    ); 
    Logger_Dbg("__TEXT_DEBUG", "Successfully created node.");
    JSON_SaveFile("text.json", node, true);
    Logger_Dbg("__TEXT_DEBUG", "Successfully stored.", Logger_S("file", "text.json"));
}

static stock text_GetConfigName(TEXT_CONFIG: config, string: dest[], len = sizeof(dest))
{
    if(config == TEXT_CONFIG_ALLOW_MENTIONS)
    {
        strcopy(dest, "allow_mentions");
    }
    else if(config == TEXT_CONFIG_CALL_OPMM)
    {
        strcopy(dest, "call_opmm");
    }
    else if(config == TEXT_CONFIG_CALL_OPRPM)
    {
        strcopy(dest, "call_oprpm");
    }
}

// --
// Hooks
// --

hook OnScriptInit()
{
    json_Parse();
    return true;
}

hook OnScriptExit()
{
    json_Save();
    return Y_HOOKS_CONTINUE_RETURN_1;
}
