--this function runs on startup
function start()
   roblox_t = load_texture("resources/test.png")
   test = create_scene("test","I walked into the testing area, it was nice.","I left.","examine", nil)
   test.examine.locations = create_locationmap(1,1,{"asdf","fghj"})
   log(tostring(test.examine.locations))
   test.callback_enter = function(scene)set_background(roblox_t,nil)end
   load_scene(test)
   nullzone = create_scene("null","I walked into the null zone, nothingness took me over","I left the nullzone","examine",nil)
   nullzone.callback_enter = function(scene)set_background(nil,nil)end
   load_scene(nullzone)
   set_currentscene(test)
   set_background(roblox_t,nil)
   return true
end
--this function runs during the text input loop
function textmain(mode, input_buf) --note that input_buf is not converted into tokens like in main
    if mode == "confirmation" then
	   if input_buf == "go" then --if the only thing they put is "go"
			return false, "Give a location I should go to. (or do help go)"
	   else
       return true
	   end
	end
end
--this function gets run after text input is recieved
function main(state) --gets an arbitrary number of arguments
return true
end
