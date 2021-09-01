#ifndef TWrapperFunctions2_h__
#define TWrapperFunctions2_h__ 1

// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

//! \cond XMP_INTERNAL_DOCUMENTATION
namespace AdobeXMPCommon {
	template < typename className >
	void CallSafeFunctionReturningVoid( className * ptr,
		void ( APICALL className::*Func )( pcIError_base & ) )
	{
		pcIError_base error( NULL );
		( ptr->*Func )( error );
		if ( error ) throw IError::MakeShared( error );
	}

	template< typename className, typename t1 >
	void CallSafeFunctionReturningVoid( className * ptr,
		void ( APICALL className::*Func )( t1, pcIError_base & ), t1 v1 )
	{
		pcIError_base error( NULL );
		( ptr->*Func )( v1, error );
		if ( error ) throw IError::MakeShared( error );
	}

	template< typename className, typename t1, typename t2 >
	void CallSafeFunctionReturningVoid( className * ptr,
		void ( APICALL className::*Func )( t1, t2, pcIError_base & ), t1 v1, t2 v2 )
	{
		pcIError_base error( NULL );
		( ptr->*Func )( v1, v2, error );
		if ( error ) throw IError::MakeShared( error );
	}

	template< typename className, typename t1, typename t2, typename t3 >
	void CallSafeFunctionReturningVoid( className * ptr,
		void ( APICALL className::*Func )( t1, t2, t3, pcIError_base & ), t1 v1, t2 v2, t3 v3 )
	{
		pcIError_base error( NULL );
		( ptr->*Func )( v1, v2, v3, error );
		if ( error ) throw IError::MakeShared( error );
	}

	template< typename className, typename t1, typename t2, typename t3, typename t4 >
	void CallSafeFunctionReturningVoid( className * ptr,
		void ( APICALL className::*Func )( t1, t2, t3, t4, pcIError_base & ), t1 v1, t2 v2, t3 v3, t4 v4 )
	{
		pcIError_base error( NULL );
		( ptr->*Func )( v1, v2, v3, v4, error );
		if ( error ) throw IError::MakeShared( error );
	}

	template< typename className, typename t1, typename t2, typename t3, typename t4, typename t5 >
	void CallSafeFunctionReturningVoid( className * ptr,
		void ( APICALL className::*Func )( t1, t2, t3, t4, t5, pcIError_base & ), t1 v1, t2 v2, t3 v3, t4 v4, t5 v5 )
	{
		pcIError_base error( NULL );
		( ptr->*Func )( v1, v2, v3, v4, v5, error );
		if ( error ) throw IError::MakeShared( error );
	}

	template< typename className, typename t1, typename t2, typename t3, typename t4, typename t5, typename t6 >
	void CallSafeFunctionReturningVoid( className * ptr,
		void ( APICALL className::*Func )( t1, t2, t3, t4, t5, t6, pcIError_base & ), t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6 )
	{
		pcIError_base error( NULL );
		( ptr->*Func )( v1, v2, v3, v4, v5, error );
		if ( error ) throw IError::MakeShared( error );
	}

	template < typename className >
	void CallConstSafeFunctionReturningVoid( const className * const ptr,
		void ( APICALL className::*Func )( pcIError_base & ) const )
	{
		pcIError_base error( NULL );
		( ptr->*Func )( error );
		if ( error ) throw IError::MakeShared( error );
	}

	template < typename className, typename t1 >
	void CallConstSafeFunctionReturningVoid( const className * const ptr,
		void ( APICALL className::*Func )( t1, pcIError_base & ) const, t1 v1 )
	{
		pcIError_base error( NULL );
		( ptr->*Func )( v1, error );
		if ( error ) throw IError::MakeShared( error );
	}

	template < typename className, typename t1, typename t2 >
	void CallConstSafeFunctionReturningVoid( const className * const ptr,
		void ( APICALL className::*Func )( t1, t2, pcIError_base & ) const, t1 v1, t2 v2 )
	{
		pcIError_base error( NULL );
		( ptr->*Func )( v1, v2, error );
		if ( error ) throw IError::MakeShared( error );
	}

	template < typename className, typename t1, typename t2, typename t3 >
	void CallConstSafeFunctionReturningVoid( const className * const ptr,
		void ( APICALL className::*Func )( t1, t2, t3, pcIError_base & ) const, t1 v1, t2 v2, t3 v3 )
	{
		pcIError_base error( NULL );
		( ptr->*Func )( v1, v2, v3, error );
		if ( error ) throw IError::MakeShared( error );
	}

	template < typename className, typename t1, typename t2, typename t3, typename t4 >
	void CallConstSafeFunctionReturningVoid( const className * const ptr,
		void ( APICALL className::*Func )( t1, t2, t3, t4, pcIError_base & ) const, t1 v1, t2 v2, t3 v3, t4 v4 )
	{
		pcIError_base error( NULL );
		( ptr->*Func )( v1, v2, v3, v4, error );
		if ( error ) throw IError::MakeShared( error );
	}

	template < typename className, typename t1, typename t2, typename t3, typename t4, typename t5 >
	void CallConstSafeFunctionReturningVoid( const className * const ptr,
		void ( APICALL className::*Func )( t1, t2, t3, t4, t5, pcIError_base & ) const, t1 v1, t2 v2, t3 v3, t4 v4, t5 v5 )
	{
		pcIError_base error( NULL );
		( ptr->*Func )( v1, v2, v3, v4, v5, error );
		if ( error ) throw IError::MakeShared( error );
	}


	template < typename className, typename returnType, typename internalReturnType >
	returnType CallSafeFunction( className * ptr,
		internalReturnType ( APICALL className::*Func )( pcIError_base & ) )
	{
		pcIError_base error( NULL );
		internalReturnType returnValue = (ptr->*Func)( error );
		if ( error ) throw IError::MakeShared( error );
	#if XMP_WinBuild
		#pragma warning( push )
		#pragma warning( disable : 4800 )
	#endif
		return static_cast< returnType >( returnValue );
	#if XMP_WinBuild
		#pragma warning( pop )
	#endif
	}

	template < typename className, typename returnType, typename internalReturnType, typename t1 >
	returnType CallSafeFunction( className * ptr,
		internalReturnType ( APICALL className::*Func )( t1, pcIError_base & ), t1 v1 )
	{
		pcIError_base error( NULL );
		internalReturnType returnValue = (ptr->*Func)( v1, error );
		if ( error ) throw IError::MakeShared( error );
	#if XMP_WinBuild
		#pragma warning( push )
		#pragma warning( disable : 4800 )
	#endif
		return static_cast< returnType >( returnValue );
	#if XMP_WinBuild
		#pragma warning( pop )
	#endif
	}

	template < typename className, typename returnType, typename internalReturnType, typename t1, typename t2 >
	returnType CallSafeFunction( className * ptr,
		internalReturnType ( APICALL className::*Func )( t1, t2, pcIError_base & ), t1 v1, t2 v2 )
	{
		pcIError_base error( NULL );
		internalReturnType returnValue = (ptr->*Func)( v1, v2, error );
		if ( error ) throw IError::MakeShared( error );
	#if XMP_WinBuild
		#pragma warning( push )
		#pragma warning( disable : 4800 )
	#endif
		return static_cast< returnType >( returnValue );
	#if XMP_WinBuild
		#pragma warning( pop )
	#endif
	}

	template < typename className, typename returnType, typename internalReturnType, typename t1, typename t2, typename t3 >
	returnType CallSafeFunction( className * ptr,
		internalReturnType( APICALL className::*Func )( t1, t2, t3, pcIError_base & ), t1 v1, t2 v2, t3 v3 )
	{
		pcIError_base error( NULL );
		internalReturnType returnValue = ( ptr->*Func )( v1, v2, v3, error );
		if ( error ) throw IError::MakeShared( error );
	#if XMP_WinBuild
		#pragma warning( push )
		#pragma warning( disable : 4800 )
	#endif
		return static_cast< returnType >( returnValue );
	#if XMP_WinBuild
		#pragma warning( pop )
	#endif
	}

	template < typename className, typename returnType, typename internalReturnType, typename t1, typename t2, typename t3, typename t4 >
	returnType CallSafeFunction( className * ptr,
		internalReturnType( APICALL className::*Func )( t1, t2, t3, t4, pcIError_base & ), t1 v1, t2 v2, t3 v3, t4 v4 )
	{
		pcIError_base error( NULL );
		internalReturnType returnValue = ( ptr->*Func )( v1, v2, v3, v4, error );
		if ( error ) throw IError::MakeShared( error );
	#if XMP_WinBuild
		#pragma warning( push )
		#pragma warning( disable : 4800 )
	#endif
		return static_cast< returnType >( returnValue );
	#if XMP_WinBuild
		#pragma warning( pop )
	#endif
	}

	template < typename className, typename returnType, typename internalReturnType, typename t1, typename t2, typename t3, typename t4, typename t5 >
	returnType CallSafeFunction( className * ptr,
		internalReturnType( APICALL className::*Func )( t1, t2, t3, t4, t5, pcIError_base & ), t1 v1, t2 v2, t3 v3, t4 v4, t5 v5 )
	{
		pcIError_base error( NULL );
		internalReturnType returnValue = ( ptr->*Func )( v1, v2, v3, v4, v5, error );
		if ( error ) throw IError::MakeShared( error );
	#if XMP_WinBuild
		#pragma warning( push )
		#pragma warning( disable : 4800 )
	#endif
		return static_cast< returnType >( returnValue );
	#if XMP_WinBuild
		#pragma warning( pop )
	#endif
	}
    
    // Fixing AML build on mac

    template < typename className, typename returnType, typename internalReturnType, typename t1, typename t2, typename t3, typename t4, typename t5, typename t6 >
    returnType CallSafeFunction( className * ptr,
                                internalReturnType( APICALL className::*Func )( t1, t2, t3, t4, t5, t6, pcIError_base & ), t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6 )
    {
        pcIError_base error( NULL );
        internalReturnType returnValue = ( ptr->*Func )( v1, v2, v3, v4, v5, v6, error );
        if ( error ) throw IError::MakeShared( error );
    #if XMP_WinBuild
        #pragma warning( push )
        #pragma warning( disable : 4800 )
    #endif
        return static_cast< returnType >( returnValue );
    #if XMP_WinBuild
        #pragma warning( pop )
    #endif
    }

	template < typename className, typename returnType, typename internalReturnType >
	returnType CallConstSafeFunction( const className * const ptr,
		internalReturnType ( APICALL className::*Func )( pcIError_base & ) const )
	{
		pcIError_base error( NULL );
		internalReturnType returnValue = (ptr->*Func)( error );
		if ( error ) throw IError::MakeShared( error );
	#if XMP_WinBuild
		#pragma warning( push )
		#pragma warning( disable : 4800 )
	#endif
		return static_cast< returnType >( returnValue );
	#if XMP_WinBuild
		#pragma warning( pop )
	#endif
	}

	template < typename className, typename returnType, typename internalReturnType, typename t1 >
	returnType CallConstSafeFunction( const className * const ptr,
		internalReturnType ( APICALL className::*Func )( t1, pcIError_base & ) const, t1 v1 )
	{
		pcIError_base error( NULL );
		internalReturnType returnValue = (ptr->*Func)( v1, error );
		if ( error ) throw IError::MakeShared( error );
	#if XMP_WinBuild
		#pragma warning( push )
		#pragma warning( disable : 4800 )
	#endif
		return static_cast< returnType >( returnValue );
	#if XMP_WinBuild
		#pragma warning( pop )
	#endif
	}

	template < typename className, typename returnType, typename internalReturnType, typename t1, typename t2 >
	returnType CallConstSafeFunction( const className * const ptr,
		internalReturnType ( APICALL className::*Func )( t1, t2, pcIError_base & ) const, t1 v1, t2 v2 )
	{
		pcIError_base error( NULL );
		internalReturnType returnValue = (ptr->*Func)( v1, v2, error );
		if ( error ) throw IError::MakeShared( error );
	#if XMP_WinBuild
		#pragma warning( push )
		#pragma warning( disable : 4800 )
	#endif
		return static_cast< returnType >( returnValue );
	#if XMP_WinBuild
		#pragma warning( pop )
	#endif
	}

	template < typename className, typename returnType, typename internalReturnType, typename t1, typename t2, typename t3 >
	returnType CallConstSafeFunction( const className * const ptr,
		internalReturnType ( APICALL className::*Func )( t1, t2, t3, pcIError_base & ) const, t1 v1, t2 v2, t3 v3 )
	{
		pcIError_base error( NULL );
		internalReturnType returnValue = (ptr->*Func)( v1, v2, v3, error );
		if ( error ) throw IError::MakeShared( error );
	#if XMP_WinBuild
		#pragma warning( push )
		#pragma warning( disable : 4800 )
	#endif
		return static_cast< returnType >( returnValue );
	#if XMP_WinBuild
		#pragma warning( pop )
	#endif
	}

	template < typename className, typename returnType, typename internalReturnType, typename t1, typename t2, typename t3, typename t4 >
	returnType CallConstSafeFunction( const className * const ptr,
		internalReturnType ( APICALL className::*Func )( t1, t2, t3, t4, pcIError_base & ) const, t1 v1, t2 v2, t3 v3, t4 v4 )
	{
		pcIError_base error( NULL );
		internalReturnType returnValue = (ptr->*Func)( v1, v2, v3, v4, error );
		if ( error ) throw IError::MakeShared( error );
	#if XMP_WinBuild
		#pragma warning( push )
		#pragma warning( disable : 4800 )
	#endif
		return static_cast< returnType >( returnValue );
	#if XMP_WinBuild
		#pragma warning( pop )
	#endif
	}

	template < typename className, typename returnType, typename internalReturnType, typename t1, typename t2, typename t3, typename t4, typename t5 >
	returnType CallConstSafeFunction( const className * const ptr,
		internalReturnType ( APICALL className::*Func )( t1, t2, t3, t4, t5, pcIError_base & ) const, t1 v1, t2 v2, t3 v3, t4 v4, t5 v5 )
	{
		pcIError_base error( NULL );
		internalReturnType returnValue = (ptr->*Func)( v1, v2, v3, v4, v5, error );
		if ( error ) throw IError::MakeShared( error );
	#if XMP_WinBuild
		#pragma warning( push )
		#pragma warning( disable : 4800 )
	#endif
		return static_cast< returnType >( returnValue );
	#if XMP_WinBuild
		#pragma warning( pop )
	#endif
	}
    
    //Fixing AML build on mac
    
    template < typename className, typename returnType, typename internalReturnType, typename t1, typename t2, typename t3, typename t4, typename t5, typename t6 >
    returnType CallConstSafeFunction( const className * const ptr,
                                     internalReturnType ( APICALL className::*Func )( t1, t2, t3, t4, t5, t6, pcIError_base & ) const, t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6 )
    {
        pcIError_base error( NULL );
        internalReturnType returnValue = (ptr->*Func)( v1, v2, v3, v4, v5, v6, error );
        if ( error ) throw IError::MakeShared( error );
    #if XMP_WinBuild
        #pragma warning( push )
        #pragma warning( disable : 4800 )
    #endif
        return static_cast< returnType >( returnValue );
    #if XMP_WinBuild
        #pragma warning( pop )
    #endif
    }
    
    template < typename className, typename returnType, typename internalReturnType, typename t1, typename t2, typename t3, typename t4, typename t5, typename t6, typename t7 >
    returnType CallConstSafeFunction( const className * const ptr,
                                     internalReturnType ( APICALL className::*Func )( t1, t2, t3, t4, t5, t6, t7, pcIError_base & ) const, t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6, t7 v7 )
    {
        pcIError_base error( NULL );
        internalReturnType returnValue = (ptr->*Func)( v1, v2, v3, v4, v5, v6, v7, error );
        if ( error ) throw IError::MakeShared( error );
    #if XMP_WinBuild
        #pragma warning( push )
        #pragma warning( disable : 4800 )
    #endif
        return static_cast< returnType >( returnValue );
    #if XMP_WinBuild
        #pragma warning( pop )
    #endif
    }

    
    template < typename className, typename returnType, typename internalReturnType, typename t1, typename t2, typename t3, typename t4, typename t5, typename t6, typename t7, typename t8, typename t9  >
    returnType CallConstSafeFunction( const className * const ptr,
                                     internalReturnType ( APICALL className::*Func )( t1, t2, t3, t4, t5, t6, t7, t8, t9, pcIError_base & ) const, t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6, t7 v7, t8 v8, t9 v9 )
    {
        pcIError_base error( NULL );
        internalReturnType returnValue = (ptr->*Func)( v1, v2, v3, v4, v5, v6, v7, v8, v9, error );
        if ( error ) throw IError::MakeShared( error );
    #if XMP_WinBuild
        #pragma warning( push )
        #pragma warning( disable : 4800 )
    #endif
        return static_cast< returnType >( returnValue );
    #if XMP_WinBuild
        #pragma warning( pop )
    #endif
    }
    
    

	template < typename className, typename internalReturnType, typename sharedPointerType >
	shared_ptr< sharedPointerType > CallSafeFunctionReturningPointer( className * ptr,
		internalReturnType( APICALL className::*Func )( pcIError_base & ) )
	{
		pcIError_base error( NULL );
		internalReturnType returnValue = ( ptr->*Func )( error );
		if ( error ) throw IError::MakeShared( error );
		return sharedPointerType::MakeShared( returnValue );
	}

	template < typename className, typename internalReturnType, typename sharedPointerType, typename t1 >
	shared_ptr< sharedPointerType > CallSafeFunctionReturningPointer( className * ptr,
		internalReturnType( APICALL className::*Func )( t1, pcIError_base & ), t1 v1 )
	{
		pcIError_base error( NULL );
		internalReturnType returnValue = ( ptr->*Func )( v1, error );
		if ( error ) throw IError::MakeShared( error );
		return sharedPointerType::MakeShared( returnValue );
	}

	template < typename className, typename internalReturnType, typename sharedPointerType, typename t1, typename t2 >
	shared_ptr< sharedPointerType > CallSafeFunctionReturningPointer( className * ptr,
		internalReturnType( APICALL className::*Func )( t1, t2, pcIError_base & ), t1 v1, t2 v2 )
	{
		pcIError_base error( NULL );
		internalReturnType returnValue = ( ptr->*Func )( v1, v2, error );
		if ( error ) throw IError::MakeShared( error );
		return sharedPointerType::MakeShared( returnValue );
	}

	template < typename className, typename internalReturnType, typename sharedPointerType, typename t1, typename t2, typename t3 >
	shared_ptr< sharedPointerType > CallSafeFunctionReturningPointer( className * ptr,
		internalReturnType( APICALL className::*Func )( t1, t2, t3, pcIError_base & ), t1 v1, t2 v2, t3 v3 )
	{
		pcIError_base error( NULL );
		internalReturnType returnValue = ( ptr->*Func )( v1, v2, v3, error );
		if ( error ) throw IError::MakeShared( error );
		return sharedPointerType::MakeShared( returnValue );
	}

	template < typename className, typename internalReturnType, typename sharedPointerType, typename t1, typename t2, typename t3, typename t4 >
	shared_ptr< sharedPointerType > CallSafeFunctionReturningPointer( className * ptr,
		internalReturnType( APICALL className::*Func )( t1, t2, t3, t4, pcIError_base & ), t1 v1, t2 v2, t3 v3, t4 v4 )
	{
		pcIError_base error( NULL );
		internalReturnType returnValue = ( ptr->*Func )( v1, v2, v3, v4, error );
		if ( error ) throw IError::MakeShared( error );
		return sharedPointerType::MakeShared( returnValue );
	}

	template < typename className, typename internalReturnType, typename sharedPointerType, typename t1, typename t2, typename t3, typename t4, typename t5 >
	shared_ptr< sharedPointerType > CallSafeFunctionReturningPointer( className * ptr,
		internalReturnType( APICALL className::*Func )( t1, t2, t3, t4, t5, pcIError_base & ), t1 v1, t2 v2, t3 v3, t4 v4, t5 v5 )
	{
		pcIError_base error( NULL );
		internalReturnType returnValue = ( ptr->*Func )( v1, v2, v3, v4, v5, error );
		if ( error ) throw IError::MakeShared( error );
		return sharedPointerType::MakeShared( returnValue );
	}

	template < typename className, typename internalReturnType, typename sharedPointerType, typename t1, typename t2, typename t3, typename t4, typename t5, typename t6 >
	shared_ptr< sharedPointerType > CallSafeFunctionReturningPointer( className * ptr,
		internalReturnType( APICALL className::*Func )( t1, t2, t3, t4, t5, t6, pcIError_base & ), t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6 )
	{
		pcIError_base error( NULL );
		internalReturnType returnValue = ( ptr->*Func )( v1, v2, v3, v4, v5, v6, error );
		if ( error ) throw IError::MakeShared( error );
		return sharedPointerType::MakeShared( returnValue );
	}

	template < typename className, typename internalReturnType, typename sharedPointerType >
	shared_ptr< sharedPointerType > CallConstSafeFunctionReturningPointer( const className * const ptr,
		internalReturnType( APICALL className::*Func )( pcIError_base & ) const )
	{
		pcIError_base error( NULL );
		internalReturnType returnValue = ( ptr->*Func )( error );
		if ( error ) throw IError::MakeShared( error );
		return sharedPointerType::MakeShared( returnValue );
	}

	template < typename className, typename internalReturnType, typename sharedPointerType, typename t1 >
	shared_ptr< sharedPointerType > CallConstSafeFunctionReturningPointer( const className * const ptr,
		internalReturnType( APICALL className::*Func )( t1, pcIError_base & ) const, t1 v1 )
	{
		pcIError_base error( NULL );
		internalReturnType returnValue = ( ptr->*Func )( v1, error );
		if ( error ) throw IError::MakeShared( error );
		return sharedPointerType::MakeShared( returnValue );
	}

	template < typename className, typename internalReturnType, typename sharedPointerType, typename t1, typename t2 >
	shared_ptr< sharedPointerType > CallConstSafeFunctionReturningPointer( const className * const ptr,
		internalReturnType( APICALL className::*Func )( t1, t2, pcIError_base & ) const, t1 v1, t2 v2 )
	{
		pcIError_base error( NULL );
		internalReturnType returnValue = ( ptr->*Func )( v1, v2, error );
		if ( error ) throw IError::MakeShared( error );
		return sharedPointerType::MakeShared( returnValue );
	}

	template < typename className, typename internalReturnType, typename sharedPointerType, typename t1, typename t2, typename t3 >
	shared_ptr< sharedPointerType > CallConstSafeFunctionReturningPointer( const className * const ptr,
		internalReturnType( APICALL className::*Func )( t1, t2, t3, pcIError_base & ) const, t1 v1, t2 v2, t3 v3 )
	{
		pcIError_base error( NULL );
		internalReturnType returnValue = ( ptr->*Func )( v1, v2, v3, error );
		if ( error ) throw IError::MakeShared( error );
		return sharedPointerType::MakeShared( returnValue );
	}

	template < typename className, typename internalReturnType, typename sharedPointerType, typename t1, typename t2, typename t3, typename t4 >
	shared_ptr< sharedPointerType > CallConstSafeFunctionReturningPointer( const className * const ptr,
		internalReturnType( APICALL className::*Func )( t1, t2, t3, t4, pcIError_base & ) const, t1 v1, t2 v2, t3 v3, t4 v4 )
	{
		pcIError_base error( NULL );
		internalReturnType returnValue = ( ptr->*Func )( v1, v2, v3, v4, error );
		if ( error ) throw IError::MakeShared( error );
		return sharedPointerType::MakeShared( returnValue );
	}

	template < typename className, typename internalReturnType, typename sharedPointerType, typename t1, typename t2, typename t3, typename t4, typename t5 >
	shared_ptr< sharedPointerType > CallConstSafeFunctionReturningPointer( const className * const ptr,
		internalReturnType( APICALL className::*Func )( t1, t2, t3, t4, t5, pcIError_base & ) const, t1 v1, t2 v2, t3 v3, t4 v4, t5 v5 )
	{
		pcIError_base error( NULL );
		internalReturnType returnValue = ( ptr->*Func )( v1, v2, v3, v4, v5, error );
		if ( error ) throw IError::MakeShared( error );
		return sharedPointerType::MakeShared( returnValue );
	}

	template < typename className, typename internalReturnType, typename sharedPointerType, typename t1, typename t2, typename t3, typename t4, typename t5, typename t6 >
	shared_ptr< sharedPointerType > CallConstSafeFunctionReturningPointer( const className * const ptr,
		internalReturnType( APICALL className::*Func )( t1, t2, t3, t4, t5, t6, pcIError_base & ) const, t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6 )
	{
		pcIError_base error( NULL );
		internalReturnType returnValue = ( ptr->*Func )( v1, v2, v3, v4, v5, v6, error );
		if ( error ) throw IError::MakeShared( error );
		return sharedPointerType::MakeShared( returnValue );
	}

	template < typename className , typename internalReturnType , typename sharedPointerType , typename t1 , typename t2 , typename t3 , typename t4 , typename t5 , typename t6 , typename t7 , typename t8 >
	shared_ptr< sharedPointerType > CallConstSafeFunctionReturningPointer ( const className * const ptr ,
		internalReturnType ( APICALL className::*Func )(t1 , t2 , t3 , t4 , t5 , t6 , t7, t8 , pcIError_base &) const , t1 v1 , t2 v2 , t3 v3 , t4 v4 , t5 v5 , t6 v6 , t7 v7 , t8 v8 )
	{
		pcIError_base error ( NULL );
		internalReturnType returnValue = (ptr->*Func)(v1 , v2 , v3 , v4 , v5 , v6 , v7 , v8 , error);
		if ( error ) throw IError::MakeShared ( error );
		return sharedPointerType::MakeShared ( returnValue );
	}
}
//! \endcond
#endif  // TWrapperFunctions2_h__

