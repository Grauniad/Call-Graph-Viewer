EXPORT_INCLUDES=$(shell echo *.h)
SOURCES=$(shell echo *.cpp)

MODE=CPP
TARGET_LIB=libCallgrindData
TAGS_FILE=profiler-libcallgrinddata-c++.tags

LINKED_LIBS=libSource libIOInterface libUtils libString libProfData

ADDITIONAL_INCLUDE_FLAGS+="../libExec/"

include ../makefile.core.include
