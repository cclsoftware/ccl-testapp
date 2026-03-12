#************************************************************************************************
#
# This file is part of Crystal Class Library (R)
# Copyright (c) 2025 CCL Software Licensing GmbH.
# All Rights Reserved.
#
# Licensed for use under either:
#  1. a Commercial License provided by CCL Software Licensing GmbH, or
#  2. GNU Affero General Public License v3.0 (AGPLv3).
# 
# You must choose and comply with one of the above licensing options.
# For more information, please visit ccl.dev.
#
# Filename    : CMakeLists.txt
# Description : CCL Test
#
#************************************************************************************************

cmake_minimum_required (VERSION 3.30 FATAL_ERROR)

project ("CCL Test")
set (VENDOR_PRODUCT_NAME "CCL Test")
set (PROJECT_VENDOR ccl)

# Find dependencies
include (vendor OPTIONAL)

find_package (ccl ${CCL_VERSION_REQUIREMENT} REQUIRED COMPONENTS 
	cclapp
	cclbase
	cclextras-tools
	cclgui
	cclnet
	cclsecurity
	cclsystem
	ccltest
	ccltext
)

find_package (skins)
find_package (cclspy)
find_package (jsengine REQUIRED)


# Add targets
ccl_add_app (testapp GUI
	VENDOR ccl
	VERSION_FILE "${CMAKE_CURRENT_LIST_DIR}/../source/appversion.h"
	VERSION_PREFIX APP
)

ccl_set_product_name (testapp ${testapp_NAME})
ccl_set_startup_project (testapp)

ccl_use_skin_packages (testapp neutralbase neutraldesign)
ccl_add_skin_package (testapp ${CMAKE_CURRENT_LIST_DIR}/../skin)

list (APPEND testapp_resources
	${CMAKE_CURRENT_LIST_DIR}/../resource/cclgui.config
	${CMAKE_CURRENT_LIST_DIR}/../resource/commands.xml
	${CMAKE_CURRENT_LIST_DIR}/../resource/menubar.xml
)

list (APPEND testapp_source_files
	${CMAKE_CURRENT_LIST_DIR}/../source/testapp.cpp
	${CMAKE_CURRENT_LIST_DIR}/../source/testapp.h
	${CMAKE_CURRENT_LIST_DIR}/../source/testmanager.cpp
	${CMAKE_CURRENT_LIST_DIR}/../source/testmanager.h
)

list (APPEND testapp_ccl_sources
	${CCL_DIR}/main/cclmain.cpp
)

source_group ("source" FILES ${testapp_source_files})
source_group ("source\\ccl" FILES ${testapp_ccl_sources})

list (APPEND testapp_sources ${testapp_source_files} ${testapp_ccl_sources})

ccl_add_resources (testapp ${testapp_resources})
target_sources (testapp PRIVATE ${testapp_sources})

target_link_libraries (testapp PRIVATE 
	cclapp
	cclbase
	cclextras-tools
	cclgui
	cclnet
	cclsecurity
	cclsystem
	ccltext
)

ccl_add_dependencies (testapp 
	ccltest
	jsengine
	cclspy
)
