
dofile("precore_import.lua")

local _, G, P = precore.helpers()

precore.init(
	nil,
	{
		"precore.clang-opts",
		"precore.c++11-core",
		"precore.env-common",
	}
)

precore.import(".")

precore.apply_global("beard.projects")
precore.import("test")

precore.action_clean("out")
if _ACTION == "clean" then
	os.rmdir(G"${BUILD_PATH}")
end
