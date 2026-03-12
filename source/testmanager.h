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
// Filename    : testmanager.h
// Description : Test Manager
//
//************************************************************************************************

#ifndef _testmanager_h
#define _testmanager_h

#include "ccl/app/component.h"

#include "ccl/public/base/iprogress.h"
#include "ccl/public/gui/commanddispatch.h"

namespace CCL {

class TestTreeModel;
class TestResultListModel;

//************************************************************************************************
// TestManager
//************************************************************************************************

class TestManager: public Component,
				   public AbstractProgressNotify,
				   public CommandDispatcher<TestManager>
{
public:
	TestManager ();
	~TestManager ();

	// Commands
	bool onRunSelected (CmdArgs args);
	bool onRunAll (CmdArgs args);
	bool onCancel (CmdArgs args);

	// Component
	DECLARE_COMMANDS (TestManager)
	DECLARE_COMMAND_CATEGORY ("Unit Test", Component)
	IUnknown* CCL_API getObject (StringID name, UIDRef classID) override;

	// IProgressNotify
	void CCL_API setTitle (StringRef title) override;
	void CCL_API beginProgress () override;
	void CCL_API endProgress () override;
	void CCL_API updateProgress (const State& state) override;
	void CCL_API setProgressText (StringRef text) override;
	tbool CCL_API isCanceled () override;
	IProgressNotify* CCL_API createSubProgress () override;

	CLASS_INTERFACE (IProgressNotify, Component)

protected:
	bool canceled;
	TestTreeModel* testTree;
	TestResultListModel* resultList;
};

} // namespace CCL

#endif // _testmanager_h
