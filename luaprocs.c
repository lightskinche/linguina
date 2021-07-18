#include "mainheader.h"
//these functions are the 'library' for the client's scripts

//image functions
int LUAPROC_Load_Image(lua_State* L) { // {type=T_SURFACE,data=[sdl_surface address]} io_load_image(char* filename)
	//loads an image and stores it as a surf so it may be edited, a different func will turn it to a surface
	char* filename = lua_tostring(L, -1);
	SDL_Surface* surf = IMG_Load(filename);
	lua_newtable(L);
	if (surf) {
		lua_pushnumber(L, T_SURFACE);
		lua_setfield(L, 2, "type"); //should be a pointer to this surface
		lua_pushlightuserdata(L, surf);
		lua_setfield(L, 2, "data");
	}
	else {
		lua_pushnumber(L, T_NULL);
		lua_setfield(L, 2, "type"); 
		lua_pushlightuserdata(L, NULL);
		lua_setfield(L, 2, "data");
	}
	return 1;
}
//texture functions
int LUAPROC_Create_Texture(lua_State* L) { // {type=T_TEXTURE,data=[sdl_texture address]} texture_create(surface)
	if (!lua_istable(L, 1)) {
		puts("LUA EXCEPTION: Expected a table, but recieved a different type, returning garbage T_UNKNOWN value");
		lua_newtable(L);
		lua_pushnumber(L, T_UNKNOWN);
		lua_setfield(L, 2, "type");
		lua_pushlightuserdata(L, NULL);
		lua_setfield(L, 2, "data");
		return 1;
	}
	lua_getfield(L, 1, "type"); int type = -1;
	if (type = luaL_checknumber(L, 2) != T_SURFACE) {
		printf("LUA EXCEPTION: Expected T_SURFACE but recieved %d instead, returning garabage T_UNKNOWN value", type);
		lua_newtable(L);
		lua_pushnumber(L, T_UNKNOWN);
		lua_setfield(L, 3, "type");
		lua_pushlightuserdata(L, NULL);
		lua_setfield(L, 3, "data");
		return 1;
	}
	//if we made it here then all checks have passed and we should be completly safe
	lua_getfield(L, 1, "data");
	SDL_Surface* surf = lua_touserdata(L, 3);
	SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
	SDL_FreeSurface(surf);

	lua_newtable(L);
	lua_pushnumber(L, T_TEXTURE);
	lua_setfield(L, 4, "type");
	lua_pushlightuserdata(L, tex);
	lua_setfield(L, 4, "data");
	return 1;
}