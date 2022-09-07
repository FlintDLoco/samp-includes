//made by lokii
 
/*-------------------------
V2:
	Changelog:
	    Now supports rotations, virtual world, interiors added 2 function OpenAutoObject and CloseAutoObject to open and close manually
		                                                                                          and added 2 callback OnAutoObjectOpen and OnAutoObjectClose
-------------------------*/
 
/*
C2.1:
	Removed
-------------------------*/
 
/*-------------------------
V2.2
	Changelog:
		doesnt req y_hooks anymore and also supports npc's
-------------------------*/
 
#if defined _AutoMove_included
	  #endinput
#define _AutoMove_included
#endif
 
/*
native CreateAutoObject(id, modelid, Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz, worldid = -1, interiorid = -1, Float:stream_distance, Float:move_distance, Float:tox, Float:toy, Float:toz, Float:torx, Float:tory, Float:torz, Float:move_speed);
native OpenAutoObject(id);
native CloseAutoObject(id)
native DestroyAutoObject(id);
native DestroyAllAutoObjects();
*/
 
#include <streamer>
 
#define AutoMoveVersion "2.2"
 
#define MAX_AUTO_OBJECTS 1000
 
enum I_AutoMoveX
{
	Float:am_posx,
	Float:am_posy,
	Float:am_posz,
	Float:am_posrx,
	Float:am_posry,
	Float:am_posrz,
    Float:am_postox,
    Float:am_postoy,
    Float:am_postoz,
    Float:am_postorx,
    Float:am_postory,
    Float:am_postorz,
    Float:am_speed,
	am_areaid,
	am_objectid,
	bool:am_used,
	bool:am_exist,
	am_count
}
new I_AutoMove[MAX_AUTO_OBJECTS][I_AutoMoveX];
 
stock CreateAutoObject(id, modelid, Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz, worldid = -1, interiorid = -1, Float:stream_distance, Float:move_distance, Float:tox, Float:toy, Float:toz, Float:torx, Float:tory, Float:torz, Float:move_speed)
{
	 DestroyDynamicObject(I_AutoMove[id][am_objectid]);
     I_AutoMove[id][am_posx] = x;
     I_AutoMove[id][am_posy] = y;
     I_AutoMove[id][am_posz] = z;
     I_AutoMove[id][am_posrx] = rx;
     I_AutoMove[id][am_posry] = ry;
     I_AutoMove[id][am_posrz] = rz;
     I_AutoMove[id][am_postox] = tox;
     I_AutoMove[id][am_postoy] = toy;
     I_AutoMove[id][am_postoz] = toz;
     I_AutoMove[id][am_postorx] = torx;
     I_AutoMove[id][am_postory] = tory;
     I_AutoMove[id][am_postorz] = torz;
     I_AutoMove[id][am_speed] = move_speed;
	 I_AutoMove[id][am_objectid] = CreateDynamicObject(modelid, x, y, z, rx, ry, rz, worldid, interiorid, -1, stream_distance);
     I_AutoMove[id][am_areaid] = CreateDynamicSphere(x, y, z, move_distance, worldid, interiorid);
     I_AutoMove[id][am_count] = 0;
     I_AutoMove[id][am_exist] = true;
	 return 1;
}
 
stock OpenAutoObject(id)
{
	MoveDynamicObject(I_AutoMove[id][am_objectid], I_AutoMove[id][am_postox], I_AutoMove[id][am_postoy], I_AutoMove[id][am_postoz], I_AutoMove[id][am_speed], I_AutoMove[id][am_postorx], I_AutoMove[id][am_postory], I_AutoMove[id][am_postorz]);
    I_AutoMove[id][am_used] = true;
    return 1;
}
 
stock CloseAutoObject(id)
{
	MoveDynamicObject(I_AutoMove[id][am_objectid], I_AutoMove[id][am_posx], I_AutoMove[id][am_posy], I_AutoMove[id][am_posz], I_AutoMove[id][am_speed], I_AutoMove[id][am_posrx], I_AutoMove[id][am_posry], I_AutoMove[id][am_posrz]);
    I_AutoMove[id][am_used] = false;
	return 1;
}
 
stock DestroyAutoObject(id)
{
	 DestroyDynamicObject(I_AutoMove[id][am_objectid]);
     DestroyDynamicArea(I_AutoMove[id][am_areaid]);
     I_AutoMove[id][am_count] = 0;
     I_AutoMove[id][am_exist] = false;
	 return 1;
}
 
stock DestroyAllAutoObjects()
{
	 for(new lll_i = 0; lll_i < MAX_AUTO_OBJECTS; lll_i ++)
	 {
	     if(!I_AutoMove[lll_i][am_exist]) continue;
		 DestroyAutoObject(lll_i);
	 }
	 return 1;
}
 
forward OnAutoObjectOpen(playerid, auto_objectid);
 
forward OnAutoObjectClose(playerid, auto_objectid);
 
 /*
public OnPlayerEnterDynamicArea(playerid, areaid)
{
	  if(IsPlayerConnected(playerid))
	  {
			for(new j = 0; j < MAX_AUTO_OBJECTS;j++)
			if(areaid == I_AutoMove[j][am_areaid])
			{
   				if(!I_AutoMove[j][am_used])
 				{
					I_AutoMove[j][am_count] ++;
					if(I_AutoMove[j][am_count] < 2)
   					{
			 	 		MoveDynamicObject(I_AutoMove[j][am_objectid], I_AutoMove[j][am_postox], I_AutoMove[j][am_postoy], I_AutoMove[j][am_postoz], I_AutoMove[j][am_speed], I_AutoMove[j][am_postorx], I_AutoMove[j][am_postory], I_AutoMove[j][am_postorz]);
			 	 		CallLocalFunction("OnAutoObjectOpen", "ii", playerid, j);
 	 					I_AutoMove[j][am_used] = true;
 	 					break;
					}
	  			}
	  		}
	  }
	  #if defined AM_OnPlayerEnterDynamicArea
		return OnPlayerEnterDynamicArea(playerid, areaid);
	#else
		return 1;
	#endif
}
 
public OnPlayerLeaveDynamicArea(playerid, areaid)
{
	  if(IsPlayerConnected(playerid))
	  {
			for(new j = 0; j < MAX_AUTO_OBJECTS;j++)
			if(areaid == I_AutoMove[j][am_areaid])
			{
				if(I_AutoMove[j][am_used])
				{
   					I_AutoMove[j][am_count] --;
		     		if(I_AutoMove[j][am_count] < 1)
		     		{
					 		MoveDynamicObject(I_AutoMove[j][am_objectid], I_AutoMove[j][am_posx], I_AutoMove[j][am_posy], I_AutoMove[j][am_posz], I_AutoMove[j][am_speed], I_AutoMove[j][am_posrx], I_AutoMove[j][am_posry], I_AutoMove[j][am_posrz]);
					 		CallLocalFunction("OnAutoObjectClose", "ii", playerid, j);
                     		I_AutoMove[j][am_used] = false;
                     		break;
					}
				}
	  		}
	  }
	  #if defined AM_OnPlayerLeaveDynamicArea
		return OnPlayerLeaveDynamicArea(playerid, areaid);
	#else
		return 1;
	#endif
}
 */
#if defined _ALS_OnPlayerEnterDynamicArea
#undef OnPlayerEnterDynamicArea
#else
#define _ALS_OnPlayerEnterDynamicArea
#endif
#define OnPlayerEnterDynamicArea AM_OnPlayerEnterDynamicArea
 
#if defined AM_OnPlayerEnterDynamicArea
forward AM_OnPlayerEnterDynamicArea(playerid, areaid);
#endif
 
#if defined _ALS_OnPlayerLeaveDynamicArea
#undef OnPlayerLeaveDynamicArea
#else
#define _ALS_OnPlayerLeaveDynamicArea
#endif
#define OnPlayerLeaveDynamicArea AM_OnPlayerLeaveDynamicArea
 
#if defined AM_OnPlayerLeaveDynamicArea
forward AM_OnPlayerLeaveDynamicArea(playerid, areaid);
#endif
 
#if defined FILTERSCRIPT
 
public OnFilterScriptExit()
{
     DestroyAllAutoObjects();
	 #if defined AM_OnFilterScriptExit
		return AM_OnFilterScriptExit();
	#else
		return 1;
	#endif
}
 
#if defined _ALS_OnFilterScriptExit
#undef OnFilterScriptExit
#else
#define _ALS_OnFilterScriptExit
#endif
#define OnFilterScriptExit AM_OnFilterScriptExit
 
#if defined AM_OnFilterScriptExit
forward AM_OnFilterScriptExit();
#endif
 
#else
 
public OnGameModeExit()
{
	 DestroyAllAutoObjects();
	 #if defined AM_OnGameModeExit
		return AM_OnGameModeExit();
	#else
		return 1;
	#endif
}
 
#if defined _ALS_OnGameModeExit
#undef OnGameModeExit
#else
#define _ALS_OnGameModeExit
#endif
#define OnGameModeExit AM_OnGameModeExit
 
#if defined AM_OnGameModeExit
forward AM_OnGameModeExit();
#endif
 
#endif
 
 
//EOF.