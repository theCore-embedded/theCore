let
	pkgs = import <nixpkgs> {};
	stdenv = pkgs.stdenv;
	callPackage = pkgs.lib.callPackageWith pkgs;
	cpputest = callPackage ./nix/cpputest { };
	cmake = pkgs.cmake;
	gcc5 = pkgs.gcc5;
	cppcheck = pkgs.cppcheck;
in rec {
	coreEnv = stdenv.mkDerivation rec {
		name = "thecore";
		buildInputs = [ cmake gcc5 cppcheck ];
	};
}

