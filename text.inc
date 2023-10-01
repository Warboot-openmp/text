#if defined _warboot_texts_included
	#endinput
#endif
#define _warboot_texts_included

#include <open.mp>
#include <sscanf2>

#include <YSI_Data\y_foreach> 
#include <YSI_Coding\y_hooks>
#include <YSI_Coding\y_va>

#include <json>
#include <errors>
#include <logger>

#define __INC_VERSION__ "1.0.0"
// do proper logging settings and summary
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
#define DEBUG_MSG_COLOR             0x326376FF

#if !defined MAX_MENTIONED_MESSAGE  
    #define MAX_MENTIONED_MESSAGE   (3)
#endif

static 
    Random_MessageText[RandomMessage:MAX_RANDOM_MESSAGES][MAX_RANDOM_MESSAGE_SIZE],
    Iterator: Messages_Iterator<MAX_RANDOM_MESSAGES>;

#if !defined MAX_CHAT_GROUPS
    #define MAX_CHAT_GROUPS 5
#endif

#if !defined MAX_CHAT_GROUP_MEMBERS
    #define MAX_CHAT_GROUP_MEMBERS 30
#endif

#define ChatGroup: Channel: 

static 
    ChatGroup_Name[ChatGroup: MAX_CHAT_GROUPS][30],
    ChatGroup_MaxMembers[ChatGroup: MAX_CHAT_GROUPS],
    Iterator: Chat_Groups<MAX_CHAT_GROUPS>,
    Iterator: ChatGroup_Members[ChatGroup: MAX_CHAT_GROUPS]<MAX_CHAT_GROUP_MEMBERS>;

#define MESSAGE_LEVEL: __TAG(MESSAGE_LEVEL):
enum MESSAGE_LEVEL:__MESSAGE_LEVEL
{
    MESSAGE_ERROR = 0,
    MESSAGE_WARNING,
    MESSAGE_USAGE,
    MESSAGE_HELP,
    MESSAGE_SUCCESS,
    MESSAGE_ANNOUNCEMENT,
    MESSAGE_DEBUG
}
static stock MESSAGE_LEVEL:_@MESSAGE_LEVEL() { return __MESSAGE_LEVEL; } 

#define TEXT_CONFIG: __TAG(TEXT_CONFIG):
enum TEXT_CONFIG:__TEXT_CONFIG
{
    TEXT_CONFIG_CALL_OPMM,
    TEXT_CONFIG_CALL_OPRPM,
    TEXT_CONFIG_ALLOW_MENTIONS,
    TEXT_CONFIG_ALLOW_PM_STATUS
}
static stock TEXT_CONFIG:_@TEXT_CONFIG() { return __TEXT_CONFIG; } 

static 
    bool: Text_DebugLevel,
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

/** 
 * <library>messages</library>
 * <summary>Sets debug handle status.</summary>
 * <param name="set">Status to set.</param>
 * <returns>No specific values.</returns>
 */
stock void: Text_Debug(bool: set)
{
    Logger_ToggleDebug("__TEXT_DEBUG", set);
    Text_DebugLevel = set;
}

/** 
 * <library>messages</library>
 * <summary>Sets configuration status.</summary>
 * <param name="config">Configuration value.</param>
 * <param name="value">Status to set.</param>
 * <returns>No specific values.</returns>
 */
stock void: Text_SetConfiguration(TEXT_CONFIG: config, bool: value)
{
    Text_BooleanConfig[config] = value;
}

/** 
 * <library>messages</library>
 * <summary>Gets configuration status.</summary>
 * <param name="config">Configuration.</param>
 * <returns>True if configuration is on, otherwise false.</returns>
 */
stock bool: Text_GetConfiguration(TEXT_CONFIG: config)
{
    return Text_BooleanConfig[config];
}


// --
// Private
// --

/* Private messages are meant to be sent by a player for a certain player and is only visible by player who received the message. */

/** 
 * <library>messages</library>
 * <summary> Chceks if a player can receive private messages. </summary>
 * <param name="playerid">Player to check.</param>
 * <returns>True if can, false if can't or non connected.</returns>
 */
stock bool: Message_CanPlayerReceivePrivate(playerid)
{
	if(!IsPlayerConnected(playerid))
	{
        Logger_Dbg("__TEXT_DEBUG", "Player is not connected.", Logger_S("function", "Message_CanPlayerReceivePrivate"));
		return false;
	}
    return (GetPVarInt(playerid, "__TEXT_Receive") == 0 ? true : false);
}

/** 
 * <library>messages</library>
 * <summary> Blocks player's receiving of private messages. </summary>
 * <param name="playerid">Player to block receiving.</param>
 * <param name="block">Status.</param>
 * <returns>
        Error code 1 if player is not connected.
        Error code 2 if modifying private message status is blocked.
        0 if success.
    </returns>
 */
stock Error: Message_BlockPrivate(playerid, bool: block)
{
	if(!IsPlayerConnected(playerid))
	{
        Logger_Dbg("__TEXT_DEBUG", "Player is not connected.", Logger_S("function", "Message_BlockPrivate"));
		return Error(1, "Not connected");
	}
    if(!Text_BooleanConfig[TEXT_CONFIG_ALLOW_PM_STATUS])
    {
        Logger_Dbg("__TEXT_DEBUG", "Player tried to block PMs.", Logger_I("playerid", playerid), Logger_S("player", ReturnPlayerName(playerid)));
        return Error(2, "Modifying private message status is not allowed.");
    }
	SetPVarInt(playerid, "__TEXT_Receive", (block ? true : false));
    return Ok();
}

/** 
 * <library>messages</library>
 * <summary> Allows a player to send private messages. </summary>
 * <param name="playerid">Player to allow.</param>
 * <returns>True if executed successfully, otherwise false (player not connected).</returns>
 */
stock bool: Message_AllowPlayerPrivate(playerid, bool: allow)
{
	if(!IsPlayerConnected(playerid))
	{
        Logger_Dbg("__TEXT_DEBUG", "Player is not connected.", Logger_S("function", "Message_AllowPlayerPrivate"));
		return false;
	}
	SetPVarInt(playerid, "__TEXT_AllowPrivate", (allow ? 1 : 0));
    return true;
}

/** 
 * <library>messages</library>
 * <summary> Chceks if a player can send private messages. </summary>
 * <param name="playerid">Player to check.</param>
 * <returns>True if can, false if can't or non connected.</returns>
 */
stock bool: Message_CanPlayerSendPrivate(playerid)
{
	if(!IsPlayerConnected(playerid))
	{
        Logger_Dbg("__TEXT_DEBUG", "Player is not connected.", Logger_S("function", "Message_CanPlayerSendPrivate"));
		return false;
	}
    return (GetPVarInt(playerid, "__TEXT_AllowPrivate") == 1 ? true : false);
}

/** 
 * <library>messages</library>
 * <summary> Sends a private message to player. </summary>
 * <param name="playerid">Sender.</param>
 * <param name="forplayerid">Receiver</param>
 * <param name="color">Message color. </param>
 * <param name="text">Message text. </param>
 * <returns>SendClientMessage Values</returns>
 */
stock Error: Message_SendPrivate(playerid, forplayerid, color, const string: text[110])
{
    if(!IsPlayerConnected(forplayerid))
	{
        Logger_Dbg("__TEXT_DEBUG", "Player is not connected.", Logger_S("function", "Message_SendPrivate"));
		return Error(1, "Not connected");
	}
    if(GetPVarInt(playerid, "__TEXT_Receive") == 0)
    {
        Logger_Dbg("__TEXT_DEBUG", "Player can't receive PM", Logger_I("playerid", playerid), Logger_S("player", ReturnPlayerName(playerid)));
        return Error(2, "Can't receive PM");
    } 
	SendClientMessage(playerid, color, ">> %s(%i): %s", ReturnPlayerName(forplayerid), forplayerid, text);
	SendClientMessage(forplayerid, color, "** %s(%i): %s", ReturnPlayerName(playerid), playerid, text);
    if(Text_BooleanConfig[TEXT_CONFIG_CALL_OPRPM])
    {
        CallRemoteFunction("OnPlayerReceivePrivateMessage", "iis", playerid, forplayerid, text);
    }
    return Ok();
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
        case MESSAGE_DEBUG:
        {
			info = ">> Debug: ";
            color = DEBUG_MSG_COLOR;
        }
    }
    va_format(va_string, sizeof(va_string), "%s%s", info, str);
    SendClientMessage(playerid, color, va_string, ___(3));
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
        case MESSAGE_DEBUG:
        {
			info = ">> Debug: ";
            color = DEBUG_MSG_COLOR;
        }
    }
    va_format(va_string, sizeof(va_string), "%s%s", info, str);
    SendClientMessageToAll(color, va_string, ___(2));
    
}

// --
// Random
// --

/** 
 * <library>messages</library>
 * <summary>Creates a random message. </summary>
 * <param name="message">Message text.</param>
 * <seealso name="Message_DeleteRandom" />
 * <returns>Index if created, otherwise -1.</returns>
 */
stock RandomMessage: Message_AddRandom(const string: message[MAX_RANDOM_MESSAGE_SIZE])
{
    new RandomMessage: idx = RandomMessage: Iter_Alloc(Messages_Iterator);
    if(idx == RandomMessage: INVALID_ITERATOR_SLOT)
    {
        Logger_Dbg("__TEXT_DEBUG", "Invalid iterator slot.", Logger_S("function", "Message_AddRandom"));
        return RandomMessage: -1;
    }
    strcopy(Random_MessageText[idx], message);
    return idx;
}

/** 
 * <library>messages</library>
 * <summary>Deletes a random message. </summary>
 * <param name="id">ID of message to delete.</param>
 * <seealso name="Message_AddRandom" />
 * <returns>Index if created, otherwise -1.</returns>
 */
stock bool: Message_DeleteRandom(RandomMessage: id)
{
    if(!Iter_Contains(Messages_Iterator, Random_MessageText[idx]))
    {
        Logger_Dbg("__TEXT_DEBUG", "Message doesn't exist.", Logger_S("function", "Message_DeleteRandom"));
        return false;
    }
    Random_MessageText[id][0] = EOS;
    Iter_Remove(Messages_Iterator, Random_MessageText[idx]);
    return true;
}

/** 
 * <library>messages</library>
 * <summary>Returns number of created messages.</summary>
 * <returns>Number of created messages.</returns>
 */
stock Message_GetNumber()
{
    return Iter_Count(Messages_Iterator);
}

/** 
 * <library>messages</library>
 * <summary>Gets random message text. </summary>
 * <param name="messageid">Message to get.</param>
 * <param name="dest">String destination.</param>
 * <seealso name="Message_DeleteRandom" />
 * <returns>Index if created, otherwise -1.</returns>
 */
stock bool: Message_RandomGet(RandomMessage: messageid, string: dest[], len = sizeof(dest))
{
    if(!Iter_Contains(Messages_Iterator, _:messageid))
    {
        Logger_Dbg("__TEXT_DEBUG", "Message doesn't exist.", Logger_S("function", "Message_RandomGet"));
        return false;
    }
    new rdx = Iter_Random(Messages_Iterator);
    strcopy(dest, Random_MessageText[messageid], maxlength = sizeof (dest));
    return true;
}

/** 
 * <library>messages</library>
 * <summary>Sends random message to all. </summary>
 * <param name="color">Color of the message.</param>
 * <param name="time">Delay between messages.</param>
 * <param name="times">How many times to send message, 0 - forever.</param>
 * <seealso name="Message_SendRandomToClient" />
 * <returns>Error code 1 if iterator is empty (no messages), 2 if invalid count, 0 if successful.</returns>
 */
stock Error: Message_SendRandomToAll(color, time, times = 0)
{
    if(Iter_IsEmpty(Messages_Iterator))
    {
        Logger_Dbg("__TEXT_DEBUG", "Iterator is empty.", Logger_S("function", "Message_SendRandomToAll"));
        return Error(1, "Empty iterator.");
    }
    if(times < 0)
    {
        Logger_Dbg("__TEXT_DEBUG", "Invalid count, must be above or equal to 0", Logger_I("count", times), Logger_S("function", "Message_SendRandomToAll"));
        return Error(2, "Invalid times input");
    }
    SetTimerEx("__text_randomSend", time, false, "iiiii", -1, color, 1, time, times);
    return Ok();
}

/** 
 * <library>messages</library>
 * <summary>Sends random message to player. </summary>
 * <param name="playerid">Player to send message to.</param>
 * <param name="color">Color of the message.</param>
 * <param name="time">Delay between messages.</param>
 * <param name="times">How many times to send message.</param>
 * <seealso name="Message_SendRandomToClient" />
 * <returns>Error code 1 if iterator is empty (no messages), 2 if player is not connected, 0 if successful.</returns>
 */
stock Error: Message_SendRandomToClient(playerid, color, time, times = 1)
{
    if(Iter_IsEmpty(Messages_Iterator))
    {
        Logger_Dbg("__TEXT_DEBUG", "Iterator is empty.", Logger_S("function", "Message_SendRandomToClient"));
        return Error(1, "Empty iterator.");
    }
    if(!IsPlayerConnected(playerid))
    {
        Logger_Dbg("__TEXT_DEBUG", "Player is not connected.", Logger_S("function", "Message_SendRandomToClient"));
        return Error(2, "Not connected.");
    }
    SetTimerEx("__text_randomSend", time, false, "iiiii", playerid, color, 2, time, times);
    return Ok();
}

// --
// Mention
// --

/** 
 * <library>messages</library>
 * <summary>Parses players' names instead @. </summary>
 * <param name="output">Destination string.</param>
 * <param name="message">Message to modify.</param>
 * <param name="mentionColor">Color of tag.</param>
 * <param name="senderid">Who mentioned player, e.g. who sent message.</param>
 * <returns>Error code 1 if message is invalid, 2 if mentions are turned off, 0 if successful.</returns>
 */
stock Error: Message_ParseMention(string: output[], const string: message[], mentionColor = 0x33BDFF, senderid = INVALID_PLAYER_ID, len = sizeof(output))
{
    if (IsNull(message))
    {   
        Logger_Dbg("__TEXT_DEBUG", "Message within function is null.", Logger_S("function", "Message_ParseMention"));
        return Error(1, "Invalid message.");
    }
    if (Text_BooleanConfig[TEXT_CONFIG_ALLOW_MENTIONS])
    {
        Logger_Dbg("__TEXT_DEBUG", "Mentions aren't turned on.", Logger_S("function", "Message_ParseMention"));
        return Error(2, "Mentions turned off.");
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
            if(Text_BooleanConfig[TEXT_CONFIG_CALL_OPMM])
            {
                CallRemoteFunction("OnPlayerMessageMentioned", "ii", playerIds, senderid);
            }
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
    return Ok();
}

// --
// Channels/chat groups
// --

/** 
 * <library>messages</library>
 * <summary>Creates a chat group.</summary>
 * <param name="name">Name of the group.</param>
 * <param name="maxmembers">Max members group can have.</param>
 * <returns>Index of group if created, otherwise -1.</returns>
 */
stock ChatGroup: ChatGroup_Create(const string: name[], maxmembers = MAX_CHAT_GROUP_MEMBERS)
{
    new ChatGroup: idx = ChatGroup: Iter_Alloc(Chat_Groups);
    if(idx == ChatGroup: INVALID_ITERATOR_SLOT)
    {
        Logger_Dbg("__TEXT_DEBUG", "Couldn't create chat group.");
        return ChatGroup: -1;
    }
    strcopy(ChatGroup_Name[idx], name);
    ChatGroup_MaxMembers[idx] = maxmembers;
    return idx;
}

/** 
 * <library>messages</library>
 * <summary>Checks if group is valid</summary>
 * <param name="id">Id of the group.</param>
 * <returns>True if valid, otherwise false.</returns>
 */
stock bool: ChatGroup_IsValid(ChatGroup: id)
{
    return Iter_Contains(Chat_Groups, _:id);
}

/** 
 * <library>messages</library>
 * <summary>Deletes a group</summary>
 * <param name="id">Id of the group.</param>
 * <returns>True if deleted, otherwise false.</returns>
 */
stock bool: ChatGroup_Delete(ChatGroup: id)
{
    if(!ChatGroup_IsValid(id))
    {
        Logger_Dbg("__TEXT_DEBUG", "Invalid chat group.");
        return false;
    }
    strdel(ChatGroup_Name[id], 0, strlen(ChatGroup_Name[id]));
    Iter_Clear(ChatGroup_Members[id]);
    Iter_Remove(Chat_Groups, id);
    return true;
}

/** 
 * <library>messages</library>
 * <summary>Adds a member to the group</summary>
 * <param name="id">Id of the group.</param>
 * <param name="id">Player to add.</param>
 * <returns>Error code 1 if channel is invalid, 2 if channel is full, 3 if channel already has a member, 0 if successful.</returns>
 */
stock Error: ChatGroup_AddMember(ChatGroup: id, playerid)
{
    if(!ChatGroup_IsValid(id))
    {
        Logger_Dbg("__TEXT_DEBUG", "Invalid chat group.");
        return Error(1, "Invalid channel.");
    }
    if(Iter_Count(ChatGroup_Members[id]) >= ChatGroup_MaxMembers[id])
    {
        Logger_Dbg("__TEXT_DEBUG", "Chat group is full.", Logger_I("groupid", _:id), Logger_I("count", Iter_Count(ChatGroup_Members[id])), Logger_I("maxmembers", ChatGroup_MaxMembers[id]));
        return Error(2, "Full channel");
    }
    if(Iter_Contains(ChatGroup_Members[id], playerid))
    {
        Logger_Dbg("__TEXT_DEBUG", "Chat group already has member.", Logger_I("groupid", _:id), Logger_I("playerid", playerid));
        return Error(3, "Channel already has member.");
    }
    Iter_Add(ChatGroup_Members[id], playerid);
    return Ok();
}

/** 
 * <library>messages</library>
 * <summary>Removes a member from the group</summary>
 * <param name="id">Id of the group.</param>
 * <param name="id">Player to remove.</param>
 * <returns>Error code 1 if channel is invalid, 2 if channel doesn't have the member, 0 if successful.</returns>
 */
stock Error: ChatGroup_RemoveMember(ChatGroup: id, playerid)
{
    if(!ChatGroup_IsValid(id))
    {
        Logger_Dbg("__TEXT_DEBUG", "Invalid chat group.");
        return Error(1, "Invalid channel.");
    }
    if(!Iter_Contains(ChatGroup_Members[id], playerid))
    {
        Logger_Dbg("__TEXT_DEBUG", "Chat group doesn't have the member.", Logger_I("groupid", _:id), Logger_I("playerid", playerid));
        return Error(2, "Channel doesn't have the member.");
    }
    Iter_Remove(ChatGroup_Members[id], playerid);
    return Ok();
}

/** 
 * <library>messages</library>
 * <summary>Sends a message to group.</summary>
 * <param name="id">Id of the group.</param>
 * <param name="color">Color of the message.</param>
 * <param name="message">Message to send.</param>
 * <returns>False if group is invalid, otherwise true.</returns>
 */
stock bool: ChatGroup_SendMessage(ChatGroup: id, color, const string: message[144], OPEN_MP_TAGS: ...)
{
    if(!ChatGroup_IsValid(id))
    {
        Logger_Dbg("__TEXT_DEBUG", "Invalid chat group.");
        return false;
    }
    foreach(new i : ChatGroup_Members[id])
    {
        SendClientMessage(i, color, message, ___(3));
    }
    return true;
}

// --
static stock bool: json_Parse()
{
    new 
        Node: node,
        bool: data;

    new ret;

    ret = JSON_ParseFile("text.json", node);
    if(ret) 
    {
        Logger_Err("Couldn't parse file.", Logger_S("file", "text.json"));
        return false;
    }

    ret = JSON_GetBool(node, "debugging", data);
    if(ret)
    {
        Logger_Log("Couldn't find key.", Logger_S("key", "logging"));
        return false;
    }
    Text_DebugLevel = data;
    Logger_ToggleDebug("__TEXT_DEBUG", data);

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

    Logger_Dbg("__TEXT_DEBUG", "Parsing option.", Logger_S("option", "allow_private_msgs_status"));
    ret = JSON_GetBool(node, "allow_private_msgs_status", data);
    if(ret)
    {
        Logger_Log("Couldn't parse option.", Logger_S("option", "allow_private_msgs_status"));
        return false;
    }
    Text_BooleanConfig[TEXT_CONFIG_ALLOW_PM_STATUS] = data;
    Logger_Dbg("__TEXT_DEBUG", "Successfully parsed option.", Logger_S("option", "allow_private_msgs_status"));

    Logger_Dbg("__TEXT_DEBUG", "Finished parsing from file.", Logger_S("file", "text.json"));
    printf("[warboot-text-module] Version %s successfully loaded.", __INC_VERSION__);
    return true;
}

static stock json_Save()
{
    Logger_Dbg("__TEXT_DEBUG", "Storing data to file...", Logger_S("file", "text.json"));
    new Node:node = JSON_Object(
        "debugging", JSON_Bool(Text_DebugLevel),
        "call_opmm", JSON_Bool(bool:Text_BooleanConfig[TEXT_CONFIG_CALL_OPMM]),
        "allow_mentions", JSON_Bool(bool:Text_BooleanConfig[TEXT_CONFIG_ALLOW_MENTIONS]),
        "call_oprpm", JSON_Bool(bool:Text_BooleanConfig[TEXT_CONFIG_CALL_OPRPM]),
        "allow_private_msgs_status", JSON_Bool(bool:Text_BooleanConfig[TEXT_CONFIG_ALLOW_PM_STATUS])
    ); 
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
    else if(config == TEXT_CONFIG_ALLOW_PM_STATUS)
    {
        strcopy(dest, "allow_private_msgs_status");
    }
}

forward __text_randomSend(playerid, color, type, time, times);
public __text_randomSend(playerid, color, type, time, times)
{
    if(type == 1)
    {
        if(times != 0)
        {
            new RandomMessage: rndx = RandomMessage: Iter_Random(Messages_Iterator);
            SendClientMessageToAll(-1, Random_MessageText[rndx]);
            times --;
            SetTimerEx("__text_randomSend", time, false, "iiiii", -1, color, 1, time, times);
        }
    }
    if(type == 2)
    {
        if(times != 0)
        {
            new RandomMessage: rndx = RandomMessage: Iter_Random(Messages_Iterator);
            SendClientMessageToAll(-1, Random_MessageText[rndx]);
            times --;
            SetTimerEx("__text_randomSend", time, false, "iiiii", playerid, color, 2, time, times);        
        }
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
