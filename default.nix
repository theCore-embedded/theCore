# TODO: comments
{ pkgs ? import <nixpkgs> {}
}:
let
  cpputest = pkgs.callPackage ./nix/cpputest {};

in with pkgs; {
  coreEnv = stdenv.mkDerivation {
    name = "thecore";
    buildInputs = [ cmake gcc6 cppcheck cpputest gcc-arm-embedded-5_2 doxygen ];
  };
}
