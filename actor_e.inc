/*
 *  Extended actor functions by Emmet.
 *  Updated by Admigo
 *
 *  Features:
 *    Many useful functions for more actor control
 *    Automatically preloads animations for an actor
 *    Damage is automatically deducted
 *    Attach 3D labels to an actor
 *    Lots more!
 *
 *  Update 08.04.2016
 *  - Added SetActorName(idactor, name[]);
 *  - Added GetActorName(idactor);
 *  - Added ToggleActorName(idactor, toggle);
 *  - Added SetActorChatBubble(idactor, text[], color, Float:drawdistance, expiretime);
 *
 *
 *  Creation date: May 8, 2015
*/



#define MAX_ACTOR_NAME 24
#define ACTOR_NAME_DRAW_DISTANCE (10.0)


// Resynchronize an actor.
forward ResyncActor(idactor);
// Respawns an actor.
forward RespawnActor(idactor);
// Checks if the actor is dead.
forward IsActorDead(idactor);
// Sets an actor's respawn time (after death).
forward SetActorRespawnTime(idactor, time);
// Checks if a player is in range of an actor.
forward IsPlayerInRangeOfActor(playerid, idactor, Float:radius = 5.0);
// Attaches a 3D text label to an actor.
forward Text3D:Attach3DTextLabelToActor(idactor, text[], color, Float:fOffsetX, Float:fOffsetY, Float:fOffsetZ, Float:distance = 10.0, worldid = 0, testlos = 0);

// Called when an actor dies.
forward OnActorDeath(idactor, killerid, reason);
// Called when an actor spawns.
forward OnActorSpawn(idactor);
// Called when a player aims at an actor.
forward OnPlayerTargetActor(playerid, newtarget, oldtarget);

// List of animation libraries used for preloading.
static s_AnimationLibraries[][] = {
	!"AIRPORT",    !"ATTRACTORS",   !"BAR", 		!"BASEBALL",
	!"BD_FIRE",    !"BEACH", 	    !"BENCHPRESS",  !"BF_INJECTION",
	!"BIKED", 	   !"BIKEH", 	    !"BIKELEAP", 	!"BIKES",
	!"BIKEV", 	   !"BIKE_DBZ",     !"BMX", 		!"BOMBER",
	!"BOX", 	   !"BSKTBALL",     !"BUDDY", 		!"BUS",
	!"CAMERA", 	   !"CAR",          !"CARRY", 		!"CAR_CHAT",
	!"CASINO",	   !"CHAINSAW",     !"CHOPPA", 		!"CLOTHES",
	!"COACH", 	   !"COLT45",       !"COP_AMBIENT", !"COP_DVBYZ",
	!"CRACK", 	   !"CRIB",         !"DAM_JUMP", 	 !"DANCING",
	!"DEALER", 	   !"DILDO",        !"DODGE", 	 	 !"DOZER",
	!"DRIVEBYS",   !"FAT",          !"FIGHT_B", 	 !"FIGHT_C",
	!"FIGHT_D",    !"FIGHT_E",      !"FINALE", 		 !"FINALE2",
	!"FLAME",      !"FLOWERS",      !"FOOD", 	 	 !"FREEWEIGHTS",
	!"GANGS",      !"GHANDS",       !"GHETTO_DB", 	 !"GOGGLES",
	!"GRAFFITI",   !"GRAVEYARD",    !"GRENADE", 	 !"GYMNASIUM",
	!"HAIRCUTS",   !"HEIST9",       !"INT_HOUSE", 	 !"INT_OFFICE",
	!"INT_SHOP",   !"JST_BUISNESS", !"KART", 		 !"KISSING",
	!"KNIFE",      !"LAPDAN1", 		!"LAPDAN2", 	 !"LAPDAN3",
	!"LOWRIDER",   !"MD_CHASE", 	!"MD_END", 	 	 !"MEDIC",
	!"MISC",       !"MTB", 			!"MUSCULAR", 	 !"NEVADA",
	!"ON_LOOKERS", !"OTB", 			!"PARACHUTE", 	 !"PARK",
	!"PAULNMAC",   !"PED", 			!"PLAYER_DVBYS", !"PLAYIDLES",
	!"POLICE",     !"POOL", 		!"POOR", 		 !"PYTHON",
	!"QUAD",       !"QUAD_DBZ", 	!"RAPPING", 	 !"RIFLE",
	!"RIOT",       !"ROB_BANK", 	!"ROCKET",	 	 !"RUSTLER",
	!"RYDER",      !"SCRATCHING", 	!"SHAMAL", 		 !"SHOP",
	!"SHOTGUN",    !"SILENCED", 	!"SKATE", 		 !"SMOKING",
	!"SNIPER",     !"SPRAYCAN", 	!"STRIP", 		 !"SUNBATHE",
	!"SWAT",       !"SWEET", 		!"SWIM", 		 !"SWORD",
	!"TANK",       !"TATTOOS",	 	!"TEC", 		 !"TRAIN",
	!"TRUCK",      !"UZI", 			!"VAN", 		 !"VENDING",
	!"VORTEX",     !"WAYFARER", 	!"WEAPONS", 	 !"WUZI",
	!"WOP",        !"GFUNK", 		!"RUNNINGMAN"
};

static s_ActorRespawnTimer[MAX_ACTORS] = {-1, ...};
static s_ActorChatBubbleTimer[MAX_ACTORS] = {-1, ...};
static s_TargetActor[MAX_PLAYERS] = {INVALID_ACTOR_ID, ...};
static s_3DTextidactor[MAX_3DTEXT_GLOBAL] = {INVALID_ACTOR_ID, ...};

enum Actor_data
{
	a_Name[MAX_ACTOR_NAME],
	a_Color,
	Text3D:a_NameTag,
	Text3D:a_ChatBubble,
	bool:a_NameTagCreated,
	bool:a_ChatBubbleCreated,
	bool:a_ShowNameTag
}
new ActorInfo[MAX_ACTORS][Actor_data];


stock static PreloadActorAnimations(idactor)
{
	for(new i = 0; i < sizeof(s_AnimationLibraries); i ++)
	{
	    ApplyActorAnimation(idactor, s_AnimationLibraries[i], "null", 0.0, 0, 0, 0, 0, 0);
	}
}

stock ResyncActor(idactor)
{
	if(IsValidActor(idactor))
	{
	    new
	        Float:x,
	        Float:y,
	        Float:z,
			worldid = GetActorVirtualWorld(idactor);
	        
		// Get the actor's position.
		GetActorPos(idactor, x, y, z);
		// Set their position and world to some arbitrary values.
		SetActorPos(idactor, 1000.0, -2000.0, 500.0);
		SetActorVirtualWorld(idactor, random(cellmax));
		// We need a timer to defer the actions. Doing it right away doesn't seem to work.
		SetTimerEx("RestoreActor", 850, 0, "iifff", idactor, worldid, x, y, z);
		return 1;
	}
	return 0;
}

forward RestoreActor(idactor, worldid, Float:x, Float:y, Float:z);
public RestoreActor(idactor, worldid, Float:x, Float:y, Float:z)
{
	SetActorVirtualWorld(idactor, worldid);
	SetActorPos(idactor, x, y, z);
	return 1;
}

// That's right, a public function. See "SetActorRespawnTime" for details.
forward RespawnActor(idactor);
public RespawnActor(idactor)
{
	if(IsValidActor(idactor))
	{
	    // Reset the timer variable if set.
	    s_ActorRespawnTimer[idactor] = -1;
		s_ActorChatBubbleTimer[idactor] = -1;
	    // Restore the actor's health.
	    SetActorHealth(idactor, 100.0);
	    // Clear the death animation.
	    ClearActorAnimations(idactor);
	    // Resync the actor.
		ResyncActor(idactor);
		// Call "OnActorSpawn".
		CallLocalFunction("OnActorSpawn", "i", idactor);
		return 1;
	}
	return 0;
}

forward DestroyChatBubble(idactor);
public DestroyChatBubble(idactor)
{
	if(IsValidActor(idactor))
	{
		// Reset the variables.
		ActorInfo[idactor][a_ChatBubbleCreated] = false;
	    // Delete the 3D Text Label
		Delete3DTextLabel(ActorInfo[idactor][a_ChatBubble]);
		// Reset the timer variable if set.
		s_ActorChatBubbleTimer[idactor] = -1;
		//return 1;
	}
	return 1;
}

stock IsActorDead(idactor)
{
	if(IsValidActor(idactor))
	{
	    new
	        Float:health;
	
	    // Get the actor's health.
	    GetActorHealth(idactor, health);
	    return (health <= 0.0);
	}
	return 0;
}

stock SetActorRespawnTime(idactor, time)
{
	if(IsValidActor(idactor))
	{
	    if(s_ActorRespawnTimer[idactor] != -1)
	    {
	        // Kill any previously created timer.
	        KillTimer(s_ActorRespawnTimer[idactor]);
		}
		s_ActorRespawnTimer[idactor] = SetTimerEx("RespawnActor", time, 0, "i", idactor);
		return 1;
	}
	return 0;
}

stock IsPlayerInRangeOfActor(playerid, idactor, Float:radius = 5.0)
{
	new
	    Float:x,
	    Float:y,
	    Float:z;

	if (GetActorPos(idactor, x, y, z))
	{
	    return IsPlayerInRangeOfPoint(playerid, radius, x, y, z) && GetPlayerVirtualWorld(playerid) == GetActorVirtualWorld(idactor);
	}
	return 0;
}

stock Text3D:Attach3DTextLabelToActor(idactor, text[], color, Float:fOffsetX, Float:fOffsetY, Float:fOffsetZ, Float:distance = 10.0, worldid = 0, testlos = 0)
{
	new Text3D:textid = Text3D:INVALID_3DTEXT_ID;

	if(IsValidActor(idactor))
	{
	    new
			Float:x,
			Float:y,
			Float:z;

		GetActorPos(idactor, x, y, z);
		
		textid = Create3DTextLabel(text, color, x + fOffsetX, y + fOffsetY, (z + 0.8) + fOffsetZ, distance, worldid, testlos);
		
		if(textid != Text3D:INVALID_3DTEXT_ID)
		{
			s_3DTextidactor[_:textid] = idactor;
		}
	}
	return textid;
}

stock SetActorName(idactor, name[])
{
	if(IsValidActor(idactor))
	{
		new
			Float:x,
			Float:y,
			Float:z,
			worldid = GetActorVirtualWorld(idactor);
		GetActorPos(idactor, x, y, z);
	    format(ActorInfo[idactor][a_Name],MAX_ACTOR_NAME,"%s",name);
		ActorInfo[idactor][a_ShowNameTag] = true;
		if(ActorInfo[idactor][a_NameTagCreated] == false)
		{
			ActorInfo[idactor][a_NameTag] = Create3DTextLabel(ActorInfo[idactor][a_Name], ActorInfo[idactor][a_Color], x, y, (z + 1.0), ACTOR_NAME_DRAW_DISTANCE, worldid, 0);
			ActorInfo[idactor][a_NameTagCreated] = true;
		}
		else
		{
			Update3DTextLabelText(ActorInfo[idactor][a_NameTag],ActorInfo[idactor][a_Color],ActorInfo[idactor][a_Name]);
		}
	}
	return 1;
}

stock GetActorName(idactor)
{
	new aname[MAX_ACTOR_NAME];
	if(IsValidActor(idactor))
	{
		format(aname, sizeof(aname), "%s",ActorInfo[idactor][a_Name]);
		return aname;
	}
	return aname;
}

stock SetActorColor(idactor, color)
{
	ActorInfo[idactor][a_Color]=color;
	if(ActorInfo[idactor][a_NameTagCreated] == true)
	{
		Update3DTextLabelText(ActorInfo[idactor][a_NameTag],ActorInfo[idactor][a_Color],ActorInfo[idactor][a_Name]);
	}
	return 1;
}

stock ToggleActorName(idactor,bool:toggle)
{
	ActorInfo[idactor][a_ShowNameTag] = toggle;
	if(ActorInfo[idactor][a_ShowNameTag] == true)
	{
		SetActorName(idactor, ActorInfo[idactor][a_Name]);
	}
	else
	{
		Delete3DTextLabel(ActorInfo[idactor][a_NameTag]);
		ActorInfo[idactor][a_NameTagCreated] = false;
	}
	return 1;
}

stock SetActorChatBubble(idactor, text[], color, Float:drawdistance, expiretime)
{
	if(IsValidActor(idactor))
	{
		new
			Float:x,
			Float:y,
			Float:z,
			worldid = GetActorVirtualWorld(idactor);

		GetActorPos(idactor, x, y, z);
		if(ActorInfo[idactor][a_ChatBubbleCreated] == false)
		{
			ActorInfo[idactor][a_ChatBubble] = Create3DTextLabel(text, color, x, y, (z + 1.2), drawdistance, worldid, 0);
			ActorInfo[idactor][a_ChatBubbleCreated] = true;
		}
		else
		{
			Update3DTextLabelText(ActorInfo[idactor][a_ChatBubble],color,text);
		}
		s_ActorChatBubbleTimer[idactor] = SetTimerEx("DestroyChatBubble", expiretime, 0, "i", idactor);
		return 1;
	}
	return 1;
}

stock AC_CreateActor(modelid, Float:x, Float:y, Float:z, Float:rotation)
{
	new
	    idactor = INVALID_ACTOR_ID;

	if(0 <= modelid <= 311)
	{
		idactor = CreateActor(modelid, x, y, z, rotation);

		if (idactor != INVALID_ACTOR_ID)
		{
		    // Reset the timer variable.
		    s_ActorRespawnTimer[idactor] = -1;
			// Set Actor color to white
			ActorInfo[idactor][a_Color]= 0xFFFFFFFF;
		    // Preload their animations.
		    PreloadActorAnimations(idactor);
		    // Let them know that they've spawned.
		    CallLocalFunction("OnActorSpawn", "i", idactor);
		}
	}
	return idactor;
}

stock AC_DestroyActor(idactor)
{
	if(IsValidActor(idactor))
	{
	    for(new i = 0; i < MAX_3DTEXT_GLOBAL; i ++)
	    {
	        if(s_3DTextidactor[i] == idactor)
	        {
	            // Destroy the 3D text label.
	            Delete3DTextLabel(Text3D:i);
				Delete3DTextLabel(ActorInfo[idactor][a_NameTag]);
				Delete3DTextLabel(ActorInfo[idactor][a_ChatBubble]);
	            // Reset the variables.
	            s_3DTextidactor[i] = INVALID_ACTOR_ID;
				ActorInfo[idactor][a_NameTagCreated] = false;
				ActorInfo[idactor][a_ChatBubbleCreated] = false;
				ActorInfo[idactor][a_Color]= 0xFFFFFFFF;
			}
		}
		return DestroyActor(idactor);
	}
	return 0;
}

stock AC_Delete3DTextLabel(Text3D:textid)
{
	if(textid != Text3D:INVALID_3DTEXT_ID)
	{
	    // Reset the variable.
	    s_3DTextidactor[_:textid] = INVALID_ACTOR_ID;
		return Delete3DTextLabel(textid);
	}
	return 0;
}

public OnPlayerUpdate(playerid)
{
	new target_actor = GetPlayerTargetActor(playerid);
	
	if (s_TargetActor[playerid] != target_actor)
	{
	    CallLocalFunction("OnPlayerTargetActor", "iii", playerid, target_actor, s_TargetActor[playerid]);
	
	    s_TargetActor[playerid] = target_actor;
	}
	#if defined AC_OnPlayerUpdate
	    return AC_OnPlayerUpdate(playerid);
	#else
	    return 1;
	#endif
}


#if defined _ALS_CreateActor
	#undef CreateActor
#else
	#define _ALS_CreateActor
#endif

#define CreateActor AC_CreateActor

#if defined _ALS_DestroyActor
	#undef DestroyActor
#else
	#define _ALS_DestroyActor
#endif

#define DestroyActor AC_DestroyActor

#if defined _ALS_Delete3DTextLabel
	#undef Delete3DTextLabel
#else
	#define _ALS_Delete3DTextLabel
#endif

#define Delete3DTextLabel AC_Delete3DTextLabel

#if defined _ALS_OnPlayerUpdate
	#undef OnPlayerUpdate
#else
	#define _ALS_OnPlayerUpdate
#endif

#define OnPlayerUpdate AC_OnPlayerUpdate

#if defined AC_OnPlayerUpdate
	forward AC_OnPlayerUpdate(playerid);
#endif

