//************************************************************************************************
//
// CCL Test Application
//
// This file is part of Crystal Class Library (R)
// Copyright (c) 2025 CCL Software Licensing GmbH.
// All Rights Reserved.
//
// Licensed for use under either:
//  1. a Commercial License provided by CCL Software Licensing GmbH, or
//  2. GNU Affero General Public License v3.0 (AGPLv3).
// 
// You must choose and comply with one of the above licensing options.
// For more information, please visit ccl.dev.
//
// Filename    : testapp.cpp
// Description : Test Application
//
//************************************************************************************************

#define USE_PLUGINS	 !CCL_STATIC_LINKAGE
#define USE_SERVICES (1 && USE_PLUGINS)

#include "testapp.h"
#include "testmanager.h"
#include "appversion.h"

#include "ccl/base/development.h"
#include "ccl/base/storage/url.h"
#include "ccl/extras/tools/testcollectionregistry.h"
#include "ccl/main/cclargs.h"

#include "ccl/public/plugservices.h"
#include "ccl/public/plugins/classfactory.h"
#include "ccl/public/plugins/iservicemanager.h"
#include "ccl/public/systemservices.h"
#include "ccl/public/system/isysteminfo.h"
#include "ccl/public/system/ipackagehandler.h"
#include "ccl/public/base/iunittest.h"
#include "ccl/public/securityservices.h"
#include "ccl/public/netservices.h"
#include "ccl/public/cclversion.h"

#if CCL_STATIC_LINKAGE
#include "ccl/test/testsplugin.h"
#endif

using namespace CCL;

//////////////////////////////////////////////////////////////////////////////////////////////////

void ccl_app_init ()
{
	NEW TestApp;
	RootComponent::instance ().addComponent (NEW TestManager);

	// force linkage
	String hostname;
	System::GetNetwork ().getLocalHostname (hostname);
}

#if CCL_STATIC_LINKAGE
CCL_KERNEL_INIT_LEVEL (TestApp, kFirstRun)
{
	// register classes
	AutoPtr<ClassFactory> classFactory = ClassFactory::instance ();
	classFactory->setLocalizationEnabled ();

	CCL::TestsPlugin::registerClasses (*classFactory);

	System::GetPlugInManager ().registerFactory (classFactory);

	return true;
}

CCL_KERNEL_TERM_LEVEL (TestApp, kFirstRun)
{
	// unregister classes
	AutoPtr<ClassFactory> classFactory = ClassFactory::instance ();
	System::GetPlugInManager ().unregisterFactory (classFactory);
}
#endif

//************************************************************************************************
// TestApp
//************************************************************************************************

TestApp::TestApp ()
: Application (APP_ID, /*APP_COMPANY*/ 0, APP_NAME, APP_PACKAGE_ID)
{
	setWebsite (APP_WEBSITE);
}

//////////////////////////////////////////////////////////////////////////////////////////////////

bool TestApp::startup ()
{
	if(!Application::startup ())
		return false;

	// load theme
	Url skinFolder;
	GET_DEVELOPMENT_FOLDER_LOCATION (skinFolder, CCL_APPLICATION_BASEDIR, "skin")
	if(!loadTheme (skinFolder))
		return false;

	#if USE_PLUGINS
	// scan plug-ins
	scanPlugIns ();
	#endif

	#if USE_SERVICES
	// start services
	System::GetServiceManager ().startup ();
	#endif

	// register test factories
	TestCollectionRegistry::instance ().registerTestPlugIns ();

	// assign factory for strong content encryption
	System::GetPackageHandler ().setCryptoFactory (&System::GetCryptoFactory ());

	#if CCL_PLATFORM_DESKTOP
	// create main window
	createWindow ();
	#endif

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

bool TestApp::shutdown ()
{
	TestCollectionRegistry::instance ().unregisterTestPlugIns ();

	#if USE_SERVICES
	// stop services
	System::GetServiceManager ().shutdown ();
	#endif

	return Application::shutdown ();
}
