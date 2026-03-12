include_guard (DIRECTORY)

list (APPEND testapp_ccl_sources
	${CCL_DIR}/platform/cocoa/cocoamain.mm
)

list (APPEND testapp_bundle_resources
	${CCL_DIR}/packaging/mac/resource/MainMenu.xib
	${CCL_DIR}/packaging/mac/resource/cclapp_mac.xcassets
)

list (APPEND testapp_resources
	${testapp_bundle_resources}
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

ccl_add_skin_package (testapp ${CMAKE_CURRENT_LIST_DIR}/../skin)

set_target_properties (testapp PROPERTIES
	RESOURCE "${testapp_bundle_resources}"
	XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME "cclapp"
	MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_LIST_DIR}/../packaging/mac/Info.plist"
	XCODE_ATTRIBUTE_CODE_SIGN_ENTITLEMENTS "${CMAKE_CURRENT_LIST_DIR}/../packaging/mac/testapp.entitlements"

	XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "Apple Development"
)

if(${VENDOR_USE_PUBLISHER_CERTIFICATE})
	set_target_properties (testapp PROPERTIES
		XCODE_ATTRIBUTE_CODE_SIGN_STYLE "Manual"
		XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "${SIGNING_CERTIFICATE_MAC}"
	)
endif()

