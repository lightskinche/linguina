#include "mainheader.h"
//this file is for metamethods for the types we are using
//scene functions
int METAPROC_Index_Scene(lua_State* L) { //basically just allows you to get stuff from the scene struct, no need for c-like prototype here
	s_scene* tmp_scene = lua_touserdata(L, 1);
	char* member = luaL_checkstring(L, 2);
	if (tmp_scene) {
		if (!strcmp(member, "name"))
			lua_pushstring(L, tmp_scene->name);
		else if (!strcmp(member, "on_enter"))
			lua_pushstring(L, tmp_scene->on_enter);
		else if (!strcmp(member, "on_enter"))
			lua_pushstring(L, tmp_scene->on_exit);
		else if (!strcmp(member, "examine"))
			lua_pushstring(L, tmp_scene->examine);
		else if (!strcmp(member, "locations")) {
			lua_pushlightuserdata(L, tmp_scene->locations);
			lua_getfield(L, LUA_REGISTRYINDEX, "locationmap_metatable");
			lua_setmetatable(L, -2); //turns out _that if we don't do this then we can't index or do anything with [scene_name].locations
		}
		else {
			flogf("LUA EXCEPTION: Attempted to index nonexistant member %s in type 'scene'\n", member);
			lua_pushnil(L);
			return 1;
		}
		return 1;
	}
	flogf("LUA EXCEPTION: Attempted to index nil scene\n");
	lua_pushnil(L);
	return 1;
}
int METAPROC_NewIndex_Scene(lua_State* L) { //basically just allows you to set stuff from the scene struct, no need for c-like prototype here
	s_scene* tmp_scene = lua_touserdata(L, 1);
	char* member = luaL_checkstring(L, 2);
	if (tmp_scene) {
		if (!strcmp(member, "name")) {
			char* value = luaL_checkstring(L, 3);
			tmp_scene->name = value;
		}
		else if (!strcmp(member, "on_enter")) {
			char* value = luaL_checkstring(L, 3);
			tmp_scene->on_enter = value;
		}
		else if (!strcmp(member, "on_enter")) {
			char* value = luaL_checkstring(L, 3);
			tmp_scene->on_exit = value;
		}
		else if (!strcmp(member, "examine")) {
			char* value = luaL_checkstring(L, 3);
			tmp_scene->examine = value;
		}
		else if (!strcmp(member, "a")) {
			s_location* locations = lua_touserdata(L, 3);
			tmp_scene->locations = locations;
		}
		else {
			flogf("LUA EXCEPTION: Attempted to set nonexistant member %s in type 'scene'\n", member);
			return 0;
		}
		return 0;
	}
	flogf("LUA EXCEPTION: Attempted to set nil type 'scene'\n");
	return 0;
}
int METAPROC_Tostring_Scene(lua_State* L) { // char* __tostring(s_scene(userdata)* tmp_scene)
	s_scene* tmp_scene = lua_touserdata(L, 1);
	if (tmp_scene) {
		lua_pushstring(L, tmp_scene->name);
		return 1;
	}
	flogf("LUA EXCEPTION: Attempted __tostring on a nil scene\n");
	lua_pushnil(L);
	return 1;
}

//location functions
int METAPROC_Index_LocationMap(lua_State* L) { // location(userdata)* | char* __index(location(userdata)* tmp_location, int | char* member)
	s_location* tmp_location = lua_touserdata(L, 1);
	if (tmp_location) {
		switch (lua_type(L, 2)) {
		case LUA_TNUMBER: //in this case, we want to switch the location userdata is pointing to
			lua_pushlightuserdata(L, &tmp_location[(int)lua_tonumber(L, 2) - 1]);
			return 1;
			break;
		case LUA_TSTRING:
		{
			char* member = luaL_checkstring(L, 2);
			if (!strcmp(member, "examine"))
				lua_pushstring(L, tmp_location->examine);
			else if (!strcmp(member, "on_enter"))
				lua_pushstring(L, tmp_location->on_enter);
			else if (!strcmp(member, "north"))
				lua_pushlightuserdata(L, tmp_location->north);
			else if (!strcmp(member, "south"))
				lua_pushlightuserdata(L, tmp_location->south);
			else if (!strcmp(member, "west"))
				lua_pushlightuserdata(L, tmp_location->west);
			else if (!strcmp(member, "east"))
				lua_pushlightuserdata(L, tmp_location->east);
			else if (!strcmp(member, "locations")) {
				lua_pushlightuserdata(L, tmp_location); //pretty sure we don't need to set a metatable for this
			}
			else{
				flogf("LUA EXCEPTION: Attempted to access member %s which does not exist in a type 'locationmap'\n", member);
				lua_pushnil(L);
				return 1;
			}
			return 1;
		}
		default:
			flogf("LUA EXCEPTION: Attempted to index a type 'locationmap' with a nonnumber and nonstring type\n");
		}
	}
	flogf("LUA EXCEPTION: Attempted to index a nil 'locationmap'\n");
	lua_pushnil(L);
	return 1;
}
int METAPROC_NewIndex_LocationS(lua_State* L) { // location(userdata)* | char* __index(location(userdata)* tmp_location, int | char* member)
	s_location* tmp_location = lua_touserdata(L, 1);
	if (tmp_location) {
		char* member = luaL_checkstring(L, 2);
		if (!strcmp(member, "examine")) {
			char* value = luaL_checkstring(L, 3);
			tmp_location->examine = value;
		}
		else if (!strcmp(member, "on_enter")) {
			char* value = luaL_checkstring(L, 3);
			tmp_location->on_enter = value;
		}
		else if (!strcmp(member, "things")) {
			s_thing* things = lua_touserdata(L, 3);
			tmp_location->things = things;
		}
		else if(strcmp(member, "locations"))
			flogf("LUA EXCEPTION: Attempted to access invalid member %s in type 'location' or 'locationmap'\n", member);	
		return 0;
	}
	flogf("LUA EXCEPTION: Attempted to index a nil type'\n");
	return 0;
}
int METAPROC_Index_Location(lua_State* L) { // char* __indedx(location(userdata)* tmp_location, char* member)
	s_location* tmp_location = lua_touserdata(L, 1);
	char* member = luaL_checkstring(L, 2);
	if (tmp_location) {
		if (!strcmp(member, "examine"))
			lua_pushstring(L, tmp_location->examine);
		else if (!strcmp(member, "on_enter"))
			lua_pushstring(L, tmp_location->on_enter);
		else if (!strcmp(member, "north"))
			lua_pushlightuserdata(L, tmp_location->north);
		else if (!strcmp(member, "south"))
			lua_pushlightuserdata(L, tmp_location->south);
		else if (!strcmp(member, "west"))
			lua_pushlightuserdata(L, tmp_location->west);
		else if (!strcmp(member, "east"))
			lua_pushlightuserdata(L, tmp_location->east);
		else if (!strcmp(member, "location"))
			lua_pushlightuserdata(L, tmp_location);
		else {
			flogf("LUA EXCEPTION: Attempted to access member %s which does not exist in a type 'location'\n", member);
			lua_pushnil(L);
			return 1;
		}
		return 1;
	}
	flogf("LUA EXCEPTION: Attempted to index a nil 'location'\n");
	lua_pushnil(L);
	return 1;
}
int METAPROC_Tostring_LocationS(lua_State* L) { // char* __tostring(s_location(userdata)* tmp_location)
	s_location* tmp_location = lua_touserdata(L, 1);
	if (tmp_location) {
		lua_pushstring(L, tmp_location->examine);
		return 1;
	}
	flogf("LUA EXCEPTION: Attempted __tostring on a nil type"); //don't think these will ever, ever be called but just in case
	lua_pushnil(L);
	return 1;
}