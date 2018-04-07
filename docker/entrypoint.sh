#!/bin/sh

# Hack to handle missing certificates during nix-shell run
export SSL_CERT_FILE=/root/.nix-profile/etc/ssl/certs/ca-bundle.crt
echo $SSL_CERT_FILE

if [ -n "$*" ]; then
    nix-shell --arg docs true /expr --run "$*"
else
    nix-shell --arg docs true /expr
fi
