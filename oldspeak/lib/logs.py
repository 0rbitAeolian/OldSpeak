# -*- coding: utf-8 -*-
from __future__ import absolute_import
from __future__ import unicode_literals

import os
import sys
import logging
from coloredlogs import ColoredFormatter
from pythonjsonlogger.jsonlogger import JsonFormatter


def configure_logging(level_name, stream=None, filename=None):
    relevant_loggers = [
        logging.getLogger(),
        logging.getLogger('oldspeak'),
        logging.getLogger('sqlalchemy'),
        logging.getLogger('werkzeug'),
    ]
    available_handlers = [
    ]

    logging.Logger.manager.loggerDict.clear()
    level = getattr(logging, bytes(level_name), b'INFO')

    TEST_MODE = os.getenv('TEST_MODE')
    TEST_NOLOGS = TEST_MODE in ['nologs', 'no-logs']

    if TEST_NOLOGS:
        stream = None
        filename = None
        level_name = 'WARNING'

    elif not TEST_NOLOGS:
        stream = sys.stderr
        filename = None
        level_name = 'DEBUG'

    stream_handler = None
    file_handler = None

    if stream:
        stream_handler = logging.StreamHandler(stream=stream)
        available_handlers.append(stream_handler)

    if filename:
        file_handler = logging.FileHandler(filename=filename, encoding='utf-8')
        available_handlers.append(file_handler)

    if not stream and not filename:
        available_handlers.append(logging.NullHandler())

    def setup_logger(logger):
        logger.handlers = []
        logger.setLevel(level=level)
        for handler in available_handlers:
            logger.addHandler(handler)

    if stream_handler:
        stream_handler.setLevel(level)
        stream_handler.setFormatter(ColoredFormatter(
            fmt='%(asctime)s %(name)s %(levelname)s %(message)s'))

    if file_handler:
        json_formatter = JsonFormatter(
            '%(levelname)s %(asctime)s %(module)s %(process)d %(message)s %(pathname)s $(lineno)d $(funcName)s')

        file_handler.setLevel(level)
        file_handler.setFormatter(json_formatter)

    for logger in relevant_loggers:
        setup_logger(logger)
