# TODO: comments
{ stdenv, fetchFromGitHub }:

let 
  pkgs = import <nixpkgs> {};
in with pkgs; stdenv.mkDerivation {
  name = "cpputest-3.7.2";

  configurePhase = ''
  autoreconf -i
  echo "$prefix"
  ./configure --prefix=$prefix
  '';

  buildPhase = ''
  make
  '';

  installPhase = ''
  make install
  '';

  dontStrip = true;

  buildInputs = [ cmake gcc5 autoconf automake libtool ];
  src = fetchFromGitHub {
    owner = "cpputest";
    repo = "cpputest";
    rev = "0e3b216c7ab365b67765e94aeb45085c4db029e0";
    sha256 = "0slljbkzmjhplg36ml1jpilbhvh4kccsbjmmhdimm3hrh2m465zg";
  };
}


