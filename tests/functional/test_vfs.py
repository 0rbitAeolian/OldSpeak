# -*- coding: utf-8 -*-
# import os
# from glob import glob
from tests.functional.fixtures import JohnDoe
from tests.functional.scenarios import storage_scenario
from oldspeak.persistence.vfs import Bucket
from oldspeak.persistence.vfs import System
# from oldspeak.persistence.vfs import Member

# import ipdb;ipdb.set_trace()


@storage_scenario
def test_versioned_bucket(context):
    my_bucket = Bucket(
        'my-bucket',
        author_name=JohnDoe.name,
        author_email=JohnDoe.email,
    )

    my_bucket.write_file(
        'hello-world.md',
        '\n'.join([
            '# Hello World\n',
            'Today is an important day',
        ])
    )
    # commit = my_bucket.save('adding a hello-world markdown')
    # commit.should.be.a('_pygit2.Commit')

    my_bucket.write_file(
        'todo/oldspeak.md',
        '\n'.join([
            '# OldSpeak TO-DO list\n',
            '- add white-listed fingerprints',
            '- query white-listed fingerprints',
            '- import public key',
            '- generate temporary sha1 auth tokens pointing to otps',
            '- generate random cookie tokens',
            '  - store cookie token in redis as a key that expires in 300 seconds',
            '  - whose value is the SHA1 auth token XORed with the fingerprint',
        ])
    )
    # commit = my_bucket.save('adding TO-DO list for oldspeak')
    # commit.should.be.a('_pygit2.Commit')

    my_bucket.write_file(
        'tasks/planning.md',
        '\n'.join([
            '# Backlog\n',
            '',
            '- less important 1',
            '- less important 2',
        ])
    )
    # commit = my_bucket.save('adding TO-DO list for backlog')
    # commit.should.be.a('_pygit2.Commit')

    sorted(my_bucket.list()).should.equal(sorted(['hello-world.md', 'todo/oldspeak.md', 'tasks/planning.md']))


@storage_scenario
def test_core_system_storage(context):
    system = System()

    result = system.add_fingerprint(
        JohnDoe.fingerprint,
        JohnDoe.email,
        None,
    )

    # result.should.be.a(tuple)
    # result.should.have.length_of(2)
    # tree, blob = result
    blob = result

    # tree.should.be.a('_pygit2.Tree')
    blob.should.be.a('_pygit2.Blob')
    system.list().should.equal(['fingerprints/9FF44C58C3F0456CCD41F4EE876863BB2759DF55.json'])
