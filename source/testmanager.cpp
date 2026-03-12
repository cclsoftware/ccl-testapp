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
// Filename    : testmanager.cpp
// Description : Test Manager
//
//************************************************************************************************

#include "testmanager.h"

#include "ccl/public/collections/unknownlist.h"
#include "ccl/public/gui/iparameter.h"
#include "ccl/public/gui/framework/iitemmodel.h"
#include "ccl/public/gui/framework/iuserinterface.h"
#include "ccl/public/gui/graphics/types.h"
#include "ccl/public/gui/graphics/igraphics.h"

#include "ccl/public/plugservices.h"
#include "ccl/public/guiservices.h"

#include "ccl/base/collections/objectarray.h"
#include "ccl/base/message.h"

#include "ccl/extras/tools/testcollectionregistry.h"
#include "ccl/extras/tools/testrunner.h"

namespace CCL {

//************************************************************************************************
// TestNode
//************************************************************************************************

class TestNode: public Object
{
public:
	DECLARE_CLASS (TestNode, Object)

	TestNode (TestCollectionRegistry* reg = nullptr)
	: registry (reg),
	  suite (nullptr),
	  test (-1)
	{}
	TestNode (ITestSuite* suite): registry (nullptr), suite (suite), test (-1) {}
	TestNode (ITestSuite* suite, int test): registry (nullptr), suite (suite), test (test) {}

	bool isRegistry () const { return registry != nullptr; }

	bool isSuite () const { return suite != nullptr && test == -1; }

	bool isTest () const { return suite != nullptr && test != -1; }

	TestCollectionRegistry* registry;
	ITestSuite* suite;
	int test;
};

//************************************************************************************************
// TestTreeModel
//************************************************************************************************

class TestTreeModel: public Object,
					 public ItemViewObserver<AbstractItemModel>
{
public:
	TestTreeModel ();
	~TestTreeModel ();

	TestNode* getCurrentNode () const { return currentNode; }

	// IItemModel
	tbool CCL_API getRootItem (ItemIndex& index) override;
	tbool CCL_API canExpandItem (ItemIndexRef index) override;
	tbool CCL_API getSubItems (IUnknownList& items, ItemIndexRef index) override;
	tbool CCL_API getItemTitle (String& title, ItemIndexRef index) override;
	tbool CCL_API onItemFocused (ItemIndexRef index) override;
	void CCL_API viewAttached (IItemView* itemView) override;

	CLASS_INTERFACE (IItemModel, Object)

protected:
	TestNode* rootNode;
	TestNode* currentNode;
};

//************************************************************************************************
// TestResultListModel
//************************************************************************************************

class TestResultListModel: public Object,
						   public AbstractItemModel,
						   public ITestReporter
{
public:
	TestResultListModel (IProgressNotify* progressNotify);

	// IItemModel
	int CCL_API countFlatItems () override;
	tbool CCL_API drawCell (ItemIndexRef index, int column, const DrawInfo& info) override;

	// ITestReporter
	void beginTestRun (int numTests, StringRef filter) override;
	void endTestRun () override;
	void addResult (TestResult* testResult) override;
	bool allTestsPassed () const override;

	CLASS_INTERFACE2 (IItemModel, ITestReporter, Object)

protected:
	ObjectArray testResults;
	IProgressNotify* progressNotify;

	int numTestsPlanned;
	int numTestsProcessed;

	bool hasFailedTests;
};

} // namespace CCL

using namespace CCL;

//////////////////////////////////////////////////////////////////////////////////////////////////
// Parameter Tags
//////////////////////////////////////////////////////////////////////////////////////////////////

namespace Tag 
{
	enum Tags
	{
		kProgress = 100,
		kProgressText
	};
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Commands
//////////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_COMMANDS (TestManager)
	DEFINE_COMMAND ("Unit Test", "Run Selected", TestManager::onRunSelected)
	DEFINE_COMMAND ("Unit Test", "Run All", TestManager::onRunAll)
	DEFINE_COMMAND ("Unit Test", "Cancel", TestManager::onCancel)
END_COMMANDS (TestManager)

//************************************************************************************************
// TestNode
//************************************************************************************************

DEFINE_CLASS_HIDDEN (TestNode, Object)

//************************************************************************************************
// TestManager
//************************************************************************************************

TestManager::TestManager ()
: Component (CCLSTR ("TestManager")),
  canceled (false),
  testTree (NEW TestTreeModel),
  resultList (NEW TestResultListModel (this))
{
	paramList.setController (this);
	paramList.addInteger (0, 100, CSTR ("progress"), Tag::kProgress);
	paramList.addString (CSTR ("progressText"), Tag::kProgressText);
}

//////////////////////////////////////////////////////////////////////////////////////////////////

TestManager::~TestManager ()
{
	testTree->release ();
	resultList->release ();
}

//////////////////////////////////////////////////////////////////////////////////////////////////

IUnknown* CCL_API TestManager::getObject (StringID name, UIDRef classID)
{
	if(name == "testTree")
		return ccl_as_unknown (testTree);
	else if(name == "resultList")
		return ccl_as_unknown (resultList);
	return nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_COMMANDS (TestManager, Component)

//////////////////////////////////////////////////////////////////////////////////////////////////

bool TestManager::onRunSelected (CmdArgs args)
{
	if(args.checkOnly ())
		return true;

	canceled = false;

	TestRunner runner;
	runner.addTestReporter (resultList);

	TestNode* currentNode = testTree->getCurrentNode ();
	if(currentNode)
	{
		if(currentNode->isRegistry ())
			runner.run ();

		else if(currentNode->isSuite ())
		{
			String filter = currentNode->suite->getName ();
			filter.append ("*");
			runner.run (filter);
		}

		else if(currentNode->isTest ())
		{
			int testIndex = currentNode->test;
			String filter = currentNode->suite->getTestName (testIndex);
			filter.prepend ("*");
			runner.run (filter);
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

bool TestManager::onRunAll (CmdArgs args)
{
	if(args.checkOnly ())
		return true;

	canceled = false;
	TestRunner runner;
	runner.addTestReporter (resultList);

	runner.run ();
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

bool TestManager::onCancel (CmdArgs args)
{
	if(args.checkOnly ())
		return true;

	canceled = true;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void CCL_API TestManager::setTitle (StringRef title)
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void CCL_API TestManager::beginProgress ()
{
	paramList.byTag (Tag::kProgress)->setValue (0.);
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void CCL_API TestManager::endProgress ()
{
	// TODO: Needed?
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void CCL_API TestManager::updateProgress (const State& state)
{
	paramList.byTag (Tag::kProgress)->setNormalized ((float)state.value);
	System::GetGUI ().flushUpdates (false);
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void CCL_API TestManager::setProgressText (StringRef text)
{
	paramList.byTag (Tag::kProgressText)->setValue (text);
}

//////////////////////////////////////////////////////////////////////////////////////////////////

tbool CCL_API TestManager::isCanceled ()
{
	return canceled;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

IProgressNotify* CCL_API TestManager::createSubProgress ()
{
	return nullptr;
}

//************************************************************************************************
// TestTreeModel
//************************************************************************************************

TestTreeModel::TestTreeModel ()
: currentNode (nullptr)
{
	rootNode = NEW TestNode (&TestCollectionRegistry::instance ());
}

//////////////////////////////////////////////////////////////////////////////////////////////////

TestTreeModel::~TestTreeModel ()
{
	rootNode->release ();
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void CCL_API TestTreeModel::viewAttached (IItemView* itemView)
{
	ItemViewObserver<AbstractItemModel>::viewAttached (itemView);

	UnknownPtr<ITreeView> treeView (itemView);
	if(treeView)
		treeView->expandItem (treeView->getRootItem ());
}

//////////////////////////////////////////////////////////////////////////////////////////////////

tbool CCL_API TestTreeModel::getItemTitle (String& title, ItemIndexRef index)
{
	TestNode* node = unknown_cast<TestNode> (index.getObject ());
	if(!node)
		return false;

	if(node->isRegistry ())
		title = CCLSTR ("All");

	else if(node->isSuite ())
	{
		title = node->suite->getName ();
		if(title.endsWith ("Test"))
			title.remove (title.index ("Test"), 4);
	}
	else if(node->isTest ())
	{
		title.append (node->suite->getTestName (node->test));

		if(title.index ("test") == 0)
			title.remove (0, 4);
	}
	else
		title = CCLSTR ("Test");

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

tbool CCL_API TestTreeModel::getRootItem (ItemIndex& index)
{
	index = ItemIndex (ccl_as_unknown (rootNode));
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

tbool CCL_API TestTreeModel::canExpandItem (ItemIndexRef index)
{
	TestNode* node = unknown_cast<TestNode> (index.getObject ());
	if(!node)
		return false;

	return node->isRegistry () || node->isSuite ();
}

//////////////////////////////////////////////////////////////////////////////////////////////////

tbool CCL_API TestTreeModel::getSubItems (IUnknownList& items, ItemIndexRef index)
{
	TestNode* node = unknown_cast<TestNode> (index.getObject ());
	if(!node)
		return false;

	if(node->isRegistry ())
	{
		const auto& testSuites = node->registry->getTestSuites ();
		for(auto* suite : testSuites)
			items.add (ccl_as_unknown (NEW TestNode (suite)));
	}
	else if(node->isSuite ())
	{
		int count = node->suite->countTests ();
		for(int i = 0; i < count; i++)
			items.add (ccl_as_unknown (NEW TestNode (node->suite, i)));
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

tbool CCL_API TestTreeModel::onItemFocused (ItemIndexRef index)
{
	currentNode = unknown_cast<TestNode> (index.getObject ());
	return true;
}

//************************************************************************************************
// TestResultListModel
//************************************************************************************************

TestResultListModel::TestResultListModel (IProgressNotify* progressNotify)
: progressNotify (progressNotify),
  numTestsPlanned (0),
  numTestsProcessed (0),
  hasFailedTests (false)
{
	testResults.objectCleanup ();
}

//////////////////////////////////////////////////////////////////////////////////////////////////

int CCL_API TestResultListModel::countFlatItems ()
{
	return testResults.count ();
}

//////////////////////////////////////////////////////////////////////////////////////////////////

tbool CCL_API TestResultListModel::drawCell (ItemIndexRef index, int column, const DrawInfo& info)
{
	if(column == 0)
	{
		TestResult* testResult = static_cast<TestResult*> (testResults.at (index.getIndex ()));

		StringRef suiteName = testResult->getSuiteName ();
		StringRef testName = testResult->getTestName ();
		double duration = testResult->getDuration ();

		String message;
		message << suiteName << " - " << testName << " run in ";
		message.appendFloatValue (duration * 1000., 3);
		message << "ms";

		SolidBrush textBrush (testResult->hasFailed () ? Colors::kRed : Colors::kBlack);

		if(testResult->hasFailed ())
		{
			const Vector<AutoPtr<AssertionResult>>& assertionResults = testResult->getAssertionResults ();
			for(auto& assertion : assertionResults)
				if(assertion->hasFailed ())
					message << " --- " << assertion->getInfo ().expression;
		}

		Rect drawRect (info.rect);
		drawRect.left += 5;
		info.graphics.drawString (drawRect, message, info.style.font, textBrush, Alignment::kLeftCenter);
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void TestResultListModel::beginTestRun (int numTests, StringRef filter)
{
	if(progressNotify == nullptr)
		return;

	numTestsPlanned = numTests;
	numTestsProcessed = 0;

	progressNotify->beginProgress ();
	progressNotify->updateProgress ({0.});
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void TestResultListModel::endTestRun ()
{
	if(progressNotify == nullptr)
		return;

	progressNotify->updateProgress ({1.});
	progressNotify->endProgress ();
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void TestResultListModel::addResult (TestResult* testResult)
{
	testResult->retain ();
	if(testResult->hasFailed ())
		hasFailedTests = true;

	testResults.add (testResult);
	progressNotify->updateProgress ({++numTestsProcessed / (double)(numTestsPlanned)});
	signal (Message (kChanged));
}

//////////////////////////////////////////////////////////////////////////////////////////////////

bool TestResultListModel::allTestsPassed () const
{
	return hasFailedTests;
}
