# This section performs image update, it shouldn't be used for development at all
FROM thecoreembedded/thecore AS update

RUN rm -rv /expr
ADD expr/ /expr
RUN nix-shell -Q --pure /expr

# Keep entrypoint the same

FROM nixos/nix:1.11 as base

RUN nix-channel --add https://nixos.org/channels/nixpkgs-unstable nixpkgs
RUN nix-channel --update

ADD expr/ expr/
# Prepare shell so during docker run there will be no need to build all again
RUN nix-shell --pure expr/

ADD entrypoint.sh /entrypoint.sh
# Probably will be overriden in build scripts
WORKDIR /root

ENTRYPOINT [ "/entrypoint.sh" ]
