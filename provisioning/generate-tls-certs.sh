#!/bin/bash

# Script accepts a single argument, the fqdn for the cert
PROG=$(basename "$0")
HERE=$(cd "$(dirname "$0")" && pwd)
DOMAIN="${1}"
EMAIL="infosec@${DOMAIN}"

if [ -z "$DOMAIN" ]; then
    echo "Usage: ${PROG} <domain>"
    exit 1
fi

CSR_SUBJECT_C="EE"
CSR_SUBJECT_ST="Estonia"
CSR_SUBJECT_L="Tallinn"

function generate_subject () {
    declare -r root_domain="${1}"
    shift
    declare -a sub_domains=${*}

    cat <<EOF
C=${CSR_SUBJECT_C}
ST=${CSR_SUBJECT_ST}
L=${CSR_SUBJECT_L}
O=${root_domain}
CN=${root_domain}
OU=${root_domain}
EOF
    declare -a alternate_names=();

    for item in ${sub_domains[*]}; do
        alternate_names+=("DNS:${item}.${root_domain}")
    done
    if [ -n "${sub_domains[*]}" ]; then
        echo -e "subjectAltName=${alternate_names[*]}" | tr '[:space:]' ',' | sed 's/[,]*$//g'
    fi
    echo "emailAddress=infosec@${root_domain}"
}

file_prefix="lets_encrypt"
private_key_filename="${file_prefix}.${DOMAIN}.account.key"
public_key_filename="${private_key_filename}.pub"
domain_private_key_filename="${file_prefix}.${DOMAIN}.key"

csr_filename="${file_prefix}.${DOMAIN}.csr"
cert_filename="${file_prefix}.${DOMAIN}.cert"

declare -x EMAIL
declare -x DOMAIN
declare -a SUBDOMAINS

SUBDOMAINS+=( api dist www web static cdn )
SUBDOMAINS+=( blog )
SUBDOMAINS+=( downloads drop box dropbox drive )
SUBDOMAINS+=( github )
SUBDOMAINS+=( io data info files cloud )
SUBDOMAINS+=( jabber xmpp chat )
SUBDOMAINS+=( mail email smtp pop )
SUBDOMAINS+=( media video audio webrtc voip )
SUBDOMAINS+=( onion tor )
SUBDOMAINS+=( opensource gnu projects )
SUBDOMAINS+=( pgp gpg secure crypto crypt0 h4x0r aes ciphertext cipher )
SUBDOMAINS+=( public private public )


echo -e "\033[1;30mSUBDOMAINS:\n${SUBDOMAINS[*]}\033[0m" | tr '[:space:]' '\n'

function handle_error() {
    echo -ne "\033[0m"
    reason="${1}"
    shift
    echo -ne "\033[1;37mError: \033[0;31m${reason}\033[0m\n"
    echo -ne "\033[1;37musing password: \033[0;31m${reason}\033[0m\n"
    exit 1
}

function handle_success() {
    echo -ne "\033[0m"
    reason="${1}"
    shift
    echo -ne "\033[1;37mSuccess: \033[0;32m${reason}\033[0m\n"
}

function handle_info() {
    echo -ne "\033[0m"
    reason="${1}"
    shift
    echo -ne "\033[1;37mInfo: \033[0;34m${reason}\033[0m\n"
}

function handle_metadata() {
    echo -ne "\033[0m"
    reason="${1}"
    shift
    echo -ne "\033[0;33m${reason}\033[0m\n"
}


# generate private keys
if [ ! -f "${private_key_filename}" ]; then
    if openssl genrsa 4096 > "${private_key_filename}"; then
        handle_success "generated key: ${private_key_filename}"
    else
        handle_error "failed to generate private key: ${private_key_filename}"
    fi
fi


if [ ! -f "${public_key_filename}" ]; then
    # derive public key
    if openssl rsa -in "${private_key_filename}" -pubout > "${public_key_filename}"; then
        handle_success "derived public key: ${public_key_filename} from ${private_key_filename}"
    else
        handle_error "failed to derive public key: ${public_key_filename} from ${private_key_filename}"
    fi
fi

if [ ! -f "${domain_private_key_filename}" ]; then
    if openssl genrsa 4096 > "${domain_private_key_filename}"; then
        handle_success "generated key: ${domain_private_key_filename}"
    else
        handle_error "failed to generate private key: ${domain_private_key_filename}"
    fi
fi

CSRSBJCT="$(generate_subject "${DOMAIN}" "${SUBDOMAINS[*]}" | tr "\n" "/")"
echo -e "\033[1;30mCSR Subject:\n${CSRSBJCT}\033[0m" | tr '[:space:]' '\n'


if [ ! -f "${csr_filename}" ]; then
    # Generate the CSR
    if openssl req \
               -new \
               -batch \
               -subj "${CSRSBJCT}" \
               -key "${domain_private_key_filename}" \
               -out "${csr_filename}" ; then
        handle_success "generated self-signed certificate request: ${csr_filename} with key ${domain_private_key_filename}"
    else
        handle_error "failed to generate self-signed certificate request: ${csr_filename} with key ${domain_private_key_filename}"
    fi
fi
cp $private_key_filename user.key

# Generate the cert with letsencrypt
if python "${HERE}/sign_csr.py" --email="${EMAIL}" --public-key "${public_key_filename}" "${csr_filename}" > "${cert_filename}"; then
    handle_success "Generated Let's Encrypt Certificate"
else
    handle_error "Failed to sign CSR with Let's Encrypt"
fi
