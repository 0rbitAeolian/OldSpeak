admins = {
   "lincoln@{{ root_domain }}",
   "gabriel@{{ root_domain }}",
   "claudio@{{ root_domain }}",
}

modules_enabled = {
   "roster";
   "saslauth";
   "tls";
   "dialback";
   "disco";

   "private"; -- Private XML storage (for room bookmarks, etc.)
   "vcard"; -- Allow users to set vCards

   "legacyauth";
   "version";
   "uptime";
   "time";
   "ping";
   "register";
   "posix";
   "bosh";
   --"console"; -- telnet to port 5582 (needs console_enabled = true)
   --"httpserver"; -- Serve static files from a directory over HTTP
};
log = {
        debug = "/var/log/prosody/prosody.log";
        error = "/var/log/prosody/prosody.err";
}
authentication = "internal_hashed"
allow_registration = true;
consider_bosh_secure = true;
cross_domain_bosh = true;
bosh_max_inactivity = 300; -- 5 minutes
bosh_max_requests = 20;
bosh_default_hold = 5;
daemonize = true;
pidfile = "/var/run/prosody/prosody.pid";

bosh_ports = {
   {
      port = 5280;
      path = "http-bind";
   },
   {
      port = 5281;
      path = "http-bind";
      ssl = {
         key = "{{ root_domain_tls_key_path }}";
         certificate = "{{ root_domain_tls_cert_path }}";
      }
   }
}

https_ssl = {
   key = "{{ root_domain_tls_key_path }}";
   certificate = "{{ root_domain_tls_cert_path }}";
}

VirtualHost "{{ root_domain }}"
-- Assign this host a certificate for TLS, otherwise it would use the one
-- set in the global section (if any).
-- Note that old-style SSL on port 5223 only supports one certificate, and will always
-- use the global one.
ssl = {
   key = "{{ root_domain_tls_key_path }}";
   certificate = "{{ root_domain_tls_cert_path }}";
}

Component "conference.{{ root_domain }}" "muc"
