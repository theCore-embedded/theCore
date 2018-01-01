# TODO: comments
{ pkgs ? import <nixpkgs> {}
}:

let
  xxd = pkgs.callPackage ./nix/xxd {};
  fulltoc = pkgs.callPackage ./nix/sphinx-fulltoc {};
  cmake-doxygen = pkgs.callPackage ./nix/cmake-doxygen {};
  cmake-version4git = pkgs.callPackage ./nix/cmake-version4git {};

in with pkgs; with python35Packages; {
  coreEnv = stdenv.mkDerivation {
    name = "thecore";
    hardeningDisable = [ "all" ];
    buildInputs = [
      # With Python configuration requires a special wrapper
#      (python35.buildEnv.override {
#        ignoreCollisions = true;
#        extraLibs = with python35Packages; [
          cogapp
          jsonschema
          sphinx
          sphinx_rtd_theme
          recommonmark
#        ];
#      })

      which cmake gcc6 gdb cppcheck git
      cpputest gcc-arm-embedded-5 dfu-util
      doxygen  llvmPackages.clang-unwrapped openocd
      perlPackages.ArchiveZip xxd
      fulltoc cmake-doxygen cmake-version4git plantuml graphviz ];

    shellHook = ''
        export GCC_ARM_EMBEDDED_PATH=${gcc-arm-embedded-5}
    '';
  };
}
