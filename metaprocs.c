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
	s_scene* scene = udata->data;
	if (scene) {
		//unrefrence all our callbacks
		if (scene->callback_enter)
			luaL_unref(L, LUA_REGISTRYINDEX, scene->callback_enter);
		if (scene->callback_exit)
			luaL_unref(L, LUA_REGISTRYINDEX, scene->callback_exit);
		if (scene->callback_examine)
			luaL_unref(L, LUA_REGISTRYINDEX, scene->callback_examine);
		free(scene); //free the actual data this is pointing to
	}
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
//thing
int METAPROC_Gc_Thing(lua_State* L) { //clean up
	s_data* udata = lua_touserdata(L, 1);
	s_thing* thing = udata->data;
	s_interaction* inters = thing->interactions;
	if (udata->data) {
		//clear callback list
		for (s_interaction* tmp = inters; tmp != NULL; tmp = tmp->next) {
			LL_DELETE(inters, tmp);
			if(tmp->call > 0)
				luaL_unref(L, LUA_REGISTRYINDEX, tmp->call);
			free(tmp);
		}
		free(thing);
	}
	return 0;
}
int METAPROC_Getter_Thing(lua_State* L) {
	s_data* udata = lua_touserdata(L, 1);
	s_thing* thing = udata->data;
	char* member = luaL_checkstring(L, 2);
	//should be completly safe since this only gets called on things and lua will throw an error if udata is null automatically, no checking required
	if (!strcmp(member, "name"))
		lua_pushstring(L, thing->name);
	else if (!strcmp(member, "examine"))
		lua_pushstring(L, thing->examine);
	else { //users are not allowed to access interactions, they can set them but they cannot 'get' them because the idea is they should be left alone but should also be able to be deleted when they are not neccesary, you don't want to 'talk' to anyone twice and get the same dialouge
		flogf("LUA EXCEPTION: Attempted to get invalid member, '%s,' from a type 'thing'\n", member);
		lua_pushnil(L);
	}
	return 1;
}
int METAPROC_Setter_Thing(lua_State* L) {
	s_data* udata = lua_touserdata(L, 1);
	s_thing* thing = udata->data;
	if (lua_type(L, 2) == LUA_TSTRING) {
		char* member = luaL_checkstring(L, 2);
		if (!strcmp(member, "name")) {
			char* value = luaL_checkstring(L, 3);
			thing->name = value;
		}
		else if (!strcmp(member, "examine")) {
			char* value = luaL_checkstring(L, 3);
			thing->examine = value;
		}
		else { //users are not allowed to access interactions, they can set them but they cannot 'get' them because the idea is they should be left alone but should also be able to be deleted when they are not neccesary, you don't want to 'talk' to anyone twice and get the same dialouge
			flogf("LUA EXCEPTION: Attempted to set invalid member, '%s,' from a type 'thing'\n", member);
			lua_pushnil(L);
		}
	}
	else if (lua_type(L, 3) == LUA_TNUMBER) { //we are allowed to deleted and set interactions here, still comtemplating if I should allow users to 'get' them, however
		int index = luaL_checknumber(L, 2);
		int cur_index = 1; //start at one since lua is weird
		for (s_interaction* tmp_inter = thing->interactions; tmp_inter != NULL; tmp_inter = tmp_inter->next) {
			if (cur_index == index) {
				if (lua_type(L, 3) == LUA_TTABLE) { //locations and interactions are special and since the lua client doesn't have direct control over them we will use tables as consructors, for scenes and other types the client is more privalages with, this will not be the case
					lua_rawgeti(L, 3, 1); //get keyword 
					lua_rawgeti(L, 3, 1); //get callback
					tmp_inter->call = luaL_ref(L, -1); //function should be at the top
					tmp_inter->keyword = luaL_checkstring(L, -1);
				}
				else if (lua_type(L, 3) == LUA_TNIL) {
					LL_DELETE(thing->interactions, tmp_inter);
					if(tmp_inter->call > 0)
						luaL_unref(L, LUA_REGISTRYINDEX, tmp_inter->call);
					free(tmp_inter);
				}
				return 0;
			}
			cur_index++;
		}
		//index was out of bounds if we made it here
		flogf("LUA EXCEPTION: Index given to 'thing' 'set' function was out of bounds\n");
	}
	else 
		flogf("LUA EXCEPTION: Invalid type passed to 'thing' 'set' function'");
	return 0;
}