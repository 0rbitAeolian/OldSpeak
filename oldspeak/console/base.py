#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os
import sys
import json
import logging
import argparse
import gevent.monkey

from oldspeak.version import version


LOGO = '''\n
\033[1;30m  _ \  |     __ \ \033[0;32m   ___|   _ \  ____|    \    |  /
\033[1;30m |   | |     |   |\033[0;32m \___ \  |   | __|     _ \   | /
\033[1;30m |   | |     |   |\033[0;32m       | ___/  |      ___ \  |/
\033[1;30m\___/ _____|____/ \033[0;32m _____/ _|    _____|_/    _\_|\_\n
\033[0;34m{0}\033[0m\n\n
\033[0m'''


def bootstrap_conf_with_gevent(args, loglevel=logging.DEBUG):
    gevent.monkey.patch_all(thread=True, select=True, subprocess=True)


def get_sub_parser_argv():
    argv = sys.argv[2:]
    return argv


def get_main_parser_argv():
    argv = sys.argv[1:2]
    return argv


def execute_command_version():
    parser = argparse.ArgumentParser(
        prog='oldspeak version --json',
        description='prints the software version')

    parser.add_argument(
        '--json',
        action='store_true',
        default=False,
        help='shows the version as a json')

    args = parser.parse_args(get_sub_parser_argv())

    if args.json:
        print json.dumps({'version': version, 'name': 'OldSpeak'}, indent=2)
    else:
        print get_logo('v{0}'.format(version))


def get_logo(*lines):
    if 'LOGO' in os.environ:
        return ''

    os.environ['LOGO'] = LOGO
    return LOGO.format('\n'.join(lines))


def show_logo(*lines):
    sys.stderr.write(get_logo(*lines))
    sys.stderr.flush()
