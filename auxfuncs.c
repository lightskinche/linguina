#include "mainheader.h"
//this file is for random aux functions, could be anything

//we dont need any parameters since all variables we are using are global variables, this function is mainly used for ease-of-reading
inline void AUX_Load_Libraries(void) { 
	luaL_openlibs(L); //load standered library
	//load and set up custom type metatables
	//for scenes
	luaL_newmetatable(L, "scene_metatable");
	lua_pushcfunction(L, METAPROC_Index_Scene);
	lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, METAPROC_NewIndex_Scene);
	lua_setfield(L, -2, "__newindex");
	lua_pushcfunction(L, METAPROC_Tostring_Scene);
	lua_setfield(L, -2, "__tostring");
	//for location
	luaL_newmetatable(L, "location_metatable");
	lua_pushcfunction(L, METAPROC_Index_Location);
	lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, METAPROC_NewIndex_LocationS);
	lua_setfield(L, -2, "__newindex");
	lua_pushcfunction(L, METAPROC_Tostring_LocationS);
	lua_setfield(L, -2, "__tostring");
	//for locationmap
	luaL_newmetatable(L, "locationmap_metatable");
	lua_pushcfunction(L, METAPROC_Index_LocationMap);
	lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, METAPROC_NewIndex_LocationS);
	lua_setfield(L, -2, "__newindex");
	lua_pushcfunction(L, METAPROC_Tostring_LocationS);
	lua_setfield(L, -2, "__tostring");
	lua_settop(L, 0);
	//now we load our SHARED/GLOBAL lua functions 
	lua_pushcfunction(L, LUAPROC_Log);
	lua_setglobal(L, "log");

	lua_pushcfunction(L, LUAPROC_Create_Scene);
	lua_setglobal(L, "create_scene");

	lua_pushcfunction(L, LUAPROC_Destroy_Scene);
	lua_setglobal(L, "destroy_scene");

	lua_pushcfunction(L, LUAPROC_Load_Scene);
	lua_setglobal(L, "load_scene");

	lua_pushcfunction(L, LUAPROC_Unload_Scene);
	lua_setglobal(L, "unload_scene");

	lua_pushcfunction(L, LUAPROC_Find_Scene_Unloaded);
	lua_setglobal(L, "find_scene_u");

	lua_pushcfunction(L, LUAPROC_Create_LocationMap);
	lua_setglobal(L, "create_locationmap");

	lua_pushcfunction(L, LUAPROC_Create_Location);
	lua_setglobal(L, "create_location");

	lua_pushcfunction(L, LUAPROC_Destroy_Location);
	lua_setglobal(L, "destroy_location");

	lua_pushcfunction(L, LUAPROC_Wait);
	lua_setglobal(L, "wait");
	
	lua_pushcfunction(L, LUAPROC_Display);
	lua_setglobal(L, "dipslay");

	lua_pushcfunction(L, LUAPROC_Destroy_Texture);
	lua_setglobal(L, "destroy_texture");

	lua_pushcfunction(L, LUAPROC_Set_Background);
	lua_setglobal(L, "set_background");

	lua_pushcfunction(L, LUAPROC_Set_CurrentScene);
	lua_setglobal(L, "set_currentscene");
}

void AUX_Handle_GameLoop(void) { //recieves text input, calls lua 'main' function and moves the text-based game forward
	//first, let's recieve input from the user
	static int shiftheld = 0, receving_input = 1; //if the shift key is held down or not
	if (receving_input) {
		while (SDL_PollEvent(&even)) {
			if (even.type == SDL_KEYDOWN) {
				for (int i = 0; i < MAX_INPUT_SIZE - 1; ++i) {
					input_buf[MAX_INPUT_SIZE - 2] = 0;
					if (!input_buf[i]) {
						//handle special keys
						if (even.key.keysym.sym == SDLK_LSHIFT)
							shiftheld = 1;
						if (even.key.keysym.sym == SDLK_RETURN)
							receving_input = 0;
						//handle typical keys
						if (even.key.keysym.sym >= SDLK_a && even.key.keysym.sym <= SDLK_z || even.key.keysym.sym == SDLK_SPACE) {
							if (!shiftheld)
								input_buf[i] = even.key.keysym.sym;
							else if (even.key.keysym.sym != SDLK_SPACE)
								input_buf[i] = even.key.keysym.sym - 32;
						}
						else if (even.key.keysym.sym == SDLK_BACKSPACE)
							if (&input_buf[i - 1] >= input_buf)
								input_buf[i - 1] = 0;
						break;
					}
				}
			}
			else if (even.type == SDL_KEYUP) {
				if (even.key.keysym.sym == SDLK_LSHIFT)
					shiftheld = 0;
			}
			else if (even.type == SDL_WINDOWEVENT) {
				if (even.window.event == SDL_WINDOWEVENT_CLOSE)
					exit(0);
			}
		}
		//run textmain 
		lua_rawgeti(L, LUA_REGISTRYINDEX, textmain_ref);
		//set up 'state' table, it has different functions then main
		lua_newtable(L);
		if (receving_input) {
			lua_pushstring(L, "standered");
			lua_setfield(L, 2, "mode");
			//lua_pushcfunction(L, f);
			//lua_setfield(L, 2, "func");
			//push input buffer
			lua_pushstring(L, input_buf);
			lua_call(L, 2, 0);
		}
		else {
			if (g_err_text.text)
				SDL_DestroyTexture(g_err_text.text);
			lua_pushstring(L, "confirmation");
			lua_setfield(L, 2, "mode");
			lua_pushstring(L, input_buf);
			lua_call(L, 2, 2);
			receving_input = !lua_toboolean(L, -2);
			if (receving_input) {
				SDL_Surface* tmp_surf = TTF_RenderText_Blended_Wrapped(strd_font, lua_tostring(L, -1), (SDL_Color) { 255, 0, 0 }, resolution_x);
				if (tmp_surf) {
					g_err_text.text = SDL_CreateTextureFromSurface(renderer, tmp_surf);
					g_err_text.w = tmp_surf->w, g_err_text.h = tmp_surf->h;
					SDL_FreeSurface(tmp_surf);
				}
				else
					flogf("LUA EXCEPTION: No reason given for function '%s' failure\n", textmain_symbol);
			}
		}
		SDL_RenderClear(renderer);
		if (background)
			SDL_RenderCopy(renderer, background, NULL, NULL);
		//render background, if any
		{
			SDL_Surface* tmp_surf = TTF_RenderText_Blended_Wrapped(strd_font, input_buf, (SDL_Color) { 255, 255, 255 }, resolution_x);
			SDL_Texture* tmp_text = SDL_CreateTextureFromSurface(renderer, tmp_surf);
			if (tmp_surf) {
				if (text_background)
					SDL_RenderFillRect(renderer, &(SDL_Rect){0, 50, resolution_x, tmp_surf->h});
				SDL_RenderCopy(renderer, tmp_text, NULL, &(SDL_Rect){0, 50, tmp_surf->w, tmp_surf->h});
			}
			SDL_DestroyTexture(tmp_text);
			SDL_FreeSurface(tmp_surf);
		}
		//draw error text, if any
		if (g_err_text.text)
			SDL_RenderCopy(renderer, g_err_text.text, NULL, &(SDL_Rect){0, 100, g_err_text.w, g_err_text.h});
		//draw response text at the bottom
		if(text_background)
			SDL_RenderFillRect(renderer, &(SDL_Rect){0, resolution_y - g_text.h - 25, g_text.w, g_text.h});
		SDL_RenderCopy(renderer, g_text.text, NULL, &(SDL_Rect){0, resolution_y - g_text.h - 25, g_text.w, g_text.h});
		SDL_RenderPresent(renderer);
		lua_settop(L, 0); //get everything off the stack
	}
	else {
		//prase input and seperate into tokens
		//run the 'main' function before we do any processing
		lua_rawgeti(L, LUA_REGISTRYINDEX, main_ref);
		char input_buf_c[MAX_INPUT_SIZE]; memcpy(input_buf_c, input_buf, strlen(input_buf) + 1);
		//do this so we don't end up modifyig the actual input_buf since we still need it
		char** tokens[MAX_TOKENS] = {NULL,NULL,NULL,NULL};
		tokens[0] = strtok(input_buf_c, " "); 
		lua_pushstring(L, tokens[0]);
		for (int i = 1; i < MAX_TOKENS; ++i) {
			tokens[i] = strtok(NULL, " ");
			lua_pushstring(L, tokens[i]);
		}
		lua_call(L, 4, 1);
		if (!lua_toboolean(L, -1))
			goto QUIT;
		//process tokens
		if (tokens[0]) {
			//make sure we empty the last g_text
			if (g_text.text)
				SDL_DestroyTexture(g_text.text);
			g_text.w = 0, g_text.h = 0;
			if (!strcmp(tokens[0], "go") && tokens[1]) { //most simple directive, switch current_scene
				s_scene* tmp_scene = NULL;
				HASH_FIND_STR(scenes, tokens[1], tmp_scene);
				if (!tmp_scene)
					AUX_Display("I could not find that place anywhere around me.");
				else if (tmp_scene != cur_scene) {
					if (cur_scene) {
						if (cur_scene->callback_exit > 0) {
							lua_rawgeti(L, LUA_REGISTRYINDEX, cur_scene->callback_exit);
							lua_pushlightuserdata(L, cur_scene);
							lua_getfield(L, LUA_REGISTRYINDEX, "scene_metatable");
							lua_setmetatable(L, -2);
							lua_call(L, 1, 0); //function takes the scene that is is the 'child' of, aka the scene we are leaving
						}
						if (cur_scene->on_exit) {
							AUX_Display(cur_scene->on_exit);
							AUX_RenderTextProccesing();
							SDL_Delay(500);
							SDL_PollEvent(&even);
							while (!keyboard[SDL_SCANCODE_RETURN])
								SDL_PollEvent(&even);
						}
					}
					cur_scene = tmp_scene;
					if (cur_scene->callback_enter > 0) {
						lua_rawgeti(L, LUA_REGISTRYINDEX, cur_scene->callback_enter);
						lua_pushlightuserdata(L, cur_scene);
						lua_getfield(L, LUA_REGISTRYINDEX, "scene_metatable");
						lua_setmetatable(L, -2);
						lua_call(L, 1, 0); //function takes in the scene it is the 'child' of
					}
					if (cur_scene->on_enter)
						AUX_Display(cur_scene->on_enter);
				}
				else
					AUX_Display("I am already here.");
			}
			AUX_RenderTextProccesing();
		}
		QUIT:
		memset(input_buf, 0, MAX_INPUT_SIZE); //reset input_buf
		lua_settop(L, 0);
		receving_input = 1;
	}
}
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