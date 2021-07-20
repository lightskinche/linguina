#include "mainheader.h"
//this file is for random aux functions, could be anything
static const struct luaL_reg engine_lib[] = {
	{"log", LUAPROC_Log},
	{"create_scene", LUAPROC_Create_Scene},
	{"create_locationmap", LUAPROC_Create_LocationMap},
	{"destroy_locationmap", LUAPROC_Destroy_Location},
	{"wait", LUAPROC_Wait},
	{"display", LUAPROC_Display},
	{"load_texture", LUAPROC_Load_Texture},
	{"load_audio", LUAPROC_Load_Audio},
	{"load_music", LUAPROC_Load_Music},
	{"set_background", LUAPROC_Set_Background},
	{"set_currentscene", LUAPROC_Set_CurrentScene},
    {NULL, NULL}
};
static const struct luaL_reg scene_methods[] = {
	{"load",METAPROC_Load_Scene},
	{"unload",METAPROC_Unload_Scene},
	{"get", METAPROC_Getter_Scene},
	{"set", METAPROC_Setter_Scene},
	{NULL, NULL}
};
static const struct luaL_reg audio_methods[] = {
	{"play",METAPROC_Play_Audio},
	{"stop",METAPROC_Stop_Audio},
	{NULL, NULL}
};
static const struct luaL_reg music_methods[] = {
	{"play",METAPROC_Play_Music},
	{"stop",METAPROC_Stop_Music},
	{NULL, NULL}
};
//we dont need any parameters since all variables we are using are global variables, this function is mainly used for ease-of-reading
inline void AUX_Load_Libraries(void) { 
	luaL_openlibs(L); //load standered library
	//now we load our SHARED/GLOBAL lua functions 
	luaL_openlib(L, "engine", engine_lib, 0);
	//scene metatable
	luaL_newmetatable(L, "scene_metatable");
	luaL_newmetatable(L, "scene_metamethods");
	luaL_openlib(L, NULL, scene_methods, 0); //should set methods of scene_metamethods which is the __index for scene_metatable
	lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, METAPROC_Gc_Scene);
	lua_setfield(L, -2, "__gc");
	//audio metatable
	luaL_newmetatable(L, "audio_metatable");
	luaL_newmetatable(L, "audio_metamethods");
	luaL_openlib(L, NULL, audio_methods, 0); 
	lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, METAPROC_Gc_Audio);
	lua_setfield(L, -2, "__gc");
	//music metatable
	luaL_newmetatable(L, "music_metatable");
	luaL_newmetatable(L, "music_metamethods");
	luaL_openlib(L, NULL, music_methods, 0);
	lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, METAPROC_Gc_Music);
	lua_setfield(L, -2, "__gc");
	//texture metatable
	luaL_newmetatable(L, "texture_metatable");
	lua_pushcfunction(L, METAPROC_Gc_Texture);
	lua_setfield(L, -2, "__gc");
}
//game loop was long so it has its own file
void flogf(char* format, ...) {
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	vfprintf(logfile, format, args);
	va_end(args);
}
void AUX_Display(char* buffer) {
	if (g_text.text)
		SDL_DestroyTexture(g_text.text); //remove current texture from g_text if it is there
	SDL_Surface* surf = TTF_RenderText_Blended_Wrapped(strd_font, buffer, (SDL_Color) { 255, 255, 255 }, resolution_x);
	if (surf) {
		SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
		s_renderered_text tmp_text = { tex,surf->w,surf->h };
		g_text = tmp_text; //set the current text to this
		SDL_FreeSurface(surf);
	}
	else
		flogf("EXCEPTION: Could not generate surface in engine AUX_Display\n");
}
void AUX_RenderTextProccesing(void) { //couldn't come up with a better name, this function just renders the current g_text
	//special render function so the text just doesn't appear, shouldn't decrease perfomance at all and should be togglable
	for (float j = 0; j < g_text.w; j += 0.5) {
		SDL_RenderClear(renderer);
		if (background) {
			SDL_RenderCopy(renderer, background, NULL, NULL);
			if (text_background)
				SDL_RenderFillRect(renderer, &(SDL_Rect){0, resolution_y - g_text.h - 25, g_text.w, g_text.h});
		}
		if (g_text.h - 25 > 5)
			SDL_RenderCopy(renderer, g_text.text, &(SDL_Rect){0, 0, j, 25}, & (SDL_Rect){0, resolution_y - g_text.h - 25, j, 25});
		else
			SDL_RenderCopy(renderer, g_text.text, &(SDL_Rect){0, 0, j, g_text.h}, & (SDL_Rect){0, resolution_y - g_text.h - 25, j, g_text.h});
		SDL_RenderPresent(renderer);
	}
	if (g_text.h - 25 > 5) {
		for (float i = 25; i < g_text.h; i += 0.25) {
			SDL_RenderClear(renderer);
			if (background) {
				SDL_RenderCopy(renderer, background, NULL, NULL);
				if (text_background)
					SDL_RenderFillRect(renderer, &(SDL_Rect){0, resolution_y - g_text.h - 25, g_text.w, g_text.h});
			}
			SDL_RenderCopy(renderer, g_text.text, &(SDL_Rect){0, 0, g_text.w, i}, & (SDL_Rect){0, resolution_y - g_text.h - 25, g_text.w, i});
			SDL_RenderPresent(renderer);
		}
	}
}