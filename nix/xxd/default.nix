{ stdenv, fetchurl, autoconf, automake, libtool }:

let
  version = "8.0.0013";
in stdenv.mkDerivation {
  name = "xxd-${version}";

  buildInputs = [ autoconf automake libtool ];

  src = fetchurl {
    url = "http://github.com/vim/vim/archive/v${version}.tar.gz";
    sha256 = "46b509cd0c4d085cfec60b2e8d30daca568908c4d7dbcaa2af586ed897cc8d4b";
  };

  preConfigure = ''
    cd src/xxd
  '';

  installPhase = ''
    mkdir -p $out/bin
    install -m755 xxd $out/bin/
  '';

  meta = {
    description = "xxd stand-alone binary from vim source tree";
    homepage = "http://vim.wikia.com/wiki/Hex_dump";
    license = stdenv.lib.licenses.gpl2;
  };
}
