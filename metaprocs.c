#include "mainheader.h"

int METAPROC_Load_Scene(lua_State* L) { 
	s_data* udata = lua_touserdata(L, 1);
	if (udata && udata->data) {
		s_scene* tmp_scene = udata->data;
		HASH_ADD_KEYPTR(hh, scenes, tmp_scene->name, strlen(tmp_scene->name), tmp_scene);
		return 0;
	}
	flogf("LUA EXCEPTION: Attempted to load a nil type scene\n");
	return 0;
}
int METAPROC_Unload_Scene(lua_State* L) { 
	s_data* udata = lua_touserdata(L, 1);
	if (udata && udata->data) { //dont have to check for type since its impossible for it to be anything but nil and a scene
		s_scene* tmp_scene = udata->data;
		HASH_DELETE(hh, scenes, tmp_scene);
		return 0;
	}
	flogf("LUA EXCEPTION: Attempted to unload a nil type scene\n");
	return 0;
}
int METAPROC_Gc_Scene(lua_State* L) { //scene has to be unloaded and we have to actually free it too
	s_data* udata = lua_touserdata(L, 1);
	free(udata->data); //free the actual data this is pointing to
	return 0;
}
int METAPROC_Getter_Scene(lua_State* L) {
	s_data* udata = lua_touserdata(L, 1);
	s_scene* tmp_scene = udata->data;
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
			lua_pushstring(L, tmp_scene->locations);
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
	s_data* udata = lua_touserdata(L, 1);
	s_scene* tmp_scene = udata->data;
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
			s_data* tmp_udata = lua_touserdata(L, 3);
			if (tmp_udata && tmp_udata->type == T_LOCATION)
				tmp_scene->locations = tmp_udata;
			else if (tmp_udata)
				flogf("LUA EXCEPTION: Invalid value, attempted to set 'locations'/'locationmap' to a type '%s'\n", e_typenames[tmp_udata->type]);
		}
		else {
			flogf("LUA EXCEPTION: Attempted to set invalid member, %s, from scene\n", member);
		}
		return 0;
	}
	flogf("LUA EXCEPTION: Attempted to set member %s of a nil scene\n", member);
	return 0;
}
//audio and music start here
int METAPROC_Gc_Audio(lua_State* L) { 
	s_data* udata = lua_touserdata(L, 1);
	if(udata->data)
		Mix_FreeChunk(udata->data);
	return 0;
}
int METAPROC_Gc_Music(lua_State* L) { //void destroy_music(Mix_Music* tmp_music)
	s_data* udata = lua_touserdata(L, 1);
	if (udata->data)
		Mix_FreeMusic(udata->data);
	return 0;
}
int METAPROC_Play_Audio(lua_State* L) { //void play_audio(Mix_Chunk* tmp_chunk, int loops)
	s_data* udata = lua_touserdata(L, 1);
	if (udata && udata->type == T_AUDIO)
		Mix_PlayChannel(-1, udata->data, luaL_checknumber(L, 2));
	else if (udata)
		flogf("LUA EXCEPTION: Invalid value,'%s,' passed to 'play_audio' which expected type 'T_AUDIO'\n", e_typenames[udata->type]);
	else
		flogf("LUA EXCPETION: Invalid value, NIL, passed to 'play_audio'\n");
	return 0;
}
int METAPROC_Stop_Audio(lua_State* L) { // void stop_audio(Mix_Chunk* chunk_to_stop)
	s_data* udata = lua_touserdata(L, 1);
	if (udata && udata->type == T_AUDIO) {
		for (int i = 0; i < MAX_MIXER_CHANNELS; ++i)
			if (Mix_GetChunk(i) == udata->data)
				Mix_HaltChannel(i);
	}
	else if (udata)
		flogf("LUA EXCEPTION: Invalid value, '%s,' passed to 'stop_audio' which expected type 'T_AUDIO'\n", e_typenames[udata->type]);
	else
		flogf("LUA EXCEPTION: Invalid value, NIL, passed to 'stop_audio'\n");
	return 0;
}
int METAPROC_Play_Music(lua_State* L) { //void play_music(Mix_Music* tmp_music) *music will repeat until stopped manually
	s_data* udata = lua_touserdata(L, 1);
	if (udata && udata->type == T_MUSIC)
		Mix_PlayMusic(udata->data, -1);
	else if (udata)
		flogf("LUA EXCEPTION: Invalid value, '%s,' passed to 'play_music' which expects type 'T_MUSIC'\n", e_typenames[udata->type]);
	else
		flogf("LUA EXCEPTION: Invalid value, NIL, passed to 'play_music'\n");
	return 0;
}
int METAPROC_Stop_Music(lua_State* L) { // void stop_music(void)
	Mix_HaltMusic();
	return 0;
}
//texture
int METAPROC_Gc_Texture(lua_State* L) { // void destory_texture(SDL_Texture(userdata)* tex)
	s_data* texture_data = lua_touserdata(L, 1);
	if (texture_data->data)
		SDL_DestroyTexture(texture_data->data);
	return 0;
}