{ stdenv, python27Packages, fetchurl, autoconf, automake, libtool }:

let
  version = "1.2.0";
in python27Packages.buildPythonPackage rec {
  name = "sphinxcontrib-fulltoc-${version}";

  src = fetchurl {
    url = "mirror://pypi/s/sphinxcontrib-fulltoc/${name}.tar.gz"; #md5=1b4326b588ae9e7bfe69b51670b74cfb
    sha256 = "c845d62fc467f3135d4543e9f10e13ef91852683bd1c90fd19d07f9d36757cd9";
  };

  meta = {
    description = "Include a full table of contents in your sidebar.";
    homepage = "http://sphinxcontrib-fulltoc.readthedocs.io/";
    license = stdenv.lib.licenses.asl20;
    platforms = stdenv.lib.platforms.unix;
  };
}

