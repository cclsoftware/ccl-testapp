include_guard (DIRECTORY)

list (APPEND testapp_ccl_sources
	${CCL_DIR}/platform/android/androidmain.cpp
)

source_group ("source\\core" FILES ${corelib_android_jnionload_sources})

list (APPEND testapp_ccl_sources
	${corelib_android_jnionload_sources}
)

ccl_add_deployment_project (testapp "dev.ccl.testapp")
