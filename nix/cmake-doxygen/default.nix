{ stdenv, fetchFromGitHub, libtool, cmake }:

stdenv.mkDerivation {
  name = "cmake-doxygen";

  src = fetchFromGitHub {
    owner = "Oleh-Kravchenko";
    repo = "cmake-doxygen";
    rev = "3bde6b8";
    sha256 = "0qbf39la8xpckjj36f37qr9m932ky0w35s3rfql7xw9yp74vz3f5";
  };

  buildInputs = [ libtool cmake ];

  meta = {
    description = "Easy Doxygen integration with CMake.";
    homepage = "https://github.com/Oleh-Kravchenko/cmake-doxygen";
    license = stdenv.lib.licenses.bsd3;
  };
}
