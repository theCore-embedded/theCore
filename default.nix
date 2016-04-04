# TODO: comments
{ pkgs ? import <nixpkgs> {}
}:
let
  gcc-arm-embedded = pkgs.callPackage_i686 ./nix/gcc-arm-embedded {
    dirName = "5.0";
    subdirName = "5-2015-q4-major";
    version = "5.2-2015q4-20151219";
    releaseType = "major";
    sha256 = "12mbwl9iwbw7h6gwwkvyvfmrsz7vgjz27jh2cz9z006ihzigi50y";
  };

  cpputest = pkgs.callPackage ./nix/cpputest {};

in with pkgs; {
  coreEnv = stdenv.mkDerivation {
    name = "thecore";
    buildInputs = [ cmake gcc5 cppcheck cpputest gcc-arm-embedded doxygen ];
  };
}
