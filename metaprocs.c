#include "mainheader.h"

int METAPROC_Load_Scene(lua_State* L) { 
	s_scene* tmp_scene = lua_touserdata(L, 1);
	if (tmp_scene) {
		HASH_ADD_KEYPTR(hh, scenes, tmp_scene->name, strlen(tmp_scene->name), tmp_scene);
		return 0;
	}
	flogf("LUA EXCEPTION: Attempted to load a nil type scene\n");
	return 0;
}
int METAPROC_Unload_Scene(lua_State* L) { 
	s_scene* tmp_scene = lua_touserdata(L, 1);
	if (tmp_scene) {
		HASH_DELETE(hh, scenes, tmp_scene);
		return 0;
	}
	flogf("LUA EXCEPTION: Attempted to unload a nil type scene\n");
	return 0;
}
int METAPROC_Gc_Scene(lua_State* L) { //scene has to be unloaded
	s_scene* tmp_scene = lua_touserdata(L, 1);
	flogf("LUA DEBUG: Scene %s was garbage collected\n", tmp_scene->name); //just so that we know
	return 0;
}
int METAPROC_Getter_Scene(lua_State* L) {
	s_scene* tmp_scene = lua_touserdata(L, 1);
	char* member = luaL_checkstring(L, 2);
	if (tmp_scene) {
		if (!strcmp(member, "name"))
			lua_pushstring(L, tmp_scene->name);
		else if (!strcmp(member, "on_enter"))
			lua_pushstring(L, tmp_scene->on_enter);
		else if (!strcmp(member, "on_exit"))
			lua_pushstring(L, tmp_scene->on_exit);
		else if (!strcmp(member, "examine"))
			lua_pushstring(L, tmp_scene->examine);
		else if (!strcmp(member, "locations") || !strcmp(member, "locationmap"))
			lua_pushstring(L, tmp_scene->locations->examine);
		else {
			flogf("LUA EXCEPTION: Attempted to get invalid member, %s, from scene\n", member);
			lua_pushnil(L);
		}
		return 1;
	}
	flogf("LUA EXCEPTION: Attempted to get member %s of a nil scene\n", member);
	lua_pushnil(L);
	return 1;
}
int METAPROC_Setter_Scene(lua_State* L) {
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
		else if (!strcmp(member, "on_exit")) {
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
		else if (!strcmp(member, "locations") || !strcmp(member, "locationmap")) {
			s_location* tmp_locations = lua_touserdata(L, 3);
			tmp_scene->locations = tmp_locations;
		}
		else {
			flogf("LUA EXCEPTION: Attempted to set invalid member, %s, from scene\n", member);
		}
		return 0;
	}
	flogf("LUA EXCEPTION: Attempted to set member %s of a nil scene\n", member);
	return 0;
}