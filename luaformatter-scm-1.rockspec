package = "LuaFormatter"
version = "scm-1"
source = {
   url = "git+https://github.com/Koihik/LuaFormatter.git"
}
description = {
   summary = "Reformats your Lua source code.",
   detailed = "Reformats your Lua source code.",
   homepage = "https://github.com/Koihik/LuaFormatter",
   license = "Apache 2.0"
}
build = {
   type = "cmake",
   copy_directories = {"docs"}
}
