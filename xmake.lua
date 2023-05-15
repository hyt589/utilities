add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate", {outputdir = "./"})

local target_name = "hyt_common"

target(target_name)
    if hyt_build_shared then
        print("building hyt as shared")
        set_kind("shared")
        add_defines("HYT_EXPORT", "HYT_BUILD_SHARED")
    else
        print("building hyt as static")
        set_kind("static")
    end
    set_kind("shared")
    add_files("src/logging/*.cpp")
    add_files("src/scheduling/*.cpp")
    add_includedirs("include", {public = true})

target("demo")
    set_kind("binary")
    add_files("demo/*.cpp")
    add_deps(target_name)

