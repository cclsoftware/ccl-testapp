include_guard (DIRECTORY)

list (APPEND  testapp_ccl_sources
	${CCL_DIR}/platform/win/winmain.cpp
)

list (APPEND testapp_icons
	1	${CCL_DIR}/packaging/win/resource/cclapp.ico
)

ccl_embed_manifest (testapp "${CCL_DIR}/packaging/win/application.manifest")
