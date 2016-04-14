# TODO: comments
{ stdenv
, fetchFromGitHub
, autoconf
, automake
, libtool
, gcc5
}:

stdenv.mkDerivation {
  name = "cpputest-3.7.2";

  configurePhase = ''
    autoreconf -i
    ./configure --prefix=$out
  '';

  buildInputs = [ gcc5 autoconf automake libtool ];

  src = fetchFromGitHub {
    owner = "cpputest";
    repo = "cpputest";
    rev = "0e3b216c7ab365b67765e94aeb45085c4db029e0";
    sha256 = "0slljbkzmjhplg36ml1jpilbhvh4kccsbjmmhdimm3hrh2m465zg";
  };

  # TODO: add metadata
}
