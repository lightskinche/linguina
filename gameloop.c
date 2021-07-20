#include "mainheader.h"
//still qualifying it as an "AUX" function, but it really isn't since this is neccesary for the game to be a 'game'
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
		if (receving_input) {
			lua_pushstring(L, "standered");
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
		if (text_background)
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
		char** tokens[MAX_TOKENS] = { NULL,NULL,NULL,NULL };
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
			//this directive should be mainly used for going north, south, west, or east and should only
			//switch the current scene in specific situations like: 'I walked west and now I see a store' because then saying 'go store' makes more sense
			//however, this use of 'go' can be circumvented completly by just only having one scene loaded at a time and silently swiching scenes
			if (!strcmp(tokens[0], "go") && tokens[1]) {
				if (cur_scene && cur_scene->locations && cur_scene->locations->data) {
					s_location* tmp_location = cur_scene->locations->data;
					if (!strcmp(tokens[1], "north")) {
						if (tmp_location->north) {
							if (tmp_location->callback_exit > 0) {
								lua_rawgeti(L, LUA_REGISTRYINDEX, tmp_location->callback_exit);
								lua_pushstring(L, "north"); //push direction the player is leaving in
								lua_call(L, 1, 0);
							}
							cur_scene->locations->data = tmp_location->north;
							tmp_location = cur_scene->locations->data;
							AUX_Display(tmp_location->on_enter);
							if (tmp_location->callback_enter > 0) {
								lua_rawgeti(L, LUA_REGISTRYINDEX, tmp_location->callback_enter);
								lua_pushstring(L, "north"); //lets the client know what direction the player typed to come here relative to the last location
								lua_call(L, 1, 0);
							}
						}
						else {
							AUX_Display("There isn't anything north of here.");
							if (tmp_location->callback_invalid > 0) {
								lua_rawgeti(L, LUA_REGISTRYINDEX, tmp_location->callback_invalid);
								lua_pushstring(L, "north"); //let them know the direction the player tried to go in
								lua_call(L, 1, 0);
							}
						}
						AUX_RenderTextProccesing(); //the QUIT label skips renderering so we'll just do it here
						goto QUIT;
					}
					else if (!strcmp(tokens[1], "south")) {
						if (tmp_location->south) {
							if (tmp_location->callback_exit > 0) {
								lua_rawgeti(L, LUA_REGISTRYINDEX, tmp_location->callback_exit);
								lua_pushstring(L, "south"); //push direction the player is leaving in
								lua_call(L, 1, 0);
							}
							cur_scene->locations->data = tmp_location->south;
							tmp_location = cur_scene->locations->data;
							AUX_Display(tmp_location->on_enter);
							if (tmp_location->callback_enter > 0) {
								lua_rawgeti(L, LUA_REGISTRYINDEX, tmp_location->callback_enter);
								lua_pushstring(L, "south");
								lua_call(L, 1, 0);
							}
						}
						else {
							AUX_Display("There isn't anything south of here.");
							if (tmp_location->callback_invalid > 0) {
								lua_rawgeti(L, LUA_REGISTRYINDEX, tmp_location->callback_invalid);
								lua_pushstring(L, "south"); //let them know the direction the player tried to go in
								lua_call(L, 1, 0);
							}
						}
						AUX_RenderTextProccesing();
						goto QUIT;
					}
					else if (!strcmp(tokens[1], "west")) {
						if (tmp_location->west) {
							if (tmp_location->callback_exit > 0) {
								lua_rawgeti(L, LUA_REGISTRYINDEX, tmp_location->callback_exit);
								lua_pushstring(L, "west"); //push direction the player is leaving in
								lua_call(L, 1, 0);
							}
							cur_scene->locations->data = tmp_location->west;
							tmp_location = cur_scene->locations->data;
							AUX_Display(tmp_location->on_enter);
							if (tmp_location->callback_enter > 0) {
								lua_rawgeti(L, LUA_REGISTRYINDEX, tmp_location->callback_enter);
								lua_pushstring(L, "west");
								lua_call(L, 1, 0);
							}
						}
						else {
							AUX_Display("There isn't anything west of here.");
							if (tmp_location->callback_invalid > 0) {
								lua_rawgeti(L, LUA_REGISTRYINDEX, tmp_location->callback_invalid);
								lua_pushstring(L, "west"); //let them know the direction the player tried to go in
								lua_call(L, 1, 0);
							}
						}
						AUX_RenderTextProccesing();
						goto QUIT;
					}
					else if (!strcmp(tokens[1], "east")) {
						if (tmp_location->east) {
							if (tmp_location->callback_exit > 0) {
								lua_rawgeti(L, LUA_REGISTRYINDEX, tmp_location->callback_exit);
								lua_pushstring(L, "east"); //push direction the player is leaving in
								lua_call(L, 1, 0);
							}
							cur_scene->locations->data = tmp_location->east;
							tmp_location = cur_scene->locations->data;
							AUX_Display(tmp_location->on_enter);
							if (tmp_location->callback_enter > 0) {
								lua_rawgeti(L, LUA_REGISTRYINDEX, tmp_location->callback_enter);
								lua_pushstring(L, "east");
								lua_call(L, 1, 0);
							}
						}
						else {
							AUX_Display("There isn't anything east of here.");
							if (tmp_location->callback_invalid > 0) {
								lua_rawgeti(L, LUA_REGISTRYINDEX, tmp_location->callback_invalid);
								lua_pushstring(L, "east"); //let them know the direction the player tried to go in
								lua_call(L, 1, 0);
							}
						}
						AUX_RenderTextProccesing();
						goto QUIT;
					}
				}
				else if (!cur_scene->locations)
					flogf("LUA EXCEPTION: Scene '%s' did not have a location map\n", cur_scene->name);
				s_scene* tmp_scene = NULL;
				HASH_FIND_STR(scenes, tokens[1], tmp_scene);
				if (!tmp_scene)
					AUX_Display("I could not find that place anywhere around me.");
				else if (tmp_scene != cur_scene) {
					if (cur_scene) {
						if (cur_scene->callback_exit > 0) {
							lua_rawgeti(L, LUA_REGISTRYINDEX, cur_scene->callback_exit);
							lua_pushstring(L, cur_scene->name);
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
						lua_pushstring(L, cur_scene->name);
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