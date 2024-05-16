//
//  DisplayList.cpp
//
//  This file is part of the ObjLibrary, by Richard Hamilton,
//    which is copyright Hamilton 2009-2021.
// 
//  You may use these files for any purpose as long as you do
//    not explicitly claim them as your own work or object to
//    other people using them.
// 
//  If you are distributing the source files, you must not
//    remove this notice.  If you are only distributing compiled
//    code, no credit is required.
// 
//  A (theoretically) up-to-date version of the ObjLibrary can
//    be found at:
//  http://infiniplix.ca/resources/obj_library/
//

#include <cassert>
#include <cstddef>	// for NULL

#include "../GetGlut.h"
#include "DisplayList.h"

using namespace ObjLibrary;
namespace
{
	bool g_is_disabled_for_exit = false;
}



bool DisplayList :: isGlutInitialized ()
{
#ifdef GLUT_INIT_STATE
	return glutGet(GLUT_INIT_STATE) == 1;
#else
	return true;
#endif
}

bool DisplayList :: isDisabledForExit ()
{
	return g_is_disabled_for_exit;
}

void DisplayList :: disableAllForExit ()
{
	assert(!isDisabledForExit());

	g_is_disabled_for_exit = true;

	assert(isDisabledForExit());
}



DisplayList :: DisplayList ()
{
	mp_data = NULL;
}

DisplayList :: DisplayList (const DisplayList& original)
{
	assert(!original.isPartial());

	mp_data = NULL;
	copy(original);
}

DisplayList :: ~DisplayList ()
{
	makeEmpty();
}

DisplayList& DisplayList :: operator= (const DisplayList& original)
{
	assert(!original.isPartial());

	if(&original != this)
	{
		makeEmpty();
		copy(original);
	}

	return *this;
}



unsigned int DisplayList :: getState() const
{
	if(mp_data == NULL)
		return EMPTY;
	else if(mp_data->m_usages == 0)
		return PARTIAL;
	else
		return READY;
}

bool DisplayList :: isEmpty() const
{
	return (mp_data == NULL);
}

bool DisplayList :: isPartial() const
{
	return (mp_data != NULL && mp_data->m_usages == 0);
}

bool DisplayList :: isReady() const
{
	return (mp_data != NULL && mp_data->m_usages > 0);
}

void DisplayList :: draw () const
{
	assert(isGlutInitialized());
	assert(!isDisabledForExit());
	assert(isReady());

	glCallList(mp_data->m_list_id);
}



void DisplayList :: makeEmpty ()
{
	switch(getState())
	{
	case PARTIAL:
		end();
		break;
	case READY:
		assert(mp_data->m_usages > 0);
		mp_data->m_usages--;
		if(mp_data->m_usages == 0)
		{
			if(!isDisabledForExit())
				glDeleteLists(mp_data->m_list_id, 1);
			delete mp_data;
		}
		mp_data = NULL;
		break;
	}

	assert(isEmpty());
}

void DisplayList :: begin ()
{
	assert(isGlutInitialized());
	assert(!isDisabledForExit());
	assert(!isPartial());

	if(getState() == READY)
		makeEmpty();

	assert(isEmpty());

	mp_data = new InnerData();
	mp_data->m_usages = 0;
	mp_data->m_list_id = glGenLists(1);

	glNewList(mp_data->m_list_id, GL_COMPILE);

	assert(getState() == PARTIAL);
}

void DisplayList :: end ()
{
	assert(isGlutInitialized());
	assert(!isDisabledForExit());
	assert(isPartial());

	glEndList();

	assert(mp_data->m_usages == 0);
	mp_data->m_usages = 1;

	assert(isReady());
}



void DisplayList :: copy (const DisplayList& original)
{
	assert(isEmpty());
	assert(!original.isPartial());

	mp_data = original.mp_data;

	if(mp_data != NULL)
		mp_data->m_usages++;
}
