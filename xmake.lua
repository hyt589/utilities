add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate", {outputdir = "./"})

target("hyt-utils")
    set_kind("static")
    add_files("src/logging/*.cpp")
    add_files("src/scheduling/*.cpp")
    add_includedirs("include", {public = true})

