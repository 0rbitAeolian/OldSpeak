ansible-bitcoind
=========

This ansible role installs and configure a bitcoind server

Requirements
------------

This role requires Ansible 1.4 higher and platforms listed in the metadata file.


Role Variables
--------------

Variables follows the name and default values of bitcoind parameters as defined here: <https://en.bitcoin.it/wiki/Running_Bitcoin>

Available options (and there default values) are:

    # Use bitcoin testnet
    bitcoind_testnet: 0

    # Enable RPC server
    bitcoind_server: 1

    # connect through a proxy, replace by proxy IP when needed, eg: 127.0.0.1:9050
    bitcoind_proxy: False

	# switch mining on/off
    bitcoind_gen: 0

	# Maintain a full transaction index
    bitcoind_txindex: 0

	# Do not load the wallet and disable wallet RPC calls
    bitcoind_disablewallet: 0

    # Maximum number of inbound+outbound connections.
    bitcoind_maxconnections: 125

	### RPC Server options ###

	# RPC username, mandatory if server=1
	bitcoind_rpcuser:

	# RPC password, mandatory if server=1
	bitcoind_rpcpassword:

	# How many seconds bitcoin will wait for a complete RPC HTTP request.
    bitcoind_rpctimeout: 30

    # By default, only RPC connections from localhost are allowed.  Specify
    # as many rpcallowip= settings as you like to allow connections from
    # other hosts (and you may use * as a wildcard character).
    # NOTE: opening up the RPC port to hosts outside your local
    # trusted network is NOT RECOMMENDED, because the rpcpassword
    # is transmitted over the network unencrypted.
    bitcoind_rpcallowip: [ "127.0.0.1", "192.168.1.*" ]

	# Listen for RPC connections on this TCP port. Uses default port if
	# not set
	bitcoind_rpcport:

Example Playbook
----------------

Including an example of how to use your role (for instance, with variables passed in as parameters) is always nice for users too:

    - hosts: servers
      roles:
         - {
             role: vineus.bitcoind,
             bitcoind_rpcuser: Ulysses,
             bitcoind_rpcpassword: S3cr3t!
           }

License
-------

MIT

Acknowledgements
----------------

Lots of inspiration from: <https://github.com/sivel/ansible-newrelic>, thx!
