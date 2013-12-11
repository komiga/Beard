
dofile("../premake_common.lua")

-- Test solution

precore.make_solution(
	"test",
	{"debug", "release"},
	{"x64", "x32"},
	nil,
	{
		"precore-generic",
		"beard-deps",
		"beard-import"
	}
)

-- Groups

include("general")
include("tty")
include("ui")

action_clean()
