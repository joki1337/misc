#!/usr/bin/env python3
import os
import shutil
import tempfile
from subprocess import check_call

if __name__ == '__main__':

    check_call(['apt-get', 'update'])
    check_call([
        'apt-get', 'install', '-y',
        'make',
        'git',
        'gcc',
        'libx11-dev',
        'libxft-dev',
        'libxinerama-dev',
    ])

    old_wd = os.getcwd()
    tempdir = tempfile.mkdtemp()
    try:
        os.chdir(tempdir)
        check_call(['git', 'clone', 'https://git.suckless.org/dwm'])

        os.chdir('dwm')
        check_call(['make', 'install'])

    finally:
        print('deleting %s' % tempdir)
        os.chdir(old_wd)
        shutil.rmtree(tempdir)
