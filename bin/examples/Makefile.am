AM_LDFLAGS = -static

bin_PROGRAMS = normal normal_parallel ising

nobase_pkginclude_HEADERS = NormalSampler.h Ising2dSampler.h details/random_utils.h

normal_SOURCES = normal.cpp
normal_LDADD = ../../muninn/libmuninn.la

normal_parallel_SOURCES = normal_parallel.cpp
normal_parallel_LDADD = ../../muninn/libmuninn.la

ising_SOURCES = ising.cpp
ising_LDADD = ../../muninn/libmuninn.la
