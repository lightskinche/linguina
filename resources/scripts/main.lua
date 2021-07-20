--callbacks
function callback_workpls(dir)
engine.set_background(roblox_t,nil)
engine.display("You came from the " .. dir)
end
function callback_2(dir)
if dir == "south" then
engine.set_background(nil,nil)
else
engine.set_background(roblox_t,true)
end
end
function callback_3(dir)
if dir == "north" then
engine.display("That way is dangerous traveler!")
end
end

--this function runs on startup
function start()
   roblox_t = engine.load_texture("resources/test.png")
   test = engine.create_scene("test","I walked into the testing area, it was nice.","I left.","examine", nil)
   test:set("callback_enter", function(scene)engine.set_background(roblox_t,nil)end)
   test:load()
   nullzone = engine.create_scene("null","I walked into the null zone, nothingness took me over","I left the nullzone","examine",nil)
   nullzone:set("callback_enter",function(scene)engine.set_background(nil,nil)end)
   nullzone:load()
   --perfect, this is exactly how I want the locationmap function to look and work
   testlocations = engine.create_locationmap(3,3,2,2,
   {"bigtest","workpls", callback_workpls, callback_2, callback_3},{"cool spot","asf", nil, nil, nil},{"ssss","vbsbsb", nil, nil, nil},
   {"bigtesta","workplsa", nil, nil, nil},{"cool spota","asf", nil, nil, nil},{"ssssa","vbsbsb", nil, nil, nil},
   {"bigtestb","workplsb", nil, nil, nil},{"cool spotb","asf", nil, nil, nil},{"ssssb","vbsbsb", nil, nil, nil})
   nullzone:set("locations",testlocations)
  -- print(nullzone:get("locationmap"))
   --engine.destroy_locationmap(testlocations)
   engine.set_currentscene(test)
   engine.set_background(roblox_t,nil)
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
function main() --gets an arbitrary number of arguments
return true
end
