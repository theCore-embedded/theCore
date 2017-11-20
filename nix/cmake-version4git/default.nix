{ stdenv, fetchFromGitHub, libtool, cmake }:

stdenv.mkDerivation {
  name = "cmake-version4git";

  src = fetchFromGitHub {
    owner = "Oleh-Kravchenko";
    repo = "cmake-version4git";
    rev = "ba441ee2cecee3668d883a3d0b8f6eabafbfbb51";
  };

  buildInputs = [ libtool cmake ];

  meta = {
    description = "Projects version definition with Git tags and CMake";
    homepage = "https://github.com/Oleh-Kravchenko/cmake-version4git";
    license = stdenv.lib.licenses.bsd3;
  };
}
