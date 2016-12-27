#!/bin/python

from waflib import *
import os, sys

top = os.getcwd()
out = 'build'

plat_linux = False
plat_windows = False

if sys.platform.startswith('win32'):
	plat_windows = True
elif sys.platform.startswith('linux'):
	plat_linux = True

module_end = "bin"
if plat_linux:
	module_end = 'so'
elif plat_windows:
	module_end = 'dll'
	
g_comflags = ['-pthread', '-Wall']
if plat_windows:
	g_comflags += ['-static', '-static-libgcc', '-static-libstdc++']
g_cflags = ['-std=c11'] + g_comflags
g_cxxflags = ['-std=c++14'] + g_comflags

def btype_cflags(ctx):
	return {
		'DEBUG'   : g_cflags + ['-Og', '-ggdb3', '-march=core2', '-mtune=native'],
		'NATIVE'  : g_cflags + ['-Ofast', '-march=native', '-mtune=native'],
		'RELEASE' : g_cflags + ['-O3', '-march=core2', '-mtune=generic'],
	}.get(ctx.env.BUILD_TYPE, g_cflags)
def btype_cxxflags(ctx):
	return {
		'DEBUG'   : g_cxxflags + ['-Og', '-ggdb3', '-march=core2', '-mtune=native'],
		'NATIVE'  : g_cxxflags + ['-Ofast', '-march=native', '-mtune=native'],
		'RELEASE' : g_cxxflags + ['-O3', '-march=core2', '-mtune=generic'],
	}.get(ctx.env.BUILD_TYPE, g_cxxflags)

def options(opt):
	opt.load('gcc')
	opt.load('g++')
	opt.load('cs')
	
	opt.add_option('--build_type', dest='build_type', type='string', default='RELEASE', action='store', help='DEBUG, NATIVE, RELEASE')
	opt.add_option('--no_server', dest='bldsv', default=True, action='store_false', help='True/False')
	opt.add_option('--no_client', dest='bldcl', default=True, action='store_false', help='True/False')

def configure(ctx):
	
	ctx.env.BUILD_SERVER = ctx.options.bldsv
	ctx.env.BUILD_CLIENT = ctx.options.bldcl
	
	ctx.load('gcc')
	ctx.load('g++')
	ctx.load('cs')
	
	ctx.check(features='c cprogram', lib='z', uselib_store='ZLIB')
	ctx.check(features='c cprogram', lib='dl', uselib_store='DL')
	
	if ctx.env.BUILD_CLIENT:
		ctx.check(features='c cprogram', lib='jpeg', uselib_store='JPEG')
		ctx.check(features='c cprogram', lib='png', uselib_store='PNG')
	ctx.check(features='c cprogram', lib='pthread', uselib_store='PTHREAD')
	
	if plat_linux and ctx.env.BUILD_CLIENT:
		ctx.check(features='c cprogram', lib='GL', uselib_store='GL')
		ctx.check_cfg(path='sdl2-config', args='--cflags --libs', package='', uselib_store='SDL')
		
	elif plat_windows:
		ctx.check(features='c cprogram', lib='ws2_32', uselib_store='WS2')
		if ctx.env.BUILD_CLIENT:
			ctx.check(features='c cprogram', lib='opengl32', uselib_store='GL')
			ctx.check(features='c cprogram', lib='winmm', uselib_store='WMM')
			ctx.check_cfg(path='bash sdl2-config', args='--cflags --libs', package='', uselib_store='SDL')
	ctx.check_cfg(path='pkg-config', args='--cflags --libs', package='mono-2', uselib_store='MONO')
	ctx.check_cfg(path='pkg-config', args='--cflags --libs', package='bullet', uselib_store='BULLET')
	
	btup = ctx.options.build_type.upper()
	if btup in ['DEBUG', 'NATIVE', 'RELEASE']:
		Logs.pprint('PINK', 'Setting up environment for known build type: ' + btup)
		ctx.env.BUILD_TYPE = btup
		ctx.env.CFLAGS = btype_cflags(ctx)
		ctx.env.CXXFLAGS = btype_cxxflags(ctx)
		Logs.pprint('PINK', 'CFLAGS: ' + ' '.join(ctx.env.CFLAGS))
		Logs.pprint('PINK', 'CXXFLAGS: ' + ' '.join(ctx.env.CXXFLAGS))
		if btup == 'DEBUG':
			ctx.define('_DEBUG', 1)
		else:
			ctx.define('FINAL_BUILD', 1)
		ctx.define('ARCH_STRING', 'x86_64')
	else:
		Logs.error('UNKNOWN BUILD TYPE: ' + btup)

def build(bld):
	
	### MINIZIP ###
	
	minizip_files = bld.path.ant_glob('src/minizip/*.c')
	minizip = bld (
		features = 'c cstlib',
		target = 'minizip',
		includes = 'src/minizip/include/minizip',
		source = minizip_files,
	)
	
	### BOTLIB ###
	
	botlib_files = bld.path.ant_glob('src/botlib/*.cpp')
	botlib = bld (
		features = 'cxx cxxstlib',
		target = 'botlib',
		includes = ['src'],
		source = botlib_files,
		defines = ['BOTLIB'],
	)
	
	### SERVER/CLIENT ###
	
	clsv_common_files = bld.path.ant_glob('src/qcommon/*.cpp')
	clsv_common_files += bld.path.ant_glob('src/icarus/*.cpp')
	clsv_common_files += bld.path.ant_glob('src/server/*.cpp')
	clsv_common_files += bld.path.ant_glob('src/server/NPCNav/*.cpp')
	clsv_common_files += bld.path.ant_glob('src/mp3code/*.c')
	
	clsv_common_files += bld.path.ant_glob('src/sys/snapvector.cpp')
	clsv_common_files += bld.path.ant_glob('src/sys/sys_main.cpp')
	clsv_common_files += bld.path.ant_glob('src/sys/sys_event.cpp')
	clsv_common_files += bld.path.ant_glob('src/sys/sys_log.cpp')
	clsv_common_files += bld.path.ant_glob('src/sys/con_log.cpp')
	clsv_common_files += bld.path.ant_glob('src/sys/sys_unix.cpp')
	
	clsv_common_files += bld.path.ant_glob('src/sharp/*.cpp')
	
	clsv_common_files += bld.path.ant_glob('src/phys/*.cpp')
	
	if plat_linux:
		clsv_common_files += bld.path.ant_glob('src/sys/con_tty.cpp')
	elif plat_windows:
		clsv_common_files += bld.path.ant_glob('src/sys/con_win32.cpp')
	
	# CLIENT
	
	if bld.env.BUILD_CLIENT:
	
		client_files = bld.path.ant_glob('src/client/*.cpp')

		client_files += bld.path.ant_glob('src/sdl/sdl_window.cpp')
		client_files += bld.path.ant_glob('src/sdl/sdl_input.cpp')
		client_files += bld.path.ant_glob('src/sdl/sdl_sound.cpp')
		
		client = bld (
			features = 'cxx cxxprogram',
			target = 'jaownt',
			includes = ['src'],
			source = clsv_common_files + client_files,
			uselib = ['SDL', 'ZLIB', 'DL', 'PTHREAD', 'WS2', 'WMM', 'MONO', 'BULLET'],
			use = ['minizip', 'botlib'],
			install_path = os.path.join(top, 'install')
		)
	
	# SERVER
	
	if bld.env.BUILD_SERVER:
	
		server_files = bld.path.ant_glob('src/rd-dedicated/*.cpp')
		server_files += bld.path.ant_glob('src/null/*.cpp')
		server_files += bld.path.ant_glob('src/ghoul2/*.cpp')

		server = bld (
			features = 'cxx cxxprogram',
			target = 'jaowntded',
			includes = ['src', 'src/rd-vanilla'],
			source = clsv_common_files + server_files,
			defines = ['_CONSOLE', 'DEDICATED'],
			uselib = ['ZLIB', 'DL', 'PTHREAD', 'WS2', 'MONO', 'BULLET'],
			use = ['minizip', 'botlib'],
			install_path = os.path.join(top, 'install')
		)
		
	### GAME/CGAME/UI ###
	
	common_files = bld.path.ant_glob('src/qcommon/*.c')
	
	# GAME
	
	game_files = bld.path.ant_glob('src/game/*.c')
	
	game = bld (
		features = 'c cshlib',
		target = 'jampgame',
		includes = ['src'],
		source = game_files + common_files,
		uselib = ['PTHREAD'],
		defines = ['_GAME'],
		install_path = os.path.join(top, 'install', 'base')
	)
	
	game.env.cshlib_PATTERN = '%sx86_64.' + module_end
	
	if bld.env.BUILD_CLIENT:
	
	# CGAME
		
		cgame_files = bld.path.ant_glob('src/cgame/*.c')
		cgame_files += bld.path.ant_glob('src/game/bg_*.c')
		cgame_files += bld.path.ant_glob('src/game/AnimalNPC.c')
		cgame_files += bld.path.ant_glob('src/game/FighterNPC.c')
		cgame_files += bld.path.ant_glob('src/game/SpeederNPC.c')
		cgame_files += bld.path.ant_glob('src/game/WalkerNPC.c')
		cgame_files += bld.path.ant_glob('src/ui/ui_shared.c')
		
		cgame = bld (
			features = 'c cshlib',
			target = 'cgame',
			includes = ['src'],
			source = cgame_files + common_files,
			uselib = ['PTHREAD'],
			defines = ['_CGAME'],
			install_path = os.path.join(top, 'install', 'base')
		)
		
		cgame.env.cshlib_PATTERN = '%sx86_64.' + module_end
	
	# UI
	
		ui_files = bld.path.ant_glob('src/ui/*.c')
		ui_files += bld.path.ant_glob('src/game/bg_misc.c')
		ui_files += bld.path.ant_glob('src/game/bg_saberLoad.c')
		ui_files += bld.path.ant_glob('src/game/bg_saga.c')
		ui_files += bld.path.ant_glob('src/game/bg_vehicleLoad.c')
		ui_files += bld.path.ant_glob('src/game/bg_weapons.c')
		
		ui = bld (
			features = 'c cshlib',
			target = 'ui',
			includes = ['src'],
			source = ui_files + common_files,
			uselib = ['PTHREAD'],
			defines = ['UI_BUILD'],
			install_path = os.path.join(top, 'install', 'base')
		)
		
		ui.env.cshlib_PATTERN = '%sx86_64.' + module_end
	
	### RD-VANILLA ###
	
		rdvan_files = bld.path.ant_glob('src/rd-vanilla/*.cpp')
		rdvan_files += bld.path.ant_glob('src/rd-common/*.cpp')
		rdvan_files += bld.path.ant_glob('src/ghoul2/*.cpp')
		rdvan_files += bld.path.ant_glob('src/qcommon/matcomp.cpp')
		rdvan_files += bld.path.ant_glob('src/qcommon/q_math.cpp')
		rdvan_files += bld.path.ant_glob('src/qcommon/q_shared.cpp')
		
		rdvan = bld (
			features = 'cxx cxxshlib',
			target = 'rd-vanilla',
			includes = ['src', 'src/rd-vanilla'],
			source = rdvan_files,
			uselib = ['JPEG', 'PNG', 'GL', 'PTHREAD'],
			install_path = os.path.join(top, 'install')
		)
		
		rdvan.env.cxxshlib_PATTERN = '%s_x86_64.' + module_end
	
	### C# SCRIPTING LIBRARY ###
	
	monolib_files = bld.path.ant_glob('src/sharp/sharpsv/*.cs')
	
	monolib = bld (
		features = 'cs',
		bintype='library',
		gen = 'sharpsv.dll',
		name = 'sharpsv',
		source = monolib_files,
		install_path = os.path.join(top, 'install', 'sharp')
	)
	
	###
	
def clean(ctx):
	pass
