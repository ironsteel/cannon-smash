/**********************************************************************
 *	new_exception.cpp
 *	$Id: new_exception.cpp,v 1.2 2001/08/18 16:26:18 yotsuya Exp $
 *
 *	Change "operator new" behavior to throw bad_alloc exception
 *	on memory error. This is for Microsoft Visual C++ v5.0 or higher.
 *	It must be compiled with -GX option. (of course)
 *
 *	This code is taken from msdn.
 *	"PRB: Operator New Doesn't Throw bad_alloc Exception on Failure"
 **********************************************************************/

#ifdef _MSC_VER
#if _MSC_VER >= 1000
#include <new>
#include <new.h>

// Force my_global_new_handler_object be initialized
// prior to other global objects.
#pragma warning(disable: 4073)
#pragma init_seg(lib)

namespace {
    char __rcsid__[] =
	"@(#) $Id: new_exception.cpp,v 1.2 2001/08/18 16:26:18 yotsuya Exp $";

    int my_new_handler(size_t) throw(std::bad_alloc) {
	throw std::bad_alloc();
	return 0;
    }
    
    static struct my_new_handler_obj {
	int (*old_new_handler)(size_t);
	int old_new_mode;
	my_new_handler_obj() {
	    old_new_mode = _set_new_mode(1);  // malloc will throw bad_alloc
	    old_new_handler = _set_new_handler(my_new_handler);
	}
	~my_new_handler_obj() {
	    _set_new_handler(old_new_handler);
	    _set_new_mode(old_new_mode);
	}
    } my_global_new_handler_object;
}

void* __cdecl operator new(size_t cb, const std::nothrow_t &) throw()
{
    void *p;
    try {
	p = operator new(cb);
    }
    catch (std::bad_alloc) {
	p = 0;
    }
    return p;
}

#endif /* MSC_VER >= 1000 (VC 5 or later) */
#endif /* MSC_VER (Microsoft C++ dependent) */
