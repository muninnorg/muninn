# Supress warnings from enum comparisons in Eigen
add_definitions ("-Wno-enum-compare")

add_library(muninn 
  CGE.cpp
  GE.cpp
  Factories/CGEfactory.cpp
  Histories/MultiHistogramHistory.cpp
  MLE/MLE.cpp
  tools/CanonicalAverager.cpp
  utils/utils.cpp
  utils/TArrayUtils.cpp
  utils/timer.cpp
  utils/StatisticsLogger.cpp
  utils/MessageLogger.cpp
  utils/StatisticsLogReader.cpp
  utils/nonlinear/newton.cpp
  WeightSchemes/LinearPolatedWeights.cpp
  DiffusionOptimized/DiffusionEstimator.cpp
)
