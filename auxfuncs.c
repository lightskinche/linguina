#include "mainheader.h"
//this file is for random aux functions, could be anything

//we dont need any parameters since all variables we are using are global variables, this function is mainly used for ease-of-reading
inline void AUX_Load_Libraries(void) { 
	luaL_openlibs(L); //load standered library
	//now we load our functions etc
	lua_pushcfunction(L, LUAPROC_Load_Image);
	lua_setglobal(L, "io_load_image");
	lua_pushcfunction(L, LUAPROC_Create_Texture);
	lua_setglobal(L, "create_texture");
}

void AUX_Handle_GameLoop(void) { //recieves text input, calls lua 'main' function and moves the text-based game forward
	//first, let's recieve input from the user
	static int shiftheld = 0, receving_input = 1; //if the shift key is held down or not
	if (receving_input) {
		while (SDL_PollEvent(&even)) {
			if (even.type == SDL_KEYDOWN) {
				for (int i = 0; i < MAX_BUFFER_SIZE; ++i) {
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
		}
		//printf("buf:%s\n",input_buf);
		//now render text
		SDL_RenderClear(renderer);
		{
			SDL_Surface* tmp_surf = TTF_RenderText_Blended_Wrapped(strd_font, input_buf, (SDL_Color) { 255, 255, 255 }, resolution_x);
			SDL_Texture* tmp_text = SDL_CreateTextureFromSurface(renderer, tmp_surf);
			if (tmp_surf) {
				SDL_RenderCopy(renderer, tmp_text, NULL, &(SDL_Rect){0, 0, tmp_surf->w, tmp_surf->h});
				SDL_DestroyTexture(tmp_text);
				SDL_FreeSurface(tmp_surf);
			}
		}
		SDL_RenderPresent(renderer);
	}
	else {
		//run the 'main' function before we do any processing
		lua_rawgeti(L, LUA_REGISTRYINDEX, main_ref);
		lua_call(L, 0, 0);
	}
}