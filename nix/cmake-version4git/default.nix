{ stdenv, fetchFromGitHub, libtool, cmake, git }:

stdenv.mkDerivation {
  name = "cmake-version4git";

  src = fetchFromGitHub {
    owner = "Oleh-Kravchenko";
    repo = "cmake-version4git";
    rev = "ba441ee2cecee3668d883a3d0b8f6eabafbfbb51";
    sha256 = "184yagvjwwcjchfx6c37xs9f5fc82hdbkybjxy9cc6q73x0vax4a";
  };

  buildInputs = [ libtool cmake git ];

  meta = {
    description = "Projects version definition with Git tags and CMake";
    homepage = "https://github.com/Oleh-Kravchenko/cmake-version4git";
    license = stdenv.lib.licenses.bsd3;
  };
}
