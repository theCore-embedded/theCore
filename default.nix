# TODO: comments
let
  pkgs          = import <nixpkgs> {};
  stdenv        = pkgs.stdenv;
  callPackage   = pkgs.lib.callPackageWith pkgs;
  
  gcc-arm       = pkgs.callPackage_i686 ./nix/gnu-arm-gcc {
    dirName = "5.0";
    subdirName = "5-2015-q4-major";
    version = "5.2-2015q4-20151219";
    releaseType = "major";
    sha256 = "12mbwl9iwbw7h6gwwkvyvfmrsz7vgjz27jh2cz9z006ihzigi50y";
  };

  cpputest      = callPackage ./nix/cpputest {};
in with pkgs; rec {
  coreEnv = stdenv.mkDerivation rec {
    name = "thecore";
    buildInputs = [ cmake gcc5 cppcheck cpputest gcc-arm doxygen ];
  };
}

