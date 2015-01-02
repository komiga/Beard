
dofile("premake_common.lua")

-- Core solution

precore.make_solution(
	"Beard",
	{"debug", "release"},
	{"x64", "x32"},
	nil,
	{
		"precore-generic",
		"beard-strict",
		"beard-deps"
	}
)

-- Core library

precore.make_project(
	"beard",
	"C++", "SharedLib",
	"lib/", "out/",
	nil, nil
)

configuration {"debug"}
	targetsuffix("_d")

configuration {}
	includedirs {
		"include/"
	}
	files {
		"src/Beard/**"
	}

precore.action_clean("out", "lib")
