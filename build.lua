
local S, G, R = precore.helpers()

precore.make_config_scoped("beard.env", {
	once = true,
}, {
{global = function()
	precore.define_group("BEARD", os.getcwd())
end}})

precore.make_config("beard.strict", nil, {
{project = function()
	configuration {}
		flags {
			"FatalWarnings"
		}
		buildoptions {
			"-Wno-missing-braces"
		}

	configuration {"linux"}
		buildoptions {
			"-pedantic-errors",
			"-Wextra",

			"-Wuninitialized",
			"-Winit-self",

			"-Wmissing-field-initializers",
			"-Wredundant-decls",

			"-Wfloat-equal",
			"-Wold-style-cast",

			"-Wnon-virtual-dtor",
			"-Woverloaded-virtual",

			"-Wunused",
			"-Wundef",
		}
end}})

precore.make_config("beard.dep", nil, {
"duct.dep",
"ceformat.dep",
"am.dep",
{project = function(p)
	configuration {}
		includedirs {
			G"${BEARD_ROOT}/include/"
		}

	if not p.env["NO_LINK"] then
		libdirs {
			G"${BEARD_BUILD}/lib/"
		}

		configuration {"debug"}
			links {"beard_d"}

		configuration {"release"}
			links {"beard"}
	end
end}})

precore.make_config_scoped("beard.projects", {
	once = true,
}, {
{global = function()
	precore.make_solution(
		"Beard",
		{"debug", "release"},
		{"x64", "x32"},
		nil,
		{
			"precore.generic",
		}
	)

	precore.make_project(
		"lib",
		"C++", "SharedLib",
		G"${BEARD_BUILD}/lib/",
		G"${BEARD_BUILD}/out/",
		{
			NO_LINK = true,
		}, {
			"beard.strict",
			"beard.dep",
		}
	)

	configuration {"debug"}
		targetname("beard")
		targetsuffix("_d")

	configuration {}
		files {
			"src/Beard/**"
		}
end}})

precore.apply_global({
	"precore.env-common",
	"beard.env",
})

precore.import(G"${DEP_PATH}/duct")
precore.import(G"${DEP_PATH}/ceformat")
precore.import(G"${DEP_PATH}/am")
