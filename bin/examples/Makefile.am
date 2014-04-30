AM_LDFLAGS = -static

bin_PROGRAMS = normal ising

nobase_pkginclude_HEADERS = NormalSampler.h Ising2dSampler.h details/random_utils.h

normal_SOURCES = normal.cpp
normal_LDADD = ../../muninn/libmuninn.la

ising_SOURCES = ising.cpp
ising_LDADD = ../../muninn/libmuninn.la
