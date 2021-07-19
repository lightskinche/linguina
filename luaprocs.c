#include "mainheader.h"
//these functions are the 'library' for the client's scripts

//'start' functions, this function has special access to image, audio, and font loading functions that are not allowed in 'main' and 'textmain'
//it can also force quit the program if it feels certain conditions are not met by returning false
int LUAPROC_Load_Texture(lua_State* L) { // SDL_Texture(userdata)*, int, int load_texture(char* filename)
	char* filename = luaL_checkstring(L, 1);
	SDL_Surface* surf = IMG_Load(filename);
	if (surf) {
		SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
		lua_pushlightuserdata(L, tex);
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
int LUAPROC_Destroy_Texture(lua_State* L) { // void destory_texture(SDL_Texture(userdata)* tex)
	SDL_Texture* tex = lua_touserdata(L, 1);
	if (tex)
		SDL_DestroyTexture(tex);
	else
		flogf("LUA EXCEPTION: Attempted to destroy nil texture");
	return 0;
}
//'main' fuctions, this function has special access to display functions and functions that modify the current backround, audio, and scene related stuff
//it can also cancel text prasing by returning false, this can be useful if the user wants to have a keyword to quit to menu and doesn't
//want to engine to process it
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
// 'textmain' functions, this function has the special abilty to deny the user the ability to submit input along with a reason,
//besides that it can perform realtime anlysis of input_buf and can basically runs everyframe

//these functions can be accessed anywhere
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
	s_scene* tmp_scene = calloc(1, sizeof(*tmp_scene));
	tmp_scene->name = name, tmp_scene->on_enter = on_enter, tmp_scene->on_exit = on_exit, tmp_scene->examine = examine, tmp_scene->locations = locations;
	HASH_ADD_KEYPTR(hh, unloaded_scenes, tmp_scene->name, strlen(tmp_scene->name), tmp_scene);
	lua_pushlightuserdata(L, tmp_scene);
	lua_getfield(L, LUA_REGISTRYINDEX, "scene_metatable");
	lua_setmetatable(L, 6);
	return 1;
}
int LUAPROC_Load_Scene(lua_State* L) { // void load_scene(s_scene(userdata)* tmp_scene)
	s_scene* tmp_scene = lua_touserdata(L, 1);
	if (tmp_scene) {
		HASH_DELETE(hh, unloaded_scenes, tmp_scene);
		HASH_ADD_KEYPTR(hh, scenes, tmp_scene->name, strlen(tmp_scene->name), tmp_scene);
		return 0;
	}
	flogf("LUA EXCEPTION: Attempted to load a nil type scene\n");
	return 0;
}
int LUAPROC_Unload_Scene(lua_State* L) {
	s_scene* tmp_scene = lua_touserdata(L, 1);
	if (tmp_scene) {
		HASH_DELETE(hh, scenes, tmp_scene);
		HASH_ADD_KEYPTR(hh, unloaded_scenes, tmp_scene->name, strlen(tmp_scene->name), tmp_scene);
		return 0;
	}
	flogf("LUA EXCEPTION: Attempted to unload a nil type scene\n");
	return 0;
}
int LUAPROC_Destroy_Scene(lua_State* L) { // void destroy_scene(s_scene(userdata)* tmp_scene) ***HAS TO BE AN UNLOADED SCENE IN unloaded_scenes ***
	s_scene* tmp_scene = lua_touserdata(L, 1);
	if (tmp_scene) {
		HASH_DELETE(hh, unloaded_scenes, tmp_scene);
		free(tmp_scene); //def update this if s_scene starts having more pointers inside of it
		return 0;
	}
	else {
		flogf("LUA EXCEPTION: Attempted to destroy nil scene\n");
		return 0;
	}
}
int LUAPROC_Find_Scene_Unloaded(lua_State* L) { // s_scene*(userdata) find_scene_u(char* name)
	char* name = luaL_checkstring(L, 1);
	s_scene* tmp_scene = NULL;
	HASH_FIND_STR(unloaded_scenes, name, tmp_scene);
	if (tmp_scene) {
		lua_pushlightuserdata(L, tmp_scene);
		return 1;
	}
	else {
		flogf("LUA EXCEPTION: Scene %s was not found in unloaded_scenes\n", name);
		lua_pushnil(L);
		return 1;
	}
}
//location maps will actually be a subtype with a different __index metamethod that can take in numbers
int LUAPROC_Create_LocationMap(lua_State* L) { // location(userdata)* create_locationmap(int w, int h, ...)
	int width = luaL_checknumber(L, 1);
	int height = luaL_checknumber(L, 2);
	s_location* locations = calloc(width * height, sizeof(s_location));
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			lua_rawgeti(L, 3 + j + (i * width), 1);
			lua_rawgeti(L, 3 + j + (i * width), 2);
			char* examine = lua_tostring(L, -2);
			char* on_enter = lua_tostring(L, -1);
			locations[j + (i * width)].examine = examine, locations[j + (i * width)].on_enter = on_enter;
			if (i > 0)
				if(locations[j + ((i - 1) * width)].examine)
					locations[j + (i * width)].north = &locations[j + ((i - 1) * width)];
			if (i < height - 1)
				if(locations[j + ((i + 1) * width)].examine)
					locations[j + (i * width)].south = &locations[j + ((i + 1) * width)];
			if (j > 0)
				if(locations[(j - 1) + (i * width)].examine)
					locations[j + (i * width)].west = &locations[(j - 1) + (i * width)];
			if (j < width - 1)
				if(locations[(j + 1) + (i * width)].examine)
					locations[j + (i * width)].east = &locations[(j + 1) + (i * width)];
			//printf("Location at [x:%d y:%d] is address %p and has north %p, south %p, east %p, and west %p\n", 
				//j, i, &locations[j + (i * width)], locations[j + (i * width)].north, locations[j + (i * width)].south, locations[j + (i * width)].west, locations[j + (i * width)].east);
		}
	}
	lua_pushlightuserdata(L, locations); //return this map of locations
	lua_getfield(L, LUA_REGISTRYINDEX, "locationmap_metatable");
	lua_setmetatable(L, -2);
	return 1;
}
int LUAPROC_Create_Location(lua_State* L) { // location(userdata)* create_location(char* examine, char* on_enter, s_thing* things)
	char* examine = luaL_checkstring(L, 1);
	char* on_enter = luaL_checkstring(L, 2);
	s_thing* things = lua_touserdata(L, 3);
	s_location* location = calloc(1, sizeof(location));
	location->examine = examine, location->on_enter = on_enter, location->things = things;
	lua_pushlightuserdata(L, location);
	lua_getfield(L, LUA_REGISTRYINDEX, "location_metatable");
	lua_setmetatable(L, -2);
	return 1;
}
int LUAPROC_Destroy_Location(lua_State* L) { // void destroy_location(s_location(userdata)* tmp_location)
	s_location* tmp_location = lua_touserdata(L, 1);
	if (tmp_location)
		free(tmp_location);
	else
		flogf("LUA EXCEPTION: Attempted to destroy nil 'location' or 'locationmap'\n");
	return 0;
}
int LUAPROC_Set_Background(lua_State* L) { // void set_background(SDL_Texture(userdata)* tex, bool text_background)
	SDL_Texture* tex = lua_touserdata(L, 1);
	text_background = lua_toboolean(L, 2);
	background = tex;
	return 0;
}