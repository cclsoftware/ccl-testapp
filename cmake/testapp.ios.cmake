include_guard (DIRECTORY)

list (APPEND testapp_ccl_sources
	${CCL_DIR}/platform/cocoa/cocoamain.mm
)

set (testapp_xcode_resources
	${CMAKE_CURRENT_LIST_DIR}/../packaging/ios/resource/launchscreen.storyboard
	${CCL_DIR}/packaging/ios/resource/cclapp_ios.xcassets
)

list (APPEND testapp_resources
	${testapp_xcode_resources}
)

list (APPEND testapp_sources
	${testapp_resources}
)
source_group ("Resources" FILES ${testapp_resources})

set (testapp_FRAMEWORKS
	ccltext
	cclsystem
	cclgui
	cclsecurity
	cclnet
)

set (testapp_PLUGINS
	ccltest
	jsengine
)


set_target_properties (testapp PROPERTIES
	XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"
	RESOURCE "${testapp_xcode_resources}"
	XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME "cclapp"
	MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_LIST_DIR}/../packaging/ios/Info.plist"
)
