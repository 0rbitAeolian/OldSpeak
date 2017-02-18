# -*- coding: utf-8 -*-

from __future__ import unicode_literals

import zmq
import imp
import logging
import inspect

from datetime import datetime
from zmq.devices import Device
from oldspeak.util import sanitize_name
from oldspeak.servers import Phase
from oldspeak.servers import Pipeline
from oldspeak.console.base import get_sub_parser_argv
from oldspeak.console.base import bootstrap_conf_with_gevent


DEFAULT_CONCURRENCY = 32


def execute_command_run_pipeline():
    from oldspeak.console.parsers.pipeline import parser
    args = parser.parse_args(get_sub_parser_argv())
    bootstrap_conf_with_gevent(args, loglevel=logging.DEBUG)

    module_name = ".".join([
        "oldspeak",
        "pipelines",
        sanitize_name(args.name).replace('-', '_'),
    ])

    module = imp.load_source(module_name, args.path)
    all_members = dict(
        map(
            lambda name_member3: (
                name_member3[1].name_member3[0], name_member3[1]), filter(
                lambda name_member: (
                    hasattr(
                        name_member[1], 'name') and isinstance(
                            name_member[1], type) and issubclass(
                                name_member[1], Pipeline)), inspect.getmembers(module))))

    PipelineClass = all_members.get(args.name)

    if not PipelineClass:
        print "invalid job type \033[1;32m'{0}'\033[0m at \033[1;34m{1}\033[0m, but I found these \033[1;33m{2}\033[0m".format(args.name, args.path, ", ".join([x.name for x in all_members.values()]))
        raise SystemExit(1)

    server = PipelineClass(args.name, concurrency=args.concurrency)

    pull_connect_addresses = list(args.pull_connect or [])
    sub_connect_addresses = list(args.sub_connect or [])
    server.run(
        sub_connect_addresses=sub_connect_addresses,
        sub_bind_address=args.sub_bind,
        pull_bind_address=args.pull_bind,
        pull_connect_addresses=pull_connect_addresses,
    )


def execute_command_run_phase():
    from oldspeak.console.parsers.phase import parser
    args = parser.parse_args(get_sub_parser_argv())
    bootstrap_conf_with_gevent(args)

    pull_connect_addresses = list(args.pull_connect or [])
    push_connect_addresses = list(args.push_connect or [])

    module_name = ".".join([
        "oldspeak",
        "phases",
        sanitize_name(args.job_type).replace('-', '_'),
    ])
    module = imp.load_source(module_name, args.path)
    all_members = dict(
        map(
            lambda name_member4: (
                name_member4[1].job_type, name_member4[1]), filter(
                lambda name_member1: (
                    hasattr(
                        name_member1[1], 'job_type') and isinstance(
                            name_member1[1], type) and issubclass(
                                name_member1[1], Phase)), inspect.getmembers(module))))

    PhaseClass = all_members.get(args.job_type)

    if not PhaseClass:
        print "invalid job type \033[1;32m'{0}'\033[0m at \033[1;34m{1}\033[0m, but I found these \033[1;33m{2}\033[0m".format(args.job_type, args.path, ", ".join([x.job_type for x in all_members.values()]))
        raise SystemExit(1)

    server = PhaseClass(
        pull_bind_address=args.pull_bind,
        pub_connect_address=args.pub_connect,
        concurrency=args.concurrency,
        push_connect_addresses=push_connect_addresses,
        pull_connect_addresses=pull_connect_addresses,
    )

    try:
        server.run()
    except KeyboardInterrupt:
        logging.info('exiting')


def execute_command_forwarder():
    from oldspeak.console.parsers.streamer import parser

    args = parser.parse_args(get_sub_parser_argv())
    bootstrap_conf_with_gevent(args)

    device = Device(zmq.FORWARDER, zmq.SUB, zmq.PUB)

    device.bind_in(args.subscriber)
    device.bind_out(args.publisher)
    device.setsockopt_in(zmq.SUBSCRIBE, b'')
    if args.subscriber_hwm:
        device.setsockopt_in(zmq.RCVHWM, args.subscriber_hwm)

    if args.publisher_hwm:
        device.setsockopt_out(zmq.SNDHWM, args.publisher_hwm)

    print "oldspeak forwarder started"
    print "date", datetime.utcnow().isoformat()
    print "subscriber", (getattr(args, 'subscriber'))
    print "publisher", (getattr(args, 'publisher'))
    device.start()


def execute_command_streamer():
    from oldspeak.console.parsers.streamer import parser

    args = parser.parse_args(get_sub_parser_argv())
    bootstrap_conf_with_gevent(args)

    device = Device(zmq.STREAMER, zmq.PULL, zmq.PUSH)

    device.bind_in(args.pull)
    device.bind_out(args.push)
    if args.pull_hwm:
        device.setsockopt_in(zmq.RCVHWM, args.pull_hwm)

    if args.push_hwm:
        device.setsockopt_out(zmq.SNDHWM, args.push_hwm)

    print "oldspeak streamer started"
    print "date", datetime.utcnow().isoformat()
    print "pull", (getattr(args, 'pull'))
    print "push", (getattr(args, 'push'))
    device.start()


def execute_command_run_bundle():
    from oldspeak.console.parsers.bundle import parser

    args = parser.parse_args(get_sub_parser_argv())
    bootstrap_conf_with_gevent(args, loglevel=logging.DEBUG)

    module_name = ".".join([
        "oldspeak",
        "pipelines",
        sanitize_name(args.name).replace('-', '_'),
    ])

    module = imp.load_source(module_name, args.path)
    all_members = dict(
        map(
            lambda name_member5: (
                name_member5[1].name_member5[0], name_member5[1]), filter(
                lambda name_member2: (
                    hasattr(
                        name_member2[1], 'name') and isinstance(
                            name_member2[1], type) and issubclass(
                                name_member2[1], Pipeline)), inspect.getmembers(module))))

    PipelineClass = all_members.get(args.name)

    if not PipelineClass:
        print "invalid job type \033[1;32m'{0}'\033[0m at \033[1;34m{1}\033[0m, but I found these \033[1;33m{2}\033[0m".format(args.name, args.path, ", ".join([x.name for x in all_members.values()]))
        raise SystemExit(1)

    server = PipelineClass(args.name, concurrency=args.concurrency)
    server.run_bundle(args.sub_bind, args.pull_bind)
