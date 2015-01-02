
-- Import precore

function import_precore()
	local env_name = "PRECORE_ROOT"
	local root_path = os.getenv(env_name)
	if nil == root_path or 0 == #root_path then
		error(
			"Environment variable '" .. env_name .. "' is not " ..
			"defined or is blank; assign it to precore's root directory"
		)
	end
	dofile(path.join(root_path, "precore.lua"))
end

import_precore()

-- Custom precore configs

precore.make_config(
"beard-strict", {{
project = function()
	-- NB: -Werror is a pita for GCC. Good for testing, though,
	-- since its error checking is better.
	configuration {"clang"}
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

precore.make_config(
"beard-deps", {{
project = function()
	configuration {}
		includedirs {
			precore.subst("${ROOT}/dep/duct/"),
			precore.subst("${ROOT}/dep/ceformat/"),
			precore.subst("${ROOT}/dep/am/")
		}
end}})

precore.make_config(
"beard-import", {{
project = function()
	configuration {}
		includedirs {
			precore.subst("${ROOT}/include/")
		}
		libdirs {
			precore.subst("${ROOT}/lib/")
		}

	configuration {"debug"}
		links {"beard_d"}

	configuration {"release"}
		links {"beard"}
end}})

-- Initialize precore

precore.init(
	nil,
	{
		"opt-clang",
		"c++11-core",
		"precore-env-root"
	}
)
