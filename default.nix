# TODO: comments
{ pkgs ? import <nixpkgs> {},
  docs ? false # Build dependencies required to generate documentation
}:

let
  xxd = pkgs.callPackage ./nix/xxd {};
  fulltoc = pkgs.callPackage ./nix/sphinx-fulltoc {};
  cmake-doxygen = pkgs.callPackage ./nix/cmake-doxygen {};
  cmake-version4git = pkgs.callPackage ./nix/cmake-version4git {};
  # Additionall documentation dependencies take enourmous time to download
  # and build. They are not needed by default.
  docs_deps = with pkgs; with python36Packages; if docs then
                [ sphinx
                  sphinx_rtd_theme
                  recommonmark
                  plantuml
                  graphviz
                  fulltoc
                  doxygen
                  cmake-doxygen ]
              else
                  [ ];
in with pkgs; with python36Packages; {
  coreEnv = stdenv.mkDerivation {
    name = "thecore";
    hardeningDisable = [ "all" ];
    buildInputs = [
      cogapp jsonschema which cmake gcc6 gdb cppcheck git
      cpputest gcc-arm-embedded-5 dfu-util minicom
      llvmPackages.clang-unwrapped openocd parse
      perlPackages.ArchiveZip xxd cmake-version4git ] ++ docs_deps;

    shellHook = ''
        export GCC_ARM_EMBEDDED_PATH=${gcc-arm-embedded-5}
        export TERM='linux'
    '';
  };
}
