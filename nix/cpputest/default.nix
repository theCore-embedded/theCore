{ stdenv, fetchFromGitHub, autoconf, automake, libtool }:

stdenv.mkDerivation {
  name = "cpputest-3.7.2";

  preConfigure = ''
    autoreconf -i
  '';

  enableParallelBuilding = true;

  buildInputs = [ autoconf automake libtool ];

  src = fetchFromGitHub {
    owner = "cpputest";
    repo = "cpputest";
    rev = "0e3b216c7ab365b67765e94aeb45085c4db029e0";
    sha256 = "0slljbkzmjhplg36ml1jpilbhvh4kccsbjmmhdimm3hrh2m465zg";
  };

  meta = {
    description = "Unit testing and mocking framework for C/C++";
    homepage = "http://cpputest.github.io/";
    license = stdenv.lib.licenses.bsd3;
  };
}
