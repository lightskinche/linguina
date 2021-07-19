#include "mainheader.h"
//this file is for metamethods for the types we are using
//scene functions
int METAPROC_Index_Scene(lua_State* L) { //basically just allows you to get stuff from the scene struct, no need for c-like prototype here
	s_scene* tmp_scene = lua_touserdata(L, 1);
	char* member = luaL_checkstring(L, 2);
	if (tmp_scene) {
		lua_newtable(L);
		lua_pushstring(L, tmp_scene->name);
		lua_setfield(L, -2, "name");
		lua_pushstring(L, tmp_scene->on_enter);
		lua_setfield(L, -2, "on_enter");
		lua_pushstring(L, tmp_scene->on_exit);
		lua_setfield(L, -2, "on_exit");
		lua_pushstring(L, tmp_scene->examine);
		lua_setfield(L, -2, "examine");
		lua_pushlightuserdata(L, tmp_scene->locations);
		lua_setfield(L, -2, "locations");
		lua_rawgeti(L, LUA_REGISTRYINDEX, tmp_scene->callback_enter);
		lua_setfield(L, -2, "callback_enter");
		lua_rawgeti(L, LUA_REGISTRYINDEX, tmp_scene->callback_exit);
		lua_setfield(L, -2, "callback_exit");
		lua_rawgeti(L, LUA_REGISTRYINDEX, tmp_scene->callback_examine);
		lua_setfield(L, -2, "callback_examine");
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
		else if (!strcmp(member, "callback_enter")) {
			luaL_checktype(L, 3, LUA_TFUNCTION);
			tmp_scene->callback_enter = luaL_ref(L, LUA_REGISTRYINDEX);
		}
		else if (!strcmp(member, "callback_exit")) {
			luaL_checktype(L, 3, LUA_TFUNCTION);
			tmp_scene->callback_exit = luaL_ref(L, LUA_REGISTRYINDEX);
		}
		else if (!strcmp(member, "callback_examine")) {
			luaL_checktype(L, 3, LUA_TFUNCTION);
			tmp_scene->callback_examine = luaL_ref(L, LUA_REGISTRYINDEX);
		}
		else if (!strcmp(member, "locations")) {
			s_location* locations = lua_touserdata;
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
	lua_pushlightuserdata(L, tmp_location);
	lua_getfield(L, LUA_REGISTRYINDEX, "locationmap_metatable");
	lua_setmetatable(L, -2);

	return 1;
}
int METAPROC_NewIndex_LocationMap(lua_State* L) { // location(userdata)* | char* __index(location(userdata)* tmp_location, int | char* member)
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
			flogf("LUA EXCEPTION: Attempted to access invalid member %s in a'locationmap'\n", member);	
		return 0;
	}
	flogf("LUA EXCEPTION: Attempted to index a nil type'\n");
	return 0;
}
int METAPROC_Tostring_LocationMap(lua_State* L) { // char* __tostring(s_location(userdata)* tmp_location)
	s_location* tmp_location = lua_touserdata(L, 1);
	if (tmp_location) {
		lua_pushstring(L, tmp_location->examine);
		return 1;
	}
	flogf("LUA EXCEPTION: Attempted __tostring on a nil 'locationmap'"); //don't think these will ever, ever be called but just in case
	lua_pushnil(L);
	return 1;
}