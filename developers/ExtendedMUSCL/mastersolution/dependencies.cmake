#if SOLUTION
# Dependencies of mastersolution:
#else
# Add your custom dependencies here:
#endif

# DIR will be provided by the calling file.

set(SOURCES
  ${DIR}/extendedmuscl.h
  ${DIR}/extendedmuscl.cc
  ${DIR}/slopelimfluxdiff.h
  ${DIR}/extendedmuscl_main.cc
)

set(LIBRARIES
  Eigen3::Eigen
)