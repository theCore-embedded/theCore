{ stdenv, fetchFromGitHub, libtool, cmake, git }:

stdenv.mkDerivation {
  name = "cmake-version4git";

  src = fetchFromGitHub {
    owner = "Oleh-Kravchenko";
    repo = "cmake-version4git";
    rev = "676cbd3ddbab695db692191a710a80ade9f443e3";
    sha256 = "0h40pdq4sk4fwns82xmb25g4iydgq0mfx36j330mz2fwgk1awphw";
  };

  buildInputs = [ libtool cmake git ];

  meta = {
    description = "Projects version definition with Git tags and CMake";
    homepage = "https://github.com/Oleh-Kravchenko/cmake-version4git";
    license = stdenv.lib.licenses.bsd3;
  };
}
