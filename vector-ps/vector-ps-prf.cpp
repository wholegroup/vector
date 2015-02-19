/*
 * Copyright (C) 2015 Andrey Rychkov <wholegroup@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "stdafx.h"
#include "vector-ps-prf.h"
#include "suite.h"
#include "ui.h"

using namespace ::com::wholegroup::vector;
using namespace ::com::wholegroup::vector::ps;

namespace {

// Event handler flag
static bool notifierOn = false;

// Preferences
static double gColumnWidth;
static double gGutterWidth;
static bool   gExactPoints;
static int16  gVersionMajor;
static int16  gVersionMinor;

#ifdef __APPLE__
struct shmService {
	shmService()
	{
		boost::interprocess::shared_memory_object::remove(SHM_UUID);
		
		boost::interprocess::shared_memory_object shm(
			boost::interprocess::create_only,
			SHM_UUID,
			boost::interprocess::read_write);
			
		shm.truncate(sizeof(&getPreferences));
		
		boost::interprocess::mapped_region region(shm,
			::boost::interprocess::read_write);
			
		*((getPreferencesPtr *)region.get_address()) = getPreferences;
	}
	
	~shmService()
	{
		boost::interprocess::shared_memory_object::remove(SHM_UUID);
	}
} shmService;
#endif

/************************************************************************/
/* Init.                                                                */
/************************************************************************/
void init()
{
}

/************************************************************************/
/* Update the version.                                                  */
/************************************************************************/
void updateVersion(SPBasicSuite * const basicSuite)
{
	BOOST_ASSERT_MSG(nullptr != basicSuite, "The basic suite is null.");

	// Property suite
	PropertySuite propertySuite(CreatePropertySuite(basicSuite));

	if ((kSPNoError != propertySuite.getError()) || (nullptr == propertySuite))
	{
		return;
	}

	// Version
	intptr_t version = 0;

	propertySuite->getPropertyProc(kPhotoshopSignature, propVersion,
		0, (intptr_t*)&version, nullptr);

	int32 version32 = (int32)version;

	gVersionMajor = version32 >> 16;
	gVersionMinor = version32 & 0x0000FFFF;
}

/************************************************************************/
/* Update the preferences.                                              */
/************************************************************************/
void updatePreferences(SPBasicSuite * const basicSuite)
{
	BOOST_ASSERT_MSG(nullptr != basicSuite, "The basic suite is null.");

	// Action descriptor suite
	ActionDescriptorSuite actionDescriptorSuite(CreateActionDescriptorSuitePrev(basicSuite));

	if ((kSPNoError != actionDescriptorSuite.getError()) || (nullptr == actionDescriptorSuite))
	{
		return;
	}

	// Action control suite
	ActionControlSuite actionControlSuite(CreateActionControlSuitePrev(basicSuite));

	if ((kSPNoError != actionControlSuite.getError()) || (nullptr == actionControlSuite))
	{
		return;
	}

	// Action reference suite
	ActionReferenceSuite actionReferenceSuite(CreateActionReferenceSuitePrev(basicSuite));

	if ((kSPNoError != actionReferenceSuite.getError()) || (nullptr == actionReferenceSuite))
	{
		return;
	}

	OSErr error;

	// Get the properties 
	Auto_Ref reference(actionReferenceSuite);

	error = actionReferenceSuite->PutProperty(reference.get(), classProperty, keyUnitsPrefs);

	if (kSPNoError != error)
	{
		return;
	}

	error = actionReferenceSuite->PutEnumerated(reference.get(), classApplication, typeOrdinal, enumTarget);

	if (kSPNoError != error)
	{
		return;
	}

	Auto_Desc properties(actionDescriptorSuite, false);

	error = actionControlSuite->Get(&properties, reference.get());

	if ((kSPNoError != error) || (nullptr == properties.get()))
	{
		return;
	}

	// Get keyUnitsPrefs
	Boolean hasKey; 

	error = actionDescriptorSuite->HasKey(properties.get(), keyUnitsPrefs, &hasKey);

	if ((kSPNoError != error) || !hasKey)
	{
		return;
	}

	DescriptorClassID classId;
	Auto_Desc unitsPrefs(actionDescriptorSuite, false);

	error = actionDescriptorSuite->GetObject(properties.get(), keyUnitsPrefs, &classId, &unitsPrefs);

	if ((kSPNoError != error) || (classUnitsPrefs != classId))
	{
		return;
	}

	// Get the column width
	error = actionDescriptorSuite->HasKey(unitsPrefs.get(), keyColumnWidth, &hasKey);

	if ((kSPNoError != error) || !hasKey)
	{
		return;
	}

	DescriptorUnitID unitId;
	double columnWidth;

	error = actionDescriptorSuite->GetUnitFloat(unitsPrefs.get(), keyColumnWidth, &unitId, &columnWidth);

	if (kSPNoError != error)
	{
		return;
	}

	// Get the gutter width
	error = actionDescriptorSuite->HasKey(unitsPrefs.get(), keyGutterWidth, &hasKey);

	if ((kSPNoError != error) || !hasKey)
	{
		return;
	}

	double gutterWidth;

	error = actionDescriptorSuite->GetUnitFloat(unitsPrefs.get(), keyGutterWidth, &unitId, &gutterWidth);

	if (kSPNoError != error)
	{
		return;
	}

	// Get the exact points flag
	error = actionDescriptorSuite->HasKey(unitsPrefs.get(), keyExactPoints, &hasKey);

	if ((kSPNoError != error) || !hasKey)
	{
		return;
	}

	Boolean exactPoints;

	error = actionDescriptorSuite->GetBoolean(unitsPrefs.get(), keyExactPoints, &exactPoints);

	if (kSPNoError != error)
	{
		return;
	}

	gColumnWidth = columnWidth;
	gGutterWidth = gutterWidth;
	gExactPoints = exactPoints ? true : false;
}

/************************************************************************/
/* Event handler.                                                       */
/************************************************************************/
void eventSetHandler(DescriptorEventID eventId, PIActionDescriptor actionDescriptor, 
	PIDialogRecordOptions /*options*/, void * data)
{
	// Basic suite
	SPBasicSuite * const basicSuite = reinterpret_cast<SPBasicSuite * const>(data);

	if (nullptr == basicSuite)
	{
		return;
	}

	// Handling only eventSet
	if ((eventSet != eventId) || (nullptr == actionDescriptor))
	{
		return;
	}

	// Action descriptor suite
	ActionDescriptorSuite actionDescriptorSuite(CreateActionDescriptorSuite(basicSuite));

	if ((kSPNoError != actionDescriptorSuite.getError()) || (nullptr == actionDescriptorSuite))
	{
		return;
	}

	// Get count actions
	uint32 actionCount;
	OSErr  error;

	error = actionDescriptorSuite->GetCount(actionDescriptor, &actionCount);

	if (kSPNoError != error)
	{
		return;
	}

	// Check every action
	for (uint32 n = 0; n < actionCount; ++n)
	{
		// Check key
		DescriptorKeyID actionKeyId;

		error = actionDescriptorSuite->GetKey(actionDescriptor, n, &actionKeyId);

		if ((kSPNoError != error) || (keyTo != actionKeyId))
		{
			continue;
		}

		// Check type
		DescriptorTypeID actionKeyTypeId;

		error = actionDescriptorSuite->GetType(actionDescriptor, actionKeyId, &actionKeyTypeId);

		if ((kSPNoError != error) || (typeObject != actionKeyTypeId))
		{
			continue;
		}

		// Check class
		DescriptorClassID  objClassID;
		PIActionDescriptor objDesc = nullptr;

		error = actionDescriptorSuite->GetObject(actionDescriptor, actionKeyId, &objClassID, &objDesc);

		if ((kSPNoError != error) || (classUnitsPrefs != objClassID))
		{
			continue;
		}

		updatePreferences(basicSuite);

		break;
	}

	return;
}

/************************************************************************/
/* Startup.                                                             */
/************************************************************************/
SPErr startup(SPPluginRef plugin, SPBasicSuite * basicSuite)
{
	// Action control suite
	ActionControlSuite actionControlSuite(CreateActionControlSuite(basicSuite));

	if ((kSPNoError != actionControlSuite.getError()) || (nullptr == actionControlSuite))
	{
		return kSPBadParameterError;
	}

	// Add event handler
	SPErr error = kSPNoError;

	if (!notifierOn)
	{
		error = actionControlSuite->AddNotify(
			plugin, eventSet, eventSetHandler, basicSuite);

		if (kSPNoError == error)
		{
			notifierOn = true;
		}

		updateVersion(basicSuite);

		updatePreferences(basicSuite);
	}
	
	boost::thread warmingUp([](){
		// код для прогрева JVM
		init();
	});
	
	return error;
}

/************************************************************************/
/* Shutdown.                                                            */
/************************************************************************/
SPErr shutdown(SPPluginRef plugin, SPBasicSuite * basicSuite)
{
	// Action control suite
	ActionControlSuite actionControlSuite(CreateActionControlSuite(basicSuite));

	if ((kSPNoError != actionControlSuite.getError()) || (nullptr == actionControlSuite))
	{
		return kSPBadParameterError;
	}

	// Remove the event handler
	SPErr error = kSPNoError;

	if (notifierOn)
	{
		error = actionControlSuite->RemoveNotify(plugin, eventSet);
	}

	return error;
}

} // namespace

/************************************************************************/
/* Entry point.                                                         */
/************************************************************************/
DLLExport SPAPI SPErr BOOST_SYMBOL_VISIBLE helperEntry(
	const char* caller, const char* selector, void* message)
{
	SPMessageData* basicMessage = reinterpret_cast<SPMessageData*>(message);

	if ((nullptr == basicMessage) || (nullptr == basicMessage->basic))
	{
		return kSPBadParameterError;
	}

	SPErr error = kSPNoError;

	if (basicMessage->basic->IsEqual(caller, kSPInterfaceCaller))
	{
		if (basicMessage->basic->IsEqual(selector, kSPInterfaceAboutSelector))
		{
#ifdef WIN32
			Ui::doAbout();
#endif
		}

		else if (basicMessage->basic->IsEqual(selector, kSPInterfaceStartupSelector))
		{
			error = startup(basicMessage->self, basicMessage->basic);
		}

		else if (basicMessage->basic->IsEqual(selector, kSPInterfaceShutdownSelector))
		{
			error = shutdown(basicMessage->self, basicMessage->basic);
		}
	}

	return error;
}

/************************************************************************/
/* Get the preferences.                                                 */
/************************************************************************/
bool getPreferences(double & columnWidth, double & gutterWidth, double & pointSize)
{
	columnWidth = gColumnWidth;
	gutterWidth = gGutterWidth;
	pointSize   = gExactPoints ? 72.2l : 72.0l;

	return true;
}

/************************************************************************/
/* Get the version (major).                                             */
/************************************************************************/
int16 getVersionMajor()
{
	return gVersionMajor;
}
