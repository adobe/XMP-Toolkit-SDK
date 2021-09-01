// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

namespace AdobeXMPCore {
	class IDOMSerializerProxy;
}

#define FRIEND_CLASS_DECLARATION() friend class AdobeXMPCore::IDOMSerializerProxy;

#include "XMPCore/Interfaces/IDOMSerializer.h"

#if !BUILDING_XMPCORE_LIB && !SOURCE_COMPILING_XMPCORE_LIB

#include "XMPCommon/Utilities/TWrapperFunctions.h"
#include "XMPCommon/Interfaces/IUTF8String.h"
#include "XMPCore/Interfaces/INode.h"
#include "XMPCore/Interfaces/INameSpacePrefixMap.h"

#include <assert.h>

namespace AdobeXMPCore {

#if XMP_WinBuild
	#pragma warning( push )
	#pragma warning( disable : 4250 )
#endif

	class IDOMSerializerProxy
		: public virtual IDOMSerializer
		, public virtual IConfigurableProxy
	{
	private:
		pIDOMSerializer mRawPtr;

	public:
		IDOMSerializerProxy( pIDOMSerializer ptr )
			: IConfigurableProxy( ptr )
			, mRawPtr( ptr )
		{
			mRawPtr->Acquire();
		}

		~IDOMSerializerProxy() __NOTHROW__ { mRawPtr->Release();  }

		pIDOMSerializer APICALL GetActualIDOMSerializer() __NOTHROW__ { return mRawPtr; }

		void APICALL Acquire() const __NOTHROW__ { assert( false ); }

		void APICALL Release() const __NOTHROW__ { assert( false ); }

		AdobeXMPCommon_Int::pISharedObject_I APICALL GetISharedObject_I() __NOTHROW__ {
			return mRawPtr->GetISharedObject_I();
		}

		AdobeXMPCore_Int::pIDOMSerializer_I APICALL GetIDOMSerializer_I() __NOTHROW__ {
			return mRawPtr->GetIDOMSerializer_I();
		}

		pvoid APICALL GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion ) {
			return CallSafeFunction< IVersionable, pvoid, pvoid, uint64, uint32 >(
				mRawPtr, &IVersionable::getInterfacePointer, interfaceID, interfaceVersion );
		}

		pvoid APICALL getInterfacePointer( uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error ) __NOTHROW__ {
			assert( false );
			return mRawPtr->getInterfacePointer( interfaceID, interfaceVersion, error );
		}

		virtual spIDOMSerializer APICALL Clone() const {
			return CallConstSafeFunctionReturningPointer< IDOMSerializer, pIDOMSerializer_base, IDOMSerializer >(
				mRawPtr, &IDOMSerializer_v1::clone );
		}

		virtual spIUTF8String APICALL Serialize( const spINode & node, const spcINameSpacePrefixMap & map ) {
			return CallSafeFunctionReturningPointer< IDOMSerializer, pIUTF8String_base, IUTF8String, pINode_base, pcINameSpacePrefixMap_base >(
				mRawPtr, &IDOMSerializer_v1::serialize, node ? node->GetActualINode() : NULL , map ? map->GetActualINameSpacePrefixMap() : NULL );
		}

		virtual pIDOMSerializer_base APICALL clone( pcIError_base & error ) const __NOTHROW__ {
			assert( false );
			return mRawPtr->clone( error );
		}

		virtual pIUTF8String_base APICALL serialize( pINode_base node, pcINameSpacePrefixMap_base map, pcIError_base & error ) __NOTHROW__ {
			assert( false );
			return mRawPtr->serialize( node, map, error );
		}

	};

#if XMP_WinBuild
	#pragma warning( pop )
#endif

	spIDOMSerializer IDOMSerializer_v1::MakeShared( pIDOMSerializer_base ptr ) {
		if ( !ptr ) return spIDOMSerializer();
		pIDOMSerializer p = ptr->GetInterfacePointer< IDOMSerializer >();
		return shared_ptr< IDOMSerializer >( new IDOMSerializerProxy( p ) );
	}

}

#endif  // !BUILDING_XMPCORE_LIB
