// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. If you have received this file from a source other 
// than Adobe, then your use, modification, or distribution of it requires the prior written permission
// of Adobe.
// =================================================================================================


#include "XMPCore/Interfaces/INodeIterator_I.h"
#include "XMPCommon/Utilities/TWrapperFunctions_I.h"
#include "XMPCommon/Interfaces/IError_I.h"
#include "XMPCommon/Utilities/TSmartPointers_I.h"

namespace AdobeXMPCore_Int {

	pvoid APICALL INodeIterator_I::GetInterfacePointer(uint64 interfaceID, uint32 interfaceVersion) {
		return GetInterfacePointerInternal(interfaceID, interfaceVersion, true);
	}

	pvoid APICALL INodeIterator_I::getInterfacePointer(uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error) __NOTHROW__{
		return CallUnSafeFunction< INodeIterator_I, pvoid, pvoid, uint64, uint32 >(
		error, this, NULL, &INodeIterator_I::GetInterfacePointer, __FILE__, __LINE__, interfaceID, interfaceVersion);
	}

	pvoid APICALL INodeIterator_I::GetInterfacePointerInternal(uint64 interfaceID, uint32 interfaceVersion, bool isTopLevel) {
		if (interfaceID == kINodeIteratorID) {
			switch (interfaceVersion) {
			case 1:
				return static_cast< INodeIterator_v1 * >(this);
				break;

			case kInternalInterfaceVersionNumber:
				return this;
				break;

			default:
				throw IError_I::CreateInterfaceVersionNotAvailableError(
					IError_v1::kESOperationFatal, interfaceID, interfaceVersion, __FILE__, __LINE__);
				break;
			}
		}
		if (isTopLevel)
			throw IError_I::CreateInterfaceNotAvailableError(
			IError_v1::kESOperationFatal, kINodeIteratorID, interfaceID, __FILE__, __LINE__);
		return NULL;
	}


	pINode_base APICALL INodeIterator_I::getNode( pcIError_base & error ) __NOTHROW__ {
		return CallUnSafeFunctionReturningSharedPointer< INodeIterator_v1, pINode_base, INode >(
			error, this, &INodeIterator_v1::GetNode, __FILE__, __LINE__ );
	}

	uint32 APICALL INodeIterator_I::getNodeType( pcIError_base & error ) const __NOTHROW__ {
		return CallConstUnSafeFunction< INodeIterator_v1, uint32, INode_v1::eNodeType >(
			error, this, 0, &INodeIterator_v1::GetNodeType, __FILE__, __LINE__ );
	}

	pINodeIterator_base APICALL INodeIterator_I::next( pcIError_base & error ) __NOTHROW__ {
		return CallUnSafeFunctionReturningSharedPointer< INodeIterator_v1, pINodeIterator, INodeIterator >(
			error, this, &INodeIterator_v1::Next, __FILE__, __LINE__ );
	}

}

#if BUILDING_XMPCORE_LIB || SOURCE_COMPILING_XMPCORE_LIB
namespace AdobeXMPCore {
	using namespace AdobeXMPCore_Int;
	spINodeIterator INodeIterator_v1::MakeShared( pINodeIterator_base ptr ) {
		if ( !ptr ) return spINodeIterator();
		pINodeIterator p = INodeIterator::GetInterfaceVersion() > 1 ? ptr->GetInterfacePointer< INodeIterator >() : ptr;
		return MakeUncheckedSharedPointer( p, __FILE__, __LINE__, false );
	}
}
#endif  // BUILDING_XMPCORE_LIB || SOURCE_COMPILING_XMPCORE_LIB

