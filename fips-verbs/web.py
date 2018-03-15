'''
Build or serve the KC emu webpage
'''

import os
import yaml 
import shutil
import subprocess
import glob
from distutils.dir_util import copy_tree 

from mod import log, util, project, emscripten, android, nacl

#-------------------------------------------------------------------------------
def deploy_webpage(fips_dir, proj_dir, webpage_dir) :
    """builds the final webpage under under fips-deploy/oryol-webpage"""
    ws_dir = util.get_workspace_dir(fips_dir)
    asmjs_dir = '{}/fips-deploy/yakc/yakc-emsc-make-release/'.format(ws_dir)
    wasm_dir  = '{}/fips-deploy/yakc/yakc-wasm-make-release/'.format(ws_dir)

    # webpage files
    copy_tree(proj_dir+'/web/_site', webpage_dir)

    # copy the application files
    shutil.copy(asmjs_dir + 'yakcapp.js', webpage_dir + '/yakcapp.js')
    shutil.copy(wasm_dir  + 'yakcapp.js', webpage_dir + '/yakcapp.wasm.js')
    shutil.copy(wasm_dir  + 'yakcapp.wasm', webpage_dir + '/yakcapp.wasm.txt')

    # copy game/rom image files
    for fname in glob.glob(proj_dir + '/files/*') :
        if os.path.isfile(fname):
            shutil.copy(fname, webpage_dir + '/' + os.path.basename(fname))

    # if the virtualkc directory exists, copy everything there
    # so that a simple git push is enough to upload
    # the webpage
    vkc_dir = '{}/virtualkc'.format(ws_dir)
    if os.path.isdir(vkc_dir) :
        log.info(">>> updating virtualkc repository")
        copy_tree(webpage_dir, vkc_dir)

#-------------------------------------------------------------------------------
def build_deploy_webpage(fips_dir, proj_dir) :
    # if webpage dir exists, clear it first
    ws_dir = util.get_workspace_dir(fips_dir)
    webpage_dir = '{}/fips-deploy/yakc-webpage/virtualkc'.format(ws_dir)
    if os.path.isdir(webpage_dir) :
        shutil.rmtree(webpage_dir)
    os.makedirs(webpage_dir)

    # compile
    if emscripten.check_exists(fips_dir) :
        project.gen(fips_dir, proj_dir, 'yakc-emsc-make-release')
        project.build(fips_dir, proj_dir, 'yakc-emsc-make-release')
        project.gen(fips_dir, proj_dir, 'yakc-wasm-make-release')
        project.build(fips_dir, proj_dir, 'yakc-wasm-make-release')
   
    # build the webpage via jekyll
    subprocess.call('jekyll build', cwd=proj_dir+'/web', shell=True)

    # deploy the webpage
    deploy_webpage(fips_dir, proj_dir, webpage_dir)

    log.colored(log.GREEN, 'Generated web page under {}.'.format(webpage_dir))

#-------------------------------------------------------------------------------
def serve_webpage(fips_dir, proj_dir) :
    ws_dir = util.get_workspace_dir(fips_dir)
    webpage_dir = '{}/fips-deploy/yakc-webpage'.format(ws_dir)
    p = util.get_host_platform()
    if p == 'osx' :
        try :
            subprocess.call(
                'open http://localhost:8000/virtualkc/ ; python {}/mod/httpserver.py'.format(fips_dir),
                cwd = webpage_dir, shell=True)
        except KeyboardInterrupt :
            pass
    elif p == 'win':
        try:
            subprocess.call(
                'cmd /c start http://localhost:8000/virtualkc/ && python {}/mod/httpserver.py'.format(fips_dir),
                cwd = webpage_dir, shell=True)
        except KeyboardInterrupt:
            pass
    elif p == 'linux':
        try:
            subprocess.call(
                'xdg-open http://localhost:8000/virtualkc/; python {}/mod/httpserver.py'.format(fips_dir),
                cwd = webpage_dir, shell=True)
        except KeyboardInterrupt:
            pass

#-------------------------------------------------------------------------------
def run(fips_dir, proj_dir, args) :
    if len(args) > 0 :
        if args[0] == 'build' :
            build_deploy_webpage(fips_dir, proj_dir)
        elif args[0] == 'serve' :
            serve_webpage(fips_dir, proj_dir)
        else :
            log.error("Invalid param '{}', expected 'build' or 'serve'".format(args[0]))
    else :
        log.error("Param 'build' or 'serve' expected")

#-------------------------------------------------------------------------------
def help() :
    log.info(log.YELLOW +
             'fips web build\n' +
             'fips web serve\n' +
             log.DEF +
             '    build the YAKC webpage')

