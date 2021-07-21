#include "mainheader.h"
//these functions are the 'library' for the client's scripts
//resource aqusisition functions
int LUAPROC_Load_Texture(lua_State* L) { // SDL_Texture(userdata)*, int, int load_texture(char* filename)
	char* filename = luaL_checkstring(L, 1);
	SDL_Surface* surf = IMG_Load(filename);
	if (surf) {
		SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
		s_data* udata = lua_newuserdata(L, sizeof(*udata));
		udata->data = tex, udata->type = T_TEXTURE; //we will use this structure for valididation
		lua_getfield(L, LUA_REGISTRYINDEX, "texture_metatable");
		lua_setmetatable(L, -2);
		lua_pushnumber(L, surf->w);
		lua_pushnumber(L, surf->h);
		SDL_FreeSurface(surf);
		return 3;
	}
	else {
		flogf("LUA EXCEPTION: Failed to load image file %s\n", filename);
		lua_pushnil(L);
		return 1;
	}
}
//TODO: test audio because idk if it is broken or not, but beside that everything is looking good
int LUAPROC_Load_Audio(lua_State* L) { // Mix_Chunk(userdata)* load_audio(char* filename)
	char* filename = luaL_checkstring(L, 1);
	s_data* udata = lua_newuserdata(L, sizeof(*udata));
	udata->data = Mix_LoadWAV(filename), udata->type = T_AUDIO;
	lua_getfield(L, LUA_REGISTRYINDEX, "audio_metatable");
	lua_setmetatable(L, -2);
	if(!udata->data){
		flogf("LUA EXCEPTION: Failed to load %s (audio file), MIXER: %s\n", filename, Mix_GetError());
		lua_pushnil(L);
	}
	return 1;
}
int LUAPROC_Load_Music(lua_State* L) {
	char* filename = luaL_checkstring(L, 1); //Mix_Music(userdata)* load_music(char* filename)
	s_data* udata = lua_newuserdata(L, sizeof(*udata));
	udata->data = Mix_LoadMUS(filename), udata->type = T_MUSIC;
	lua_getfield(L, LUA_REGISTRYINDEX, "music_metatable");
	lua_setmetatable(L, -2);
	if (!udata->data){
		flogf("LUA EXCEPTION: Failed to load %s (music audio file), MIXER: %s\n", filename, Mix_GetError());
		lua_pushnil(L);
	}
	return 1;
}
int LUAPROC_Display(lua_State* L) { // void display(char* buf)
	if (g_text.text)
		SDL_DestroyTexture(g_text.text); //remove current texture from g_text if it is there
	char* buffer = lua_tostring(L, -1);
	SDL_Surface* surf = TTF_RenderText_Blended_Wrapped(strd_font, buffer, (SDL_Color){ 255, 255, 255 }, resolution_x);
	if (surf) {
		SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer,surf);
		s_renderered_text tmp_text = { tex,surf->w,surf->h };
		g_text = tmp_text; //set the current text to this
		SDL_FreeSurface(surf);
		return 0;
	}
	else {
		flogf("LUA EXCEPTION: Could not render text, surf = NULL!");
		return 0;
	}
}
int LUAPROC_Log(lua_State* L) { // void log(char* buffer)
	char* buffer = luaL_checkstring(L, 1);
	flogf("LUA DEBUG: %s\n", buffer);
	return 0;
}
//scene functions
int LUAPROC_Create_Scene(lua_State* L) {// s_scene*(userdata) create_scene(char* name, char* on_enter, char* on_exit, char* examine, s_location(userdata)* locations)
	char* name = luaL_checkstring(L, 1); //apparently storing strings like this is bad, seems to be good so far, but should be weary
	char* on_enter = luaL_checkstring(L, 2);
	char* on_exit = luaL_checkstring(L, 3);
	char* examine = luaL_checkstring(L, 4);
	s_location* locations = lua_touserdata(L, 5);
	s_data* udata = lua_newuserdata(L, sizeof(*udata));
	s_scene* tmp_scene = calloc(1, sizeof(*tmp_scene));
	udata->data = tmp_scene, udata->type = T_SCENE;
	tmp_scene->callback_enter = 0, tmp_scene->callback_exit = 0, tmp_scene->callback_examine = 0; //makes sure no garbage is in there
	tmp_scene->name = name, tmp_scene->on_enter = on_enter, tmp_scene->on_exit = on_exit, tmp_scene->examine = examine, tmp_scene->locations = locations;
	lua_getfield(L, LUA_REGISTRYINDEX, "scene_metatable");
	lua_setmetatable(L, -2);
	return 1;
}
//location maps will actually be a subtype with a different __index metamethod that can take in numbers
int LUAPROC_Create_LocationMap(lua_State* L) { // location(userdata)* create_locationmap(int w, int h, int offsetx, int offsety, ...)
	int width = luaL_checkinteger(L, 1);
	int height = luaL_checkinteger(L, 2);
	int offsetx = luaL_checkninteger(L, 3);
	int offsety = luaL_checkinteger(L, 4);
	s_location* locations = calloc(width * height, sizeof(s_location));
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			lua_rawgeti(L, 5 + j + (i * width), 1);
			lua_rawgeti(L, 5 + j + (i * width), 2);
			lua_rawgeti(L, 5 + j + (i * width), 3);
			lua_rawgeti(L, 5 + j + (i * width), 4);
			lua_rawgeti(L, 5 + j + (i * width), 5);
			locations[j + (i * width)].callback_invalid = luaL_ref(L, LUA_REGISTRYINDEX); //store the callback, if any
			locations[j + (i * width)].callback_exit = luaL_ref(L, LUA_REGISTRYINDEX); //store the callback, if any
			locations[j + (i * width)].callback_enter = luaL_ref(L, LUA_REGISTRYINDEX); //store the callback, if any
			char* on_enter = lua_tostring(L, -1);
			char* examine = lua_tostring(L, -2);
			locations[j + (i * width)].examine = examine, locations[j + (i * width)].on_enter = on_enter, locations[j + (i * width)].orginal_map = locations;
			locations[j + (i * width)].w = width, locations[j + (i * width)].h = height;
		}
	}
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			if (i > 0 && locations[j + ((i - 1) * width)].examine)
				locations[j + (i * width)].north = &locations[j + ((i - 1) * width)];
			if (i < height - 1 && locations[j + ((i + 1) * width)].examine)
				locations[j + (i * width)].south = &locations[j + ((i + 1) * width)];

			if (j > 0 && locations[(j - 1) + (i * width)].examine)
				locations[j + (i * width)].west = &locations[(j - 1) + (i * width)]; //this is east and west thing is lowkey confusing me

			if(j < width - 1 && locations[(j + 1) + (i * width)].examine)
				locations[j + (i * width)].east = &locations[(j + 1) + (i * width)];

			//printf("Location at [x:%d y:%d] is address %p and has north %p, south %p, east %p, and west %p\n",
				//j, i, &locations[j + (i * width)], locations[j + (i * width)].north, locations[j + (i * width)].south, locations[j + (i * width)].west, locations[j + (i * width)].east);
		}
	}
	s_data* udata = calloc(1, sizeof(*udata));
	udata->type = T_LOCATION;
	if(offsetx > 0 && offsety > 0)
		udata->data = &locations[offsetx - 1 + ((offsety - 1) * width)]; //return this map of locations
	else {
		flogf("LUA EXCEPTION: Invalid index to location map, subscript must be greater than 0\n");
		udata->data = locations;
	}
	lua_pushlightuserdata(L, udata);
	return 1;
}
int LUAPROC_Destroy_Location(lua_State* L) { // void destroy_location(s_location(userdata)* tmp_location)
	s_data* udata = lua_touserdata(L, 1);
	s_location* location_l = udata->data;
	s_location* location = location_l->orginal_map;
	if (udata && udata->type == T_LOCATION) {
		if (location) {
			for (int i = 0; i < location->w * location->h; ++i) {
				if (location[i].callback_enter)
					luaL_unref(L, LUA_REGISTRYINDEX, location[i].callback_enter);
				if (location[i].callback_exit)
					luaL_unref(L, LUA_REGISTRYINDEX, location[i].callback_exit);
				if (location[i].callback_invalid)
					luaL_unref(L, LUA_REGISTRYINDEX, location[i].callback_invalid);
			}
			free(location);
		}
		else
			flogf("LUA EXCEPTION: Attempted to destroy nil 'location' or 'locationmap'\n");
	}
	else if (udata)
		flogf("LUA EXCPETION: Invalid type passed to 'destroy_location,' expected 'T_LOCATION' but recieved '%s'\n", e_typenames[udata->type]);
	else
		flogf("LUA EXCPETION: Invalid type, NIL, passed to 'destroy_location'\n");
	return 0;
}
int LUAPROC_Set_Background(lua_State* L) { // void set_background(SDL_Texture(userdata)* tex, bool text_background)
	s_data* udata = lua_touserdata(L, 1);
	text_background = lua_toboolean(L, 2);
	if (udata && udata->type == T_TEXTURE)
		background = udata->data;
	else if (udata)
		flogf("LUA EXPCETION: Invalid type passed to 'set_background,' expected 'T_TEXTURE' but recieved '%s'\n", e_typenames[udata->type]);
	else
		background = NULL;
	return 0;
}
int LUAPROC_Wait(lua_State* L) { //void wait(int ms)
	int ms = luaL_checkinteger(L, 1);
	SDL_Delay(ms);
	return 0;
}
int LUAPROC_Set_CurrentScene(lua_State* L) { //void set_currentscene(s_scene(userdata)* tmp_scene)
	s_data* udata = lua_touserdata(L, 1);
	if (udata && udata->type == T_SCENE)
		cur_scene = udata->data;
	else if (udata)
		flogf("LUA EXCEPTION: Invalid type passed to 'set_currentscene,' expected T_SCENE but recieved '%s'\n", e_typenames[udata->type]);
	else
		flogf("LUA EXPCETION: Inavlid type, NIL, passed to 'set_currentscene'\n");
	return 0;
}
//thing stuff here
int LUAPROC_Create_Thing(lua_State* L) { //s_thing* create_thing(char* name, char* examine, int num_interactions, ...)
	char* name = luaL_checkstring(L, 1);
	char* examine = luaL_checkstring(L, 2);
	int num_interactions = luaL_checkinteger(L, 3);
	s_thing* thing = calloc(1,sizeof(*thing)); //just to make things easier
	thing->name = name, thing->examine = examine;
	for (int i = 0; i < num_interactions; ++i) {
		luaL_checktype(L, 4 + i, LUA_TTABLE); //interactions take the form as {keyword,function}
		lua_rawgeti(L, 4 + i, 1); //get the keyword
		lua_rawgeti(L, 4 + i, 2); //get the function
		s_interaction* tmp_inter = calloc(1, sizeof(*tmp_inter));
		tmp_inter->call = luaL_ref(L, LUA_REGISTRYINDEX); //get function and store refrence to it
		tmp_inter->keyword = luaL_checkstring(L, -1); //get keyword, luaL_ref pops function so keyword is at the top now
		LL_APPEND(thing->interactions, tmp_inter); //add to list
	}
	s_data* udata = lua_newuserdata(L, sizeof(*udata));
	udata->data = thing, udata->type = T_THING;
	lua_getfield(L, LUA_REGISTRYINDEX, "thing_metatable");
	lua_setmetatable(L, -2);
	return 1;
}