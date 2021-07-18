function main()
myimage = io_load_image("resources/test.png")
print(tostring(myimage.type) .. " " .. tostring(myimage.data))
mytexture = create_texture(myimage)
print(tostring(mytexture.type) .. " " .. tostring(mytexture.data))
end
