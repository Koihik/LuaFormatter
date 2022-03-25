rockspec_format = "3.0"
package = "LuaFormatter"
version = "scm-1"
source = {
   url = "https://github.com/Koihik/LuaFormatter.git"
}
description = {
   summary = "Reformats your Lua source code.",
   detailed = "Reformats your Lua source code.",
   homepage = "https://github.com/Koihik/LuaFormatter",
   license = "Apache 2.0"
}
build = {
   type = "cmake",
   variables = {
      CMAKE_INSTALL_PREFIX = "$(PREFIX)"
   },
   copy_directories = {"docs"}
}
