--[[ A solution contains projects, and defines the available configurations

http://industriousone.com/premake/user-guide

example: http://opende.svn.sourceforge.net/viewvc/opende/trunk/build/premake4.lua?revision=1708&view=markup

http://bitbucket.org/anders/lightweight/src/tip/premake4.lua

]]
solution "rc-unitd"
	configurations { "Debug", "Release" }
	objdir "../obj"
 
-- convenience library
project "unit"
	kind "StaticLib"
	language "C++"
	targetdir "../lib"
	files { "../src/lib/**.h", "../src/lib/**.cpp" }
	
	includedirs { "../src",
				  "../externals/",
			 	  "../externals/xmlframework",
			  	  "../externals/oscframework" }
	libdirs { "../externals/xmlframework",
      		  "../externals/oscframework" }
	
	configuration "linux"
		buildoptions { "`pkg-config --cflags sdl`", "`pkg-config --cflags liblo`" }
		linkoptions { "`pkg-config --libs sdl`", "`pkg-config --libs liblo`" }
	
	configuration "macosx"
		-- MacPorts
		includedirs { "/opt/local/include" }
		libdirs { "/opt/local/lib" }

	configuration "Debug"
		defines { "DEBUG" }
		flags { "Symbols" }

	configuration "Release"
		defines { "NDEBUG" }
		flags { "Optimize" } 

-- rc-unit executable
project "rc-unitd"
	kind "ConsoleApp"
	language "C++"
	targetdir "../bin"
	files { "../src/rc-unitd/**.h", "../src/rc-unitd/**.cpp" }
	
	includedirs { "../src", "../src/lib",
				  "../externals/",
			 	  "../externals/xmlframework",
			  	  "../externals/oscframework" }
	libdirs { "../externals/xmlframework",
      		  "../externals/oscframework" }
	links { "unit", "oscframework", "xmlframework" }

	configuration "linux"
		buildoptions { "`pkg-config --cflags sdl`", "`pkg-config --cflags liblo`" }
		linkoptions { "`pkg-config --libs sdl`", "`pkg-config --libs liblo`" }

	configuration 'macosx'
		-- MacPorts
		includedirs { "/opt/local/include"}
		libdirs { "/opt/local/lib" }
		links { "lo", "pthread", "SDLmain", "SDL" }
		linkoptions { "-Wl,-framework,Cocoa", "-Wl,-framework,OpenGL",
					  "-Wl,-framework,ApplicationServices",
					  "-Wl,-framework,Carbon", "-Wl,-framework,AudioToolbox",
					  "-Wl,-framework,AudioUnit", "-Wl,-framework,IOKit" }

	configuration "Debug"
		defines { "DEBUG" }
		flags { "Symbols" }

	configuration "Release"
		defines { "NDEBUG" }
		flags { "Optimize" }
		
-- lsjs executable
project "lsjs"
	kind "ConsoleApp"
	language "C++"
	targetdir "../bin"
	files { "../src/lsjs/**.h", "../src/lsjs/**.cpp" }

	includedirs { "../src", "../src/lib",
			 	  "../externals/",
			 	  "../externals/xmlframework",
			  	  "../externals/oscframework" }

	configuration "linux"
		buildoptions { "`pkg-config --cflags sdl`" }
		linkoptions { "`pkg-config --libs sdl`" }

	configuration 'macosx'
		-- MacPorts
		includedirs { "/opt/local/include"}
		libdirs { "/opt/local/lib" }
		links { "SDLmain", "SDL" }
		linkoptions { "-Wl,-framework,Cocoa", "-Wl,-framework,OpenGL",
					  "-Wl,-framework,ApplicationServices",
					  "-Wl,-framework,Carbon", "-Wl,-framework,AudioToolbox",
					  "-Wl,-framework,AudioUnit", "-Wl,-framework,IOKit" }

	configuration "Debug"
		defines { "DEBUG" }
		flags { "Symbols" }

	configuration "Release"
		defines { "NDEBUG" }
		flags { "Optimize" }
		
-- rc-unit-notifier executable
project "rc-unit-notifier"
	kind "ConsoleApp"
	language "C++"
	targetdir "../bin"
	files { "../src/rc-unit-notifier/**.h", "../src/rc-unit-notifier/**.cpp" }
	
	includedirs { "../src", "../src/lib",
			 	  "../externals/",
			 	  "../externals/xmlframework",
			  	  "../externals/oscframework" }
	libdirs { "../externals/xmlframework",
      		  "../externals/oscframework" }
	links { "oscframework" }

	configuration "linux"
		buildoptions { "`pkg-config --cflags sdl`" }
		linkoptions { "`pkg-config --libs sdl`" }

	configuration 'macosx'
		-- MacPorts
		includedirs { "/opt/local/include"}
		libdirs { "/opt/local/lib" }
		links { "lo", "pthread", "SDLmain", "SDL" }
		linkoptions { "-Wl,-framework,Cocoa", "-Wl,-framework,OpenGL",
					  "-Wl,-framework,ApplicationServices",
					  "-Wl,-framework,Carbon", "-Wl,-framework,AudioToolbox",
					  "-Wl,-framework,AudioUnit", "-Wl,-framework,IOKit" }

	configuration "Debug"
		defines { "DEBUG" }
		flags { "Symbols" }

	configuration "Release"
		defines { "NDEBUG" }
		flags { "Optimize" }
		
