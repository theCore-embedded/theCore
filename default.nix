# TODO: comments
{ pkgs ? import <nixpkgs> {}
}:

let
  xxd = pkgs.callPackage ./nix/xxd {};

in with pkgs; {
  coreEnv = stdenv.mkDerivation {
    name = "thecore";
    buildInputs = [ python35Packages.cogapp which cmake gcc6 gdb cppcheck
                    cpputest gcc-arm-embedded-5 dfu-util
                    doxygen clang openocd perlPackages.ArchiveZip xxd ];
    shellHook = ''
        export GCC_ARM_EMBEDDED_PATH=${gcc-arm-embedded-5}
    '';
  };
}
