# -*- coding: utf-8 -*-
from __future__ import unicode_literals
import logging
import sys
import io
import os
import hashlib
import getpass
import coloredlogs
import json

from inbox import Inbox

reload(sys)
sys.setdefaultencoding('utf-8')

inbox = Inbox()

USER = os.getenv('USER', None) or getpass.getuser()


@inbox.collate
def handle(to, sender, body, subject=None, **kw):
    try:
        conversation_uuid = hashlib.sha256(":".join(map(repr, [to, sender]))).hexdigest()
        folder = './inbox/{}'.format(conversation_uuid)

        if not os.path.isdir(folder):
            os.makedirs(folder)

        email_uuid = hashlib.sha256(":".join(map(repr, [to, subject, sender, body]))).hexdigest()

        maildata = {
            'body': body,
            'conversation_uuid': conversation_uuid,
            'email_uuid': email_uuid,
            'sender': sender,
            'subject': subject,
            'to': to,
        }
        # maildata.update(kw)
        meta = json.dumps(maildata, indent=2)
        logging.info(meta)

        target = os.path.join(folder, '{}.json'.format(email_uuid))
        with io.open(target, 'wb') as fd:
            fd.write(meta)
    except:
        logging.exception('failed to handle email from %s to %s', sender, to)


def main():
    PORT = int(os.getenv('INBOX_PORT', '25'))

    if PORT < 1024 and USER != 'root':
        print "port {} is available only to root, not {USER}".format(PORT, **dict(os.environ))
        raise SystemExit(1)

    print "mailserver listening on", PORT
    inbox.serve(address='0.0.0.0', port=PORT)


if __name__ == '__main__':
    coloredlogs.install(level=logging.DEBUG)
    main()
