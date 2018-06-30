{ stdenv
, python36Packages
, fetchurl
, autoconf
, automake
, libtool }:

let
  version = "1.2.0";
in python36Packages.buildPythonPackage rec {
  name = "sphinxcontrib-fulltoc-${version}";

  buildInputs = [ python36Packages.sphinx ];

  src = fetchurl {
    url = "mirror://pypi/s/sphinxcontrib-fulltoc/${name}.tar.gz";
    sha256 = "c845d62fc467f3135d4543e9f10e13ef91852683bd1c90fd19d07f9d36757cd9";
  };

  meta = {
    description = "Include a full table of contents in your sidebar.";
    homepage = "http://sphinxcontrib-fulltoc.readthedocs.io/";
    license = stdenv.lib.licenses.asl20;
    platforms = stdenv.lib.platforms.unix;
  };
}
