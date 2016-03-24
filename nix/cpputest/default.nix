{ stdenv, fetchFromGitHub }:

with import <nixpkgs> {}; {
	sdlEnv = stdenv.mkDerivation {
		name = "cpputest";
		buildInputs = [ cmake gcc5 autoconf ];
		src = fetchFromGithHub {
			ownwer = "cpputest";
			repo = "cpputest";
			rev = "0e3b216c7ab365b67765e94aeb45085c4db029e0";
		};
	};
}

