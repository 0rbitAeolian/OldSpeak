#!/bin/bash

# Fork ofhttp://usrportage.de/archives/919-Batch-generating-SSL-certificates.html
# Author: d4v1ncy@protonmail.ch

# Script accepts a single argument, the fqdn for the cert
PROG=$(basename "$0")
HERE=$(cd "$(dirname "$0")" && pwd)
DOMAIN="${1}"
EMAIL="infosec@${DOMAIN}"

if [ -z "$DOMAIN" ]; then
    echo "Usage: ${PROG} <domain>"
    exit 1
fi

# secret=$(head -c 500 /dev/urandom | tr -dc a-z0-9A-Z | head -c 128)

# PASSPHRASE=$(echo -e "${secret}\n")
# ARMORED_PW=$(base64 <<< "$PASSPHRASE")
# Certificate details; replace items in angle brackets with your own info
#"/C=US/ST=Denial/L=Springfield/O=Dis/CN=www.example.com"
subj="
C=EE
ST=Estonia
L=Tallinn
O=$DOMAIN
CN=$DOMAIN
OU=$DOMAIN
subjectAltName=DNS:www.$DOMAIN,DNS:api.$DOMAIN,,DNS:mail.$DOMAIN,DNS:git.$DOMAIN,DNS:docs.$DOMAIN,DNS:gpg.$DOMAIN,DNS:secure.$DOMAIN,DNS:public.$DOMAIN,DNS:private.$DOMAIN
emailAddress=$EMAIL"

file_prefix="lets_encrypt"
# passphrase_filename="${file_prefix}.${DOMAIN}.pw"
private_key_filename="${file_prefix}.${DOMAIN}.account.key"
public_key_filename="${private_key_filename}.pub"
domain_private_key_filename="${file_prefix}.${DOMAIN}.key"

csr_filename="${file_prefix}.${DOMAIN}.csr"
cert_filename="${file_prefix}.${DOMAIN}.cert"

# Generate the server private key
declare -x EMAIL
declare -x DOMAIN
declare -x PASSPHRASE

function handle_error() {
    echo -ne "\033[0m"
    reason="${1}"
    shift
    unset PASSPHRASE
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

# if [ ! -f "${passphrase_filename}" ]; then
#     echo "$PASSPHRASE" > "${passphrase_filename}"
#     handle_success "stored auto-generated passphrase into ${passphrase_filename}"
# else
#     handle_error "passphrase file already exists: ${passphrase_filename}"
# fi

# handle_info "using auto-generated passphrase (base64):"
# handle_metadata "${ARMORED_PW}"

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

if [ ! -f "${csr_filename}" ]; then
    # Generate the CSR
    if openssl req \
               -new \
               -batch \
               -subj "$(echo -n "$subj" | tr "\n" "/")" \
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
