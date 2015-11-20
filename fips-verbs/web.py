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
    deploy_dir = '{}/fips-deploy/yakc/yakc-emsc-make-release/'.format(ws_dir)

    # copy the application files
    for name in ['yakc_app.js'] :
        log.info('> copy file: {}'.format(name))
        shutil.copy(deploy_dir + name, webpage_dir + '/' + name)

    # webpage files
    for name in ['index.html', 'font.ttf', 'style.css', 'emsc.js', 'about.html', 'favicon.png'] :
        log.info('> copy file: {}'.format(name))
        shutil.copy(proj_dir + '/web/' + name, webpage_dir + '/' + name)

    # copy kcc files
    for kcc in glob.glob(proj_dir + '/yakc_kcc/*.kcc') :
        log.info('> copy file: {}'.format(kcc))
        shutil.copy(kcc, webpage_dir + '/' + os.path.basename(kcc))

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
    webpage_dir = '{}/fips-deploy/yakc-webpage'.format(ws_dir)
    if os.path.isdir(webpage_dir) :
        shutil.rmtree(webpage_dir)
    os.makedirs(webpage_dir)

    # compile emscripten, pnacl and android samples
    if emscripten.check_exists(fips_dir) :
        project.gen(fips_dir, proj_dir, 'yakc-emsc-make-release')
        project.build(fips_dir, proj_dir, 'yakc-emsc-make-release')
    
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
                'open http://localhost:8000 ; python {}/mod/httpserver.py'.format(fips_dir),
                cwd = webpage_dir, shell=True)
        except KeyboardInterrupt :
            pass
    elif p == 'win':
        try:
            subprocess.call(
                'cmd /c start http://localhost:8000 && python {}/mod/httpserver.py'.format(fips_dir),
                cwd = webpage_dir, shell=True)
        except KeyboardInterrupt:
            pass
    elif p == 'linux':
        try:
            subprocess.call(
                'xdg-open http://localhost:8000; python {}/mod/httpserver.py'.format(fips_dir),
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

