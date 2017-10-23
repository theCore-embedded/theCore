# TODO: comments
{ pkgs ? import <nixpkgs> {}
}:

let
  xxd = pkgs.callPackage ./nix/xxd {};
  fulltoc = pkgs.callPackage ./nix/sphinx-fulltoc {};
  cmake-doxygen = pkgs.callPackage ./nix/cmake-doxygen {};

in with pkgs; {
  coreEnv = stdenv.mkDerivation {
    name = "thecore";
    hardeningDisable = [ "all" ];
    buildInputs = [
      # With Python configuration requiring a special wrapper
      (python35.buildEnv.override {
        ignoreCollisions = true;
        extraLibs = with python35Packages; [
          cogapp
          jsonschema
          sphinx
          sphinx_rtd_theme
        ];
      })

      which cmake gcc6 gdb cppcheck
      cpputest gcc-arm-embedded-5 dfu-util
      doxygen  llvmPackages.clang-unwrapped openocd
      perlPackages.ArchiveZip xxd
      fulltoc cmake-doxygen plantuml graphviz ];

    shellHook = ''
        export GCC_ARM_EMBEDDED_PATH=${gcc-arm-embedded-5}
    '';
  };
}
