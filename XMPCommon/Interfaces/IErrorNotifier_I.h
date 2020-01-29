#ifndef IErrorNotifier_I_h__
#define IErrorNotifier_I_h__ 1

// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. If you have received this file from a source other 
// than Adobe, then your use, modification, or distribution of it requires the prior written permission
// of Adobe.
// =================================================================================================

#include "XMPCommon/XMPCommonFwdDeclarations_I.h"
#include "XMPCommon/Interfaces/IErrorNotifier.h"

namespace XMP_COMPONENT_INT_NAMESPACE {

	//!
	//! \brief Internal interface that represents an interface to be implemented by library
	//! in case it is interested in getting notifications with respect to errors/warnings encountered
	//! within the library.
	//! \details In case library is interested in error notifications it can implement this interface
	//! and register the same with the #IConfigurationManager. For every warning or error
	//! encountered the NotifyError function will be called by the library. In case of warnings ( indicated
	//! by the severity of the error ) the implementation has the option to continue ignoring the warning by
	//! returning true else he can return false and the warning will be thrown aborting the current operation.
	//!
	class IErrorNotifier_I
		: public virtual IErrorNotifier
	{
	public:

		//!
		//! Get the current error notifier.
		//!
		static pIErrorNotifier GetErrorNotifier();

		//!
		//! Set the current error notifier.
		//!
		static pIErrorNotifier SetErrorNotifier( pIErrorNotifier errorNotifier );

	protected:
		~IErrorNotifier_I() {}

	#ifdef FRIEND_CLASS_DECLARATION
		REQ_FRIEND_CLASS_DECLARATION();
	#endif
		REQ_FRIEND_CLASS_DECLARATION();
	};
}

#endif // IErrorNotifier_I_h__
