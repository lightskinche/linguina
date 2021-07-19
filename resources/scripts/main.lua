--this function runs on startup
function start(state)
   log("this was called on startup")
   test = create_scene("test","walk","leave","examine")
   locmap = create_locationmap(3,2,{"a","b"},{"c","d"},{"e","f"},{"g","h"},{"i","j"},{"k","l"})
   --coming along well
   locmap[2].examine = "test"
   log(locmap[2].examine)
   return true
end
--this function runs during the text input loop
function textmain(state, input_buf) --note that input_buf is not converted into tokens like in main
    if state.mode == "confirmation" then
       if string.sub(input_buf,1,1) == 'w' then
	     return true
	   else
 		 return false, "There was not w present"
	   end
	end
end
--this function gets run after text input is recieved
function main(state) --gets an arbitrary number of arguments
testtest = find_scene_u("test")
if testtest then
state.display(testtest.on_enter)
destroy_scene(testtest)
else
state.display("SCENE WAS NOT FOUND\nPANIC\nNOOOOO\nBAD\nBAD\nBAD\nW")
end
return true
end
