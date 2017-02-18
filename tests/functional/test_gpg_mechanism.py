# -*- coding: utf-8 -*-

from oldspeak.crypt0 import InvitationRoster
from tests.functional.fixtures import JohnDoe
from tests.functional.scenarios import storage_scenario


@storage_scenario
def test_gpg_invite_user(context):
    roster = InvitationRoster()

    roster.get_public_key(JohnDoe.fingerprint).should.be.none
    roster.invite(JohnDoe.public_key)
    roster.get_public_key(
        JohnDoe.fingerprint).should.contain('PUBLIC KEY BLOCK')
