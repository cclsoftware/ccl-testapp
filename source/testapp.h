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
// Filename    : testapp.h
// Description : Test Application
//
//************************************************************************************************

#ifndef _testapp_h
#define _testapp_h

#include "ccl/app/application.h"

namespace CCL {

//************************************************************************************************
// TestApp
//************************************************************************************************

class TestApp: public Application
{
public:
	TestApp ();

	// Application
	bool startup () override;
	bool shutdown () override;
};

} // namespace CCL

#endif // _testapp_h
