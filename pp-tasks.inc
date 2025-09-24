#if defined _INC_y_timers
	#error pp-tasks is not compatible with y_timers
#endif

// built-in include guard removal
// just in case the user has a local dependency with the same file name
#if defined _inc_pp_tasks
	#undef _inc_pp_tasks
#endif

// custom include-guard to ensure we don't duplicate
#if defined _pp_tasks_included
	#endinput
#endif
#define _pp_tasks_included

#if !defined _PawnPlus_included
	#tryinclude <PawnPlus>
	#if !defined _PawnPlus_included
		#error "You must include PawnPlus before using pp-tasks."
	#endif
#endif

/**
 * <library name="pp-tasks" summary="Lightweight SA-MP library for scheduling global and player-specific tasks.">
 *   <license>
 *     This Source Code Form is subject to the terms of the Mozilla Public License,
 *     v. 2.0. If a copy of the MPL was not distributed with this file, You can
 *     obtain one at http://mozilla.org/MPL/2.0/.
 *
 *     The original code is copyright (c) r4sheed.
 *   </license>
 *   <summary pawndoc="true">
 *     This library provides a simple task system on top of PawnPlus. It allows
 *     defining global and per-player tasks that run on specified intervals.
 *   </summary>
 * </library>
 */

 /// <p/>

#pragma tabsize 4

/**
 * <library>pp-tasks</library>
 * <summary>
 *     Defines a player task that runs on an interval while the player is connected.
 * </summary>
 * <param name="name">The name of the task function.</param>
 * <param name="interval">Interval in milliseconds.</param>
 * <param name="playerid">The player ID passed to the task function.</param>
 */
#define ptask%0[%1](%2) \
	forward @pPT_%0_%1(%2); \
	public @pPT_%0_%1(%2)

/**
 * <library>pp-tasks</library>
 * <summary>
 *     Strips spaces from player task function names, replacing with underscores.
 * </summary>
 */
#define @pPT_%8\32; @pPT_

/**
 * <library>pp-tasks</library>
 * <summary>
 *     Defines a global task that runs on an interval regardless of players.
 * </summary>
 * <param name="name">The name of the task function.</param>
 * <param name="interval">Interval in milliseconds.</param>
 */
#define task%0[%1]() \
	forward @pT_%0_%1(); \
	public @pT_%0_%1()

/**
 * <library>pp-tasks</library>
 * <summary>
 *     Strips spaces from global task function names, replacing with underscores.
 * </summary>
 */
#define @pT_%8\32; @pT_

/**
 * <library>pp-tasks</library>
 * <summary>
 *     Sets a repeating timer by function name with additional parameters.
 * </summary>
 * <param name="functionName">Public function name to call.</param>
 * <param name="interval">Interval in milliseconds.</param>
 * <param name="repeating">True if repeating, otherwise false.</param>
 * <param name="specifiers">Format specifiers for additional parameters.</param>
 * <param name="...">Additional parameters matching the specifiers.</param>
 * <returns>The timer ID, otherwise <b><c>0</c></b>.</returns>
 */
native SetTimerExStr(
	ConstAmxString:functionName,
	interval,
	bool:repeating,
	const specifiers[] = "",
	OPEN_MP_TAGS:...
) = SetTimerEx;

/**
 * <library>pp-tasks</library>
 * <summary>
 *     Sets a simple repeating timer by function name.
 * </summary>
 * <param name="functionName">Public function name to call.</param>
 * <param name="interval">Interval in milliseconds.</param>
 * <param name="repeating">True if repeating, otherwise false.</param>
 * <returns>The timer ID, otherwise <b><c>0</c></b>.</returns>
 */
native SetTimerStr(
	ConstAmxString:functionName,
	const interval,
	bool:repeating
) = SetTimer;

/**
 * <library>pp-tasks</library>
 * <summary>
 *     Stores all defined player tasks.
 * </summary>
 */
static Map:INC_gsPlayerTasks;

/**
 * <library>pp-tasks</library>
 * <summary>
 *     Stores active timers per player.
 * </summary>
 */
static Map:INC_gsPlayerTimers;

/**
 * <library>pp-tasks</library>
 * <summary>
 *     Starts all cached player tasks for a specific player.
 * </summary>
 * <param name="playerid">The player ID.</param>
 */
static void:PPTasks_StartPlayerTasks(playerid)
{
	if (map_size(INC_gsPlayerTasks) == 0) 
		return;

	new List:list = list_new();
	map_add(INC_gsPlayerTimers, playerid, list);

	new Iter:it = map_iter(INC_gsPlayerTasks);
	while (iter_inside(it))
	{
		new timer = SetTimerExStr(
			iter_get_key_str_s(it),
			iter_get(it),
			true,
			"d",
			playerid
		);
		
		if (timer)
			list_add(list, timer);

		iter_move_next(it);
	}
}

/**
 * <library>pp-tasks</library>
 * <summary>
 *     Stops and cleans up all running tasks for a specific player.
 * </summary>
 * <param name="playerid">The player ID.</param>
 */
static void:PPTasks_StopPlayerTasks(playerid)
{
	if (!map_has_key(INC_gsPlayerTimers, playerid))
		return;
		
	new List:list;
	if (!map_get_safe(INC_gsPlayerTimers, playerid, list)) 
		return;

	new size = list_size(list);
	for (new i = 0; i < size; i++)
		KillTimer(list_get(list, i));

	list_delete(list);
	map_remove_deep(INC_gsPlayerTimers, playerid);
}

/**
 * <library>pp-tasks</library>
 * <summary>
 *     Initializes global tasks by scanning publics and starting timers.
 * </summary>
 * <returns>Always returns <b><c>1</c></b>.</returns>
 */
forward _pp@on_init@PP_Tasks();
public _pp@on_init@PP_Tasks()
{
	new buffer[16 + 1];

	new num = amx_num_publics();
	for (new i = 0; i < num; i++)
	{
		new String:publicName = amx_public_name_s(i);

		new List:regex = str_extract(
			publicName,
			"^@pT_(.*?)_(\\d+)$"
		);

		if (!regex) 
			continue;

		list_get_str(regex, 2, buffer);

		new interval = strval(buffer);
		if (interval > 0)
			SetTimerStr(publicName, interval, true);

		list_delete_deep(regex);
	}
	return 1;
}

/**
 * <library>pp-tasks</library>
 * <summary>
 *     Initializes player tasks by scanning publics and caching intervals.
 * </summary>
 * <returns>Always returns <b><c>1</c></b>.</returns>
 */
forward _pp@on_init@PP_PlayerTasks();
public _pp@on_init@PP_PlayerTasks()
{
	INC_gsPlayerTasks  = map_new();
	INC_gsPlayerTimers = map_new();

	new buffer[16 + 1];

	new num = amx_num_publics();
	for (new i = 0; i < num; i++)
	{
		new List:regex = str_extract(
			amx_public_name_s(i),
			"^@pPT_(.*?)_(\\d+)$"
		);

		if (!regex) 
			continue;

		list_get_str(regex, 2, buffer);

		new interval = strval(buffer);
		if (interval > 0)
			map_str_s_set(INC_gsPlayerTasks, list_get_str_s(regex, 0), interval);

		list_delete_deep(regex);
	}

	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		if (IsPlayerConnected(i) && !IsPlayerNPC(i))
			PPTasks_StartPlayerTasks(i);
	}

	return 1;
}

/**
 * <library>pp-tasks</library>
 * <summary>
 *     Starts all tasks when a player connects.
 * </summary>
 * <param name="playerid">The player ID.</param>
 */
public OnPlayerConnect(playerid)
{
	if (!IsPlayerNPC(playerid)) 
		PPTasks_StartPlayerTasks(playerid);

	#if defined PPTasks_OnPlayerConnect
		return PPTasks_OnPlayerConnect(playerid);
	#else
		return 1;
	#endif
}
#if defined _ALS_OnPlayerConnect
	#undef OnPlayerConnect
#else
	#define _ALS_OnPlayerConnect
#endif
#define OnPlayerConnect PPTasks_OnPlayerConnect
#if defined PPTasks_OnPlayerConnect
	forward PPTasks_OnPlayerConnect(playerid);
#endif

/**
 * <library>pp-tasks</library>
 * <summary>
 *     Stops all tasks when a player disconnects.
 * </summary>
 * <param name="playerid">The player ID.</param>
 * <param name="reason">Disconnect reason.</param>
 */
public OnPlayerDisconnect(playerid, reason)
{
	if (!IsPlayerNPC(playerid)) 
		PPTasks_StopPlayerTasks(playerid);

	#if defined PPTasks_OnPlayerDisconnect
		return PPTasks_OnPlayerDisconnect(playerid, reason);
	#else
		return 1;
	#endif
}
#if defined _ALS_OnPlayerDisconnect
	#undef OnPlayerDisconnect
#else
	#define _ALS_OnPlayerDisconnect
#endif
#define OnPlayerDisconnect PPTasks_OnPlayerDisconnect
#if defined PPTasks_OnPlayerDisconnect
	forward PPTasks_OnPlayerDisconnect(playerid, reason);
#endif
