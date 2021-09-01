// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#define IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED 1
	#include "XMPCore/ImplHeaders/NameSpacePrefixMapImpl.h"
#undef IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED

#include "XMPCommon/Interfaces/IUTF8String_I.h"
#include "XMPCommon/Interfaces/IError_I.h"
#include "XMPCommon/Utilities/AutoSharedLock.h"
#include "XMPCommon/Utilities/TSmartPointers_I.h"

#include "source/XMP_LibUtils.hpp"
#include "source/UnicodeInlines.incl_cpp"

#include "XMPCore/XMPCoreErrorCodes.h"
#include <cstring>

namespace AdobeXMPCore_Int {

	static sizet ActualLength( const char * parameter, sizet length ) {
		if ( length == npos && parameter ) return strlen( parameter );
		return length;
	}

	static bool VerifyParameters( bool prefixPresent, bool nameSpacePresent, const spcIUTF8String & prefix, const spcIUTF8String & nameSpace ) {
		bool allOk = true;
		if ( prefixPresent ) {
			if ( !prefix ) {
				NOTIFY_WARNING( IError_v1::kEDGeneral, kGECParametersNotAsExpected, "prefix is NULL", false, false );
				allOk = false;
			} else {
				if ( prefix->size() == 0) {
					NOTIFY_WARNING( IError_v1::kEDGeneral, kGECParametersNotAsExpected, "prefixLength is 0", false, false );
					allOk = false;
				}
			}
		}

		if ( nameSpacePresent ) {
			if ( !nameSpace ) {
				if ( !nameSpace ) {
					NOTIFY_WARNING( IError_v1::kEDGeneral, kGECParametersNotAsExpected, "nameSpace is NULL", false, false );
					allOk = false;
				} else {
					if ( nameSpace->size() == 0 ) {
						NOTIFY_WARNING( IError_v1::kEDGeneral, kGECParametersNotAsExpected, "nameSpaceLength is 0", false, false );
						allOk = false;
					}
				}
			}
		}

		return allOk;
	}

	static bool VerifyParameters( bool prefixPresent, bool nameSpacePresent, const char * prefix, const char * nameSpace, sizet prefixLength, sizet nameSpaceLength ) {
		bool allOk = true;
		if ( prefixPresent ) {
			if ( !prefix ) {
				NOTIFY_WARNING( IError_v1::kEDGeneral, kGECParametersNotAsExpected, "prefix is NULL", true, ( void * ) prefix );
				allOk = false;
			} else {
				if ( prefixLength == npos ) prefixLength = strlen( prefix );
				if ( prefixLength == 0 ) {
					NOTIFY_WARNING( IError_v1::kEDGeneral, kGECParametersNotAsExpected, "prefixLength is 0", true, prefixLength );
					allOk = false;
				}
			}
		}

		if ( nameSpacePresent ) {
			if ( !nameSpace ) {
				NOTIFY_WARNING( IError_v1::kEDGeneral, kGECParametersNotAsExpected, "nameSpace is NULL", true, ( void * ) nameSpace );
				allOk = false;
			} else {
				if ( nameSpaceLength == npos ) nameSpaceLength = strlen( nameSpace );
				if ( nameSpaceLength == 0 ) {
					NOTIFY_WARNING( IError_v1::kEDGeneral, kGECParametersNotAsExpected, "nameSpaceLength is 0", true, nameSpaceLength );
					allOk = false;
				}
			}
		}

		return allOk;
	}

	NameSpacePrefixMapImpl::NameSpacePrefixMapImpl() {}

	// All virtual functions
	bool APICALL NameSpacePrefixMapImpl::Insert( const char * prefix, sizet prefixLength, const char * nameSpace, sizet nameSpaceLength ) {
		if ( VerifyParameters( true, true, prefix, nameSpace, prefixLength, nameSpaceLength ) ) {
			prefixLength = ActualLength( prefix, prefixLength );
			nameSpaceLength = ActualLength( nameSpace, nameSpaceLength );
			try {
				VerifySimpleXMLName( prefix, prefix + prefixLength );
			} catch ( ... ) {
				NOTIFY_ERROR( IError_v1::kEDDataModel, kDMECBadXPath, "prefix is not a valid XML Name", IError_v1::kESOperationFatal, true, prefix );
				return false;
			}
			spcIUTF8String prefixStr = IUTF8String_I::CreateUTF8String( prefix, prefixLength );
			spcIUTF8String nameSpaceStr = IUTF8String_I::CreateUTF8String( nameSpace, nameSpaceLength );

			AutoSharedLock( mSharedMutex, true );

			// remove old entries
			if ( mPrefixToNameSpaceMap.find( prefixStr ) != mPrefixToNameSpaceMap.end() ) {
				spcIUTF8String oldValue = mPrefixToNameSpaceMap[ prefixStr ];
				mPrefixToNameSpaceMap.erase( prefixStr );
				mNameSpaceToPrefixMap.erase( oldValue );
			}

			if ( mNameSpaceToPrefixMap.find( nameSpaceStr ) != mNameSpaceToPrefixMap.end() ) {
				spcIUTF8String oldValue = mNameSpaceToPrefixMap[ nameSpaceStr ];
				mPrefixToNameSpaceMap.erase( oldValue );
				mNameSpaceToPrefixMap.erase( nameSpaceStr );
			}

			mNameSpaceToPrefixMap[ nameSpaceStr ] = prefixStr;
			mPrefixToNameSpaceMap[ prefixStr ] = nameSpaceStr;
			return true;
		}
		return false;
	}

	bool APICALL NameSpacePrefixMapImpl::RemovePrefix( const char * prefix, sizet prefixLength ) {
		if ( VerifyParameters( true, false, prefix, NULL, prefixLength, 0 ) ) {
			spcIUTF8String prefixStr = IUTF8String_I::CreateUTF8String( prefix, prefixLength );
			AutoSharedLock lock( mSharedMutex, true );
			if ( mPrefixToNameSpaceMap.find( prefixStr ) != mPrefixToNameSpaceMap.end() ) {
				spcIUTF8String nameSpaceStr = mPrefixToNameSpaceMap[ prefixStr ];
				mPrefixToNameSpaceMap.erase( prefixStr );
				mNameSpaceToPrefixMap.erase( nameSpaceStr );
				return true;
			}
		}
		return false;
	}

	bool APICALL NameSpacePrefixMapImpl::RemoveNameSpace( const char * nameSpace, sizet nameSpaceLength ) {
		if ( VerifyParameters( false, true, NULL, nameSpace, 0, nameSpaceLength ) ) {
			spcIUTF8String nameSpaceStr = IUTF8String_I::CreateUTF8String( nameSpace, nameSpaceLength );
			AutoSharedLock lock( mSharedMutex, true );
			if ( mNameSpaceToPrefixMap.find( nameSpaceStr ) != mNameSpaceToPrefixMap.end() ) {
				spcIUTF8String prefixStr = mNameSpaceToPrefixMap[ nameSpaceStr ];
				mPrefixToNameSpaceMap.erase( prefixStr );
				mNameSpaceToPrefixMap.erase( nameSpaceStr );
				return true;
			}
		}
		return false;
	}

	bool APICALL NameSpacePrefixMapImpl::IsPrefixPresent( const char * prefix, sizet prefixLength ) const {
		if ( VerifyParameters( true, false, prefix, NULL, prefixLength, 0 ) ) {
			spcIUTF8String prefixStr = IUTF8String_I::CreateUTF8String( prefix, prefixLength );
			AutoSharedLock lock( mSharedMutex, true );
			return mPrefixToNameSpaceMap.find( prefixStr ) != mPrefixToNameSpaceMap.end();
		}
		return false;
	}

	bool APICALL NameSpacePrefixMapImpl::IsNameSpacePresent( const char * nameSpace, sizet nameSpaceLength ) const {
		if ( VerifyParameters( false, true, NULL, nameSpace, 0, nameSpaceLength ) ) {
			spcIUTF8String nameSpaceStr = IUTF8String_I::CreateUTF8String( nameSpace, nameSpaceLength );
			AutoSharedLock lock( mSharedMutex, true );
			return mNameSpaceToPrefixMap.find( nameSpaceStr ) != mNameSpaceToPrefixMap.end();
		}
		return false;
	}

	spcIUTF8String APICALL NameSpacePrefixMapImpl::GetNameSpace( const char * prefix, sizet prefixLength ) const {
		if ( VerifyParameters( true, false, prefix, NULL, prefixLength, 0 ) ) {
			spcIUTF8String prefixStr = IUTF8String_I::CreateUTF8String( prefix, prefixLength );
			AutoSharedLock lock( mSharedMutex, true );
			auto it = mPrefixToNameSpaceMap.find( prefixStr );
			if ( it != mPrefixToNameSpaceMap.end() )
				return it->second;
		}
		return spcIUTF8String();
	}

	spcIUTF8String APICALL NameSpacePrefixMapImpl::GetNameSpace( const spcIUTF8String & prefix ) const {
		if ( VerifyParameters( true, false, prefix, spcIUTF8String() ) ) {
			AutoSharedLock lock( mSharedMutex, true );
			auto it = mPrefixToNameSpaceMap.find( prefix );
			if ( it != mPrefixToNameSpaceMap.end() )
				return it->second;
		}
		return spcIUTF8String();
	}

	spcIUTF8String APICALL NameSpacePrefixMapImpl::GetPrefix( const char * nameSpace, sizet nameSpaceLength ) const {
		if ( VerifyParameters( false, true, NULL, nameSpace, 0, nameSpaceLength ) ) {
			spcIUTF8String nameSpaceStr = IUTF8String_I::CreateUTF8String( nameSpace, nameSpaceLength );
			AutoSharedLock lock( mSharedMutex, true );
			auto it = mNameSpaceToPrefixMap.find( nameSpaceStr );
			if ( it != mNameSpaceToPrefixMap.end() )
				return it->second;
		}
		return spcIUTF8String();
	}

	spcIUTF8String APICALL NameSpacePrefixMapImpl::GetPrefix( const spcIUTF8String & nameSpace ) const {
		if ( VerifyParameters( false, true, spcIUTF8String(), nameSpace ) ) {
			AutoSharedLock lock( mSharedMutex, true );
			auto it = mNameSpaceToPrefixMap.find( nameSpace );
			if ( it != mNameSpaceToPrefixMap.end() )
				return it->second;
		}
		return spcIUTF8String();
	}

	sizet APICALL NameSpacePrefixMapImpl::Size() const __NOTHROW__ {
		AutoSharedLock lock( mSharedMutex, false );
		return mNameSpaceToPrefixMap.size();
	}

	void APICALL NameSpacePrefixMapImpl::Clear() __NOTHROW__ {
		AutoSharedLock lock( mSharedMutex, true );
		mNameSpaceToPrefixMap.clear();
		mPrefixToNameSpaceMap.clear();
	}

	spINameSpacePrefixMap APICALL NameSpacePrefixMapImpl::Clone() const {
		NameSpacePrefixMapImpl * ptr = new NameSpacePrefixMapImpl();
		if ( !ptr )
			NOTIFY_ERROR( IError_v1::kEDMemoryManagement, kMMECAllocationFailure, "Unable to allocate required memory", IError_v1::kESProcessFatal, false, false );
		AutoSharedLock lock( mSharedMutex, false );

		// Replacing the shallow copy with deep copy
		auto iterator = mPrefixToNameSpaceMap.begin();
		while ( iterator != mPrefixToNameSpaceMap.end() ) {
			ptr->mPrefixToNameSpaceMap[ iterator->first ] = iterator->second;
			iterator++;
		}

		iterator = mNameSpaceToPrefixMap.begin();
		while ( iterator != mNameSpaceToPrefixMap.end() ) {
			ptr->mNameSpaceToPrefixMap[ iterator->first ] = iterator->second;
			iterator++;
		}

		return MakeUncheckedSharedPointer( ptr, __FILE__, __LINE__ );
	}

	void APICALL NameSpacePrefixMapImpl::Merge( const spcINameSpacePrefixMap & otherMap ) {
		if ( otherMap ) {
			const NameSpacePrefixMapImpl * otherMapPtr = dynamic_cast< const NameSpacePrefixMapImpl * >( otherMap.get() );
			for (auto it = otherMapPtr->mPrefixToNameSpaceMap.begin(), itEnd = otherMapPtr->mPrefixToNameSpaceMap.end(); it != itEnd; ++it) {
				Insert( it->first->c_str(), it->first->size(), it->second->c_str(), it->second->size() );
			}
		}
	}

	// All static functions of _I class.

	static spINameSpacePrefixMap ManageDefaultNameSpacePrefixMap( bool destroy = false ) {
		static spINameSpacePrefixMap sDefaultNameSpacePrefixMap;
		if ( destroy ) {
			sDefaultNameSpacePrefixMap.reset();
		} else {
			if ( !sDefaultNameSpacePrefixMap ) {
				sDefaultNameSpacePrefixMap = INameSpacePrefixMap_I::CreateNameSpacePrefixMap();
				sDefaultNameSpacePrefixMap->EnableThreadSafety();
			}
		}
		return sDefaultNameSpacePrefixMap;
	}

	void INameSpacePrefixMap_I::CreateDefaultNameSpacePrefixMap() {
		ManageDefaultNameSpacePrefixMap( false );
	}

	void INameSpacePrefixMap_I::DestroyDefaultNameSapcePrefixMap() {
		ManageDefaultNameSpacePrefixMap( true );
	}

	bool INameSpacePrefixMap_I::InsertInDefaultNameSpacePrefixMap( const char * prefix, sizet prefixLength, const char * nameSpace, sizet nameSpaceLength ) {
		return ManageDefaultNameSpacePrefixMap( false )->Insert( prefix, prefixLength, nameSpace, nameSpaceLength );
	}
}

#if BUILDING_XMPCORE_LIB || SOURCE_COMPILING_XMPCORE_LIB

namespace AdobeXMPCore {
	using AdobeXMPCore_Int::MakeUncheckedSharedPointer;

	spINameSpacePrefixMap INameSpacePrefixMap_v1::CreateNameSpacePrefixMap() {
		return MakeUncheckedSharedPointer( new AdobeXMPCore_Int::NameSpacePrefixMapImpl(), __FILE__, __LINE__, true );
	}

	spcINameSpacePrefixMap INameSpacePrefixMap_v1::GetDefaultNameSpacePrefixMap() {
		return AdobeXMPCore_Int::ManageDefaultNameSpacePrefixMap( false );
	}

	spINameSpacePrefixMap INameSpacePrefixMap_v1::MakeShared( pINameSpacePrefixMap_base ptr ) {
		if ( !ptr ) return spINameSpacePrefixMap();
		pINameSpacePrefixMap p = INameSpacePrefixMap::GetInterfaceVersion() > 1 ? ptr->GetInterfacePointer< INameSpacePrefixMap >() : ptr;
		return MakeUncheckedSharedPointer( p, __FILE__, __LINE__, false );
	}
}

#endif  // BUILDING_XMPCORE_LIB || SOURCE_COMPILING_XMPCORE_LIB

