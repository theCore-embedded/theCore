# TODO: comments
{ pkgs ? import <nixpkgs> {}
}:

let
  xxd = pkgs.callPackage ./nix/xxd {};
  fulltoc = pkgs.callPackage ./nix/sphinx-fulltoc {};

in with pkgs; {
  coreEnv = stdenv.mkDerivation {
    name = "thecore";
    buildInputs = [ python35Packages.cogapp which cmake gcc6 gdb cppcheck
                    cpputest gcc-arm-embedded-5 dfu-util
                    doxygen clang openocd perlPackages.ArchiveZip xxd
                    python27Packages.sphinx python27Packages.sphinx_rtd_theme
                    fulltoc ];
    shellHook = ''
        export GCC_ARM_EMBEDDED_PATH=${gcc-arm-embedded-5}
    '';
  };
}
