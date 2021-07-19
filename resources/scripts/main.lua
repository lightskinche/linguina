--this function runs on startup
function start(state)
   log("this was called on startup")
   locmap = create_locationmap(3,2,{"a","b"},{"c","d"},{"e","f"},{"g","h"},{"i","j"},{"k","l"})
   --coming along well
   locmap[2].examine = "test"
   log(locmap[2].examine)
   --dont know why this was so hard to get right
   test = create_scene("test","walk","leave","examine", locmap)
   print(test.locations[2])
   texture = state.load_texture("resources/test.png")
   texture2 = state.load_texture("resources/test2.png")
   return true
end
--this function runs during the text input loop
function textmain(state, input_buf) --note that input_buf is not converted into tokens like in main
    if state.mode == "confirmation" then
       if string.sub(input_buf,1,1) == 'w' then
	     return true
	   else
 		 return false, "There was not a w present"
	   end
	end
end
--this function gets run after text input is recieved
function main(state) --gets an arbitrary number of arguments
testtest = find_scene_u("test")
if a then return false end
if testtest then
state.set_background(texture)
state.display(testtest.on_enter)
destroy_scene(testtest)
else
a = true
state.set_background(texture2, true)
state.destroy_texture(texture)
state.display("The a par tip redact theory is one of the best developments mankind has acheived; it is truely the pinical of creation and represents us very well as a specesis and we should ap arsfsfsffksjflk.")
end
return true
end
