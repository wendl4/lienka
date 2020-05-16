#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := beebot

include $(IDF_PATH)/make/project.mk

WEB_SRC_DIR = $(shell pwd)/html
$(eval $(call spiffs_create_partition_image,www,$(WEB_SRC_DIR)))
