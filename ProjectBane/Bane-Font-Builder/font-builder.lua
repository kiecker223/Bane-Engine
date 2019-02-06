group "App"
project "BaneFontBuilder"
kind "ConsoleApp"
includedirs {
    "../Bane-Common",
    "../Bane-Math",
    "../Bane-Core",
    "../External/include",
    "../Bane-Platform",
    "../Bane-Platform/Platform",
}
characterset "ASCII"
libdirs {"../External/libs/"}
links { "BaneCore", "BanePlatform", "freetype.lib" }
filter "system:windows"
    links { "DevIL.lib", "ILU.lib" }
    includedirs {"../Bane-Systems/Platform-Windows/"}
files {"**.h", "**.cpp"}

debugdir "../Build/Debug/"