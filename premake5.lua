-- premake5.lua
workspace "PuduScheduler"
    architecture "x64"
    configurations {"Debug","Release","Dist"}
    startproject "PuduScheduler"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
include "PuduScheduler"
include "Walnut/WalnutExternal.lua"
include "Raven/RavenExternal.lua"
