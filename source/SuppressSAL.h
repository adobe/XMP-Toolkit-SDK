// =================================================================================================
// Copyright 2020 Adobe
// All Rights Reserved.
// NOTICE: Adobe permits you to use, modify, and distribute this file in
// accordance with the terms of the Adobe license agreement accompanying
// it. If you have received this file from a source other than Adobe,
// then your use, modification, or distribution of it requires the prior
// written permission of Adobe. 
// =================================================================================================

//SuppressSAL.h Version 1.0

/* 
 * This file defines the Standard Annotation Language(SAL) symbols as empty, for non-windows platform. 
 * This file should be included in multi-platform code which uses SAL, to avoid build breaks 
 * on non-windows platform.
 */

#ifndef _H_SuppressSAL
#define _H_SuppressSAL

#if !defined(_WIN32) && !defined(_WIN64) /* The following definition is applicable only for non-windows platform */

#define _In_
#define _In_opt_

#define _In_z_
#define _In_opt_z_

#define _In_count_(size)
#define _In_opt_count_(size)
#define _In_bytecount_(size)
#define _In_opt_bytecount_(size)

#define _In_count_c_(size)
#define _In_opt_count_c_(size)
#define _In_bytecount_c_(size)
#define _In_opt_bytecount_c_(size)

#define _In_z_count_(size)
#define _In_opt_z_count_(size)
#define _In_z_bytecount_(size)
#define _In_opt_z_bytecount_(size)

#define _In_z_count_c_(size)
#define _In_opt_z_count_c_(size)
#define _In_z_bytecount_c_(size)
#define _In_opt_z_bytecount_c_(size)

#define _In_ptrdiff_count_(size)
#define _In_opt_ptrdiff_count_(size)

#define _In_count_x_(size)
#define _In_opt_count_x_(size)
#define _In_bytecount_x_(size)
#define _In_opt_bytecount_x_(size)

#define _Out_
#define _Out_opt_

#define _Out_cap_(size)
#define _Out_opt_cap_(size)
#define _Out_bytecap_(size)
#define _Out_opt_bytecap_(size)

#define _Out_cap_c_(size)
#define _Out_opt_cap_c_(size)
#define _Out_bytecap_c_(size)
#define _Out_opt_bytecap_c_(size)

#define _Out_cap_m_(mult,size)
#define _Out_opt_cap_m_(mult,size)
#define _Out_z_cap_m_(mult,size)
#define _Out_opt_z_cap_m_(mult,size)

#define _Out_ptrdiff_cap_(size)
#define _Out_opt_ptrdiff_cap_(size)

#define _Out_cap_x_(size)
#define _Out_opt_cap_x_(size)
#define _Out_bytecap_x_(size)
#define _Out_opt_bytecap_x_(size)

#define _Out_z_cap_(size)
#define _Out_opt_z_cap_(size)
#define _Out_z_bytecap_(size)
#define _Out_opt_z_bytecap_(size)

#define _Out_z_cap_c_(size)
#define _Out_opt_z_cap_c_(size)
#define _Out_z_bytecap_c_(size)
#define _Out_opt_z_bytecap_c_(size)

#define _Out_z_cap_x_(size)
#define _Out_opt_z_cap_x_(size)
#define _Out_z_bytecap_x_(size)
#define _Out_opt_z_bytecap_x_(size)

#define _Out_cap_post_count_(cap,count)
#define _Out_opt_cap_post_count_(cap,count)
#define _Out_bytecap_post_bytecount_(cap,count)
#define _Out_opt_bytecap_post_bytecount_(cap,count)

#define _Out_z_cap_post_count_(cap,count)
#define _Out_opt_z_cap_post_count_(cap,count)
#define _Out_z_bytecap_post_bytecount_(cap,count)
#define _Out_opt_z_bytecap_post_bytecount_(cap,count)

#define _Out_capcount_(capcount)
#define _Out_opt_capcount_(capcount)
#define _Out_bytecapcount_(capcount)
#define _Out_opt_bytecapcount_(capcount)

#define _Out_capcount_x_(capcount)
#define _Out_opt_capcount_x_(capcount)
#define _Out_bytecapcount_x_(capcount)
#define _Out_opt_bytecapcount_x_(capcount)

#define _Out_z_capcount_(capcount)
#define _Out_opt_z_capcount_(capcount)
#define _Out_z_bytecapcount_(capcount)
#define _Out_opt_z_bytecapcount_(capcount)

#define _Inout_
#define _Inout_opt_

#define _Inout_z_
#define _Inout_opt_z_

#define _Inout_count_(size)
#define _Inout_opt_count_(size)
#define _Inout_bytecount_(size)
#define _Inout_opt_bytecount_(size)

#define _Inout_count_c_(size)
#define _Inout_opt_count_c_(size)
#define _Inout_bytecount_c_(size)
#define _Inout_opt_bytecount_c_(size)

#define _Inout_z_count_(size)
#define _Inout_opt_z_count_(size)
#define _Inout_z_bytecount_(size)
#define _Inout_opt_z_bytecount_(size)

#define _Inout_z_count_c_(size)
#define _Inout_opt_z_count_c_(size)
#define _Inout_z_bytecount_c_(size)
#define _Inout_opt_z_bytecount_c_(size)

#define _Inout_ptrdiff_count_(size)
#define _Inout_opt_ptrdiff_count_(size)

#define _Inout_count_x_(size)
#define _Inout_opt_count_x_(size)
#define _Inout_bytecount_x_(size)
#define _Inout_opt_bytecount_x_(size)

#define _Inout_cap_(size)
#define _Inout_opt_cap_(size)
#define _Inout_bytecap_(size)
#define _Inout_opt_bytecap_(size)

#define _Inout_cap_c_(size)
#define _Inout_opt_cap_c_(size)
#define _Inout_bytecap_c_(size)
#define _Inout_opt_bytecap_c_(size)

#define _Inout_cap_x_(size)
#define _Inout_opt_cap_x_(size)
#define _Inout_bytecap_x_(size)
#define _Inout_opt_bytecap_x_(size)

#define _Inout_z_cap_(size)
#define _Inout_opt_z_cap_(size)
#define _Inout_z_bytecap_(size)
#define _Inout_opt_z_bytecap_(size)

#define _Inout_z_cap_c_(size)
#define _Inout_opt_z_cap_c_(size)
#define _Inout_z_bytecap_c_(size)
#define _Inout_opt_z_bytecap_c_(size)

#define _Inout_z_cap_x_(size)
#define _Inout_opt_z_cap_x_(size)
#define _Inout_z_bytecap_x_(size)
#define _Inout_opt_z_bytecap_x_(size)

#define _Ret_
#define _Ret_opt_

#define _Deref_out_
#define _Deref_out_opt_
#define _Deref_opt_out_
#define _Deref_opt_out_opt_

#define _Deref_out_z_
#define _Deref_out_opt_z_
#define _Deref_opt_out_z_
#define _Deref_opt_out_opt_z_

#define _Check_return_
#define _Check_return_opt_
#define _Check_return_wat_

#define _Printf_format_string_
#define _Scanf_format_string_
#define _Scanf_s_format_string_

#define _Success_(expr)

#define _In_bound_
#define _Out_bound_
#define _Ret_bound_
#define _Deref_in_bound_
#define _Deref_out_bound_
#define _Deref_inout_bound_
#define _Deref_ret_bound_

#define _In_range_(lb,ub)
#define _Out_range_(lb,ub)
#define _Ret_range_(lb,ub)
#define _Deref_in_range_(lb,ub)
#define _Deref_out_range_(lb,ub)
#define _Deref_ret_range_(lb,ub)

#define _Pre_z_
#define _Pre_opt_z_

#define _Pre_cap_(size)
#define _Pre_opt_cap_(size)
#define _Pre_bytecap_(size)
#define _Pre_opt_bytecap_(size)

#define _Pre_cap_c_(size)
#define _Pre_opt_cap_c_(size)
#define _Pre_bytecap_c_(size)
#define _Pre_opt_bytecap_c_(size)

#define _Pre_cap_m_(mult,size)
#define _Pre_opt_cap_m_(mult,size)

#define _Pre_cap_for_(param)
#define _Pre_opt_cap_for_(param)

#define _Pre_cap_x_(size)
#define _Pre_opt_cap_x_(size)
#define _Pre_bytecap_x_(size)
#define _Pre_opt_bytecap_x_(size)

#define _Pre_ptrdiff_cap_(ptr)
#define _Pre_opt_ptrdiff_cap_(ptr)

#define _Pre_z_cap_(size)
#define _Pre_opt_z_cap_(size)
#define _Pre_z_bytecap_(size)
#define _Pre_opt_z_bytecap_(size)

#define _Pre_z_cap_c_(size)
#define _Pre_opt_z_cap_c_(size)
#define _Pre_z_bytecap_c_(size)
#define _Pre_opt_z_bytecap_c_(size)

#define _Pre_z_cap_x_(size)
#define _Pre_opt_z_cap_x_(size)
#define _Pre_z_bytecap_x_(size)
#define _Pre_opt_z_bytecap_x_(size)

#define _Pre_valid_cap_(size)
#define _Pre_opt_valid_cap_(size)
#define _Pre_valid_bytecap_(size)
#define _Pre_opt_valid_bytecap_(size)

#define _Pre_valid_cap_c_(size)
#define _Pre_opt_valid_cap_c_(size)
#define _Pre_valid_bytecap_c_(size)
#define _Pre_opt_valid_bytecap_c_(size)

#define _Pre_valid_cap_x_(size)
#define _Pre_opt_valid_cap_x_(size)
#define _Pre_valid_bytecap_x_(size)
#define _Pre_opt_valid_bytecap_x_(size)

#define _Pre_count_(size)
#define _Pre_opt_count_(size)
#define _Pre_bytecount_(size)
#define _Pre_opt_bytecount_(size)

#define _Pre_count_c_(size)
#define _Pre_opt_count_c_(size)
#define _Pre_bytecount_c_(size)
#define _Pre_opt_bytecount_c_(size)

#define _Pre_count_x_(size)
#define _Pre_opt_count_x_(size)
#define _Pre_bytecount_x_(size)
#define _Pre_opt_bytecount_x_(size)

#define _Pre_ptrdiff_count_(ptr)
#define _Pre_opt_ptrdiff_count_(ptr)

#define _Pre_valid_
#define _Pre_opt_valid_

#define _Pre_invalid_

#define _Pre_notnull_
#define _Pre_maybenull_
#define _Pre_null_

#define _Pre_readonly_
#define _Pre_writeonly_

#define _Post_z_

#define _Post_maybez_

#define _Post_cap_(size)
#define _Post_bytecap_(size)

#define _Post_count_(size)
#define _Post_bytecount_(size)
#define _Post_count_c_(size)
#define _Post_bytecount_c_(size)
#define _Post_count_x_(size)
#define _Post_bytecount_x_(size)

#define _Post_z_count_(size)
#define _Post_z_bytecount_(size)
#define _Post_z_count_c_(size)
#define _Post_z_bytecount_c_(size)
#define _Post_z_count_x_(size)
#define _Post_z_bytecount_x_(size)

#define _Post_valid_
#define _Post_invalid_

#define _Post_notnull_

#define _Ret_z_
#define _Ret_opt_z_

#define _Ret_cap_(size)
#define _Ret_opt_cap_(size)
#define _Ret_bytecap_(size)
#define _Ret_opt_bytecap_(size)

#define _Ret_cap_c_(size)
#define _Ret_opt_cap_c_(size)
#define _Ret_bytecap_c_(size)
#define _Ret_opt_bytecap_c_(size)

#define _Ret_cap_x_(size)
#define _Ret_opt_cap_x_(size)
#define _Ret_bytecap_x_(size)
#define _Ret_opt_bytecap_x_(size)

#define _Ret_z_cap_(size)
#define _Ret_opt_z_cap_(size)
#define _Ret_z_bytecap_(size)
#define _Ret_opt_z_bytecap_(size)

#define _Ret_count_(size)
#define _Ret_opt_count_(size)
#define _Ret_bytecount_(size)
#define _Ret_opt_bytecount_(size)

#define _Ret_count_c_(size)
#define _Ret_opt_count_c_(size)
#define _Ret_bytecount_c_(size)
#define _Ret_opt_bytecount_c_(size)

#define _Ret_count_x_(size)
#define _Ret_opt_count_x_(size)
#define _Ret_bytecount_x_(size)
#define _Ret_opt_bytecount_x_(size)

#define _Ret_z_count_(size)
#define _Ret_opt_z_count_(size)
#define _Ret_z_bytecount_(size)
#define _Ret_opt_z_bytecount_(size)

#define _Ret_valid_
#define _Ret_opt_valid_

#define _Ret_notnull_
#define _Ret_maybenull_
#define _Ret_null_

#define _Deref_pre_z_
#define _Deref_pre_opt_z_

#define _Deref_pre_cap_(size)
#define _Deref_pre_opt_cap_(size)
#define _Deref_pre_bytecap_(size)
#define _Deref_pre_opt_bytecap_(size)

#define _Deref_pre_cap_c_(size)
#define _Deref_pre_opt_cap_c_(size)
#define _Deref_pre_bytecap_c_(size)
#define _Deref_pre_opt_bytecap_c_(size)

#define _Deref_pre_cap_x_(size)
#define _Deref_pre_opt_cap_x_(size)
#define _Deref_pre_bytecap_x_(size)
#define _Deref_pre_opt_bytecap_x_(size)

#define _Deref_pre_z_cap_(size)
#define _Deref_pre_opt_z_cap_(size)
#define _Deref_pre_z_bytecap_(size)
#define _Deref_pre_opt_z_bytecap_(size)

#define _Deref_pre_z_cap_c_(size)
#define _Deref_pre_opt_z_cap_c_(size)
#define _Deref_pre_z_bytecap_c_(size)
#define _Deref_pre_opt_z_bytecap_c_(size)

#define _Deref_pre_z_cap_x_(size)
#define _Deref_pre_opt_z_cap_x_(size)
#define _Deref_pre_z_bytecap_x_(size)
#define _Deref_pre_opt_z_bytecap_x_(size)

#define _Deref_pre_valid_cap_(size)
#define _Deref_pre_opt_valid_cap_(size)
#define _Deref_pre_valid_bytecap_(size)
#define _Deref_pre_opt_valid_bytecap_(size)

#define _Deref_pre_valid_cap_c_(size)
#define _Deref_pre_opt_valid_cap_c_(size)
#define _Deref_pre_valid_bytecap_c_(size)
#define _Deref_pre_opt_valid_bytecap_c_(size)

#define _Deref_pre_valid_cap_x_(size)
#define _Deref_pre_opt_valid_cap_x_(size)
#define _Deref_pre_valid_bytecap_x_(size)
#define _Deref_pre_opt_valid_bytecap_x_(size)

#define _Deref_pre_count_(size)
#define _Deref_pre_opt_count_(size)
#define _Deref_pre_bytecount_(size)
#define _Deref_pre_opt_bytecount_(size)

#define _Deref_pre_count_c_(size)
#define _Deref_pre_opt_count_c_(size)
#define _Deref_pre_bytecount_c_(size)
#define _Deref_pre_opt_bytecount_c_(size)

#define _Deref_pre_count_x_(size)
#define _Deref_pre_opt_count_x_(size)
#define _Deref_pre_bytecount_x_(size)
#define _Deref_pre_opt_bytecount_x_(size)

#define _Deref_pre_valid_
#define _Deref_pre_opt_valid_
#define _Deref_pre_invalid_

#define _Deref_pre_notnull_
#define _Deref_pre_maybenull_
#define _Deref_pre_null_

#define _Deref_pre_readonly_
#define _Deref_pre_writeonly_

#define _Deref_post_z_
#define _Deref_post_opt_z_

#define _Deref_post_cap_(size)
#define _Deref_post_opt_cap_(size)
#define _Deref_post_bytecap_(size)
#define _Deref_post_opt_bytecap_(size)

#define _Deref_post_cap_c_(size)
#define _Deref_post_opt_cap_c_(size)
#define _Deref_post_bytecap_c_(size)
#define _Deref_post_opt_bytecap_c_(size)

#define _Deref_post_cap_x_(size)
#define _Deref_post_opt_cap_x_(size)
#define _Deref_post_bytecap_x_(size)
#define _Deref_post_opt_bytecap_x_(size)

#define _Deref_post_z_cap_(size)
#define _Deref_post_opt_z_cap_(size)
#define _Deref_post_z_bytecap_(size)
#define _Deref_post_opt_z_bytecap_(size)

#define _Deref_post_z_cap_c_(size)
#define _Deref_post_opt_z_cap_c_(size)
#define _Deref_post_z_bytecap_c_(size)
#define _Deref_post_opt_z_bytecap_c_(size)

#define _Deref_post_z_cap_x_(size)
#define _Deref_post_opt_z_cap_x_(size)
#define _Deref_post_z_bytecap_x_(size)
#define _Deref_post_opt_z_bytecap_x_(size)

#define _Deref_post_valid_cap_(size)
#define _Deref_post_opt_valid_cap_(size)
#define _Deref_post_valid_bytecap_(size)
#define _Deref_post_opt_valid_bytecap_(size)
                                                
#define _Deref_post_valid_cap_c_(size)
#define _Deref_post_opt_valid_cap_c_(size)
#define _Deref_post_valid_bytecap_c_(size)
#define _Deref_post_opt_valid_bytecap_c_(size)
                                                
#define _Deref_post_valid_cap_x_(size)
#define _Deref_post_opt_valid_cap_x_(size)
#define _Deref_post_valid_bytecap_x_(size)
#define _Deref_post_opt_valid_bytecap_x_(size)

#define _Deref_post_count_(size)
#define _Deref_post_opt_count_(size)
#define _Deref_post_bytecount_(size)
#define _Deref_post_opt_bytecount_(size)

#define _Deref_post_count_c_(size)
#define _Deref_post_opt_count_c_(size)
#define _Deref_post_bytecount_c_(size)
#define _Deref_post_opt_bytecount_c_(size)

#define _Deref_post_count_x_(size)
#define _Deref_post_opt_count_x_(size)
#define _Deref_post_bytecount_x_(size)
#define _Deref_post_opt_bytecount_x_(size)

#define _Deref_post_valid_
#define _Deref_post_opt_valid_

#define _Deref_post_notnull_
#define _Deref_post_maybenull_
#define _Deref_post_null_

#define _Deref_ret_z_
#define _Deref_ret_opt_z_

#define _Deref2_pre_readonly_

#define _Prepost_z_
#define _Prepost_opt_z_

#define _Prepost_count_(size)
#define _Prepost_opt_count_(size)
#define _Prepost_bytecount_(size)
#define _Prepost_opt_bytecount_(size)
#define _Prepost_count_c_(size)
#define _Prepost_opt_count_c_(size)
#define _Prepost_bytecount_c_(size)
#define _Prepost_opt_bytecount_c_(size)
#define _Prepost_count_x_(size)
#define _Prepost_opt_count_x_(size)
#define _Prepost_bytecount_x_(size)
#define _Prepost_opt_bytecount_x_(size)

#define _Prepost_valid_
#define _Prepost_opt_valid_

#define _Deref_prepost_z_
#define _Deref_prepost_opt_z_

#define _Deref_prepost_cap_(size)
#define _Deref_prepost_opt_cap_(size)
#define _Deref_prepost_bytecap_(size)
#define _Deref_prepost_opt_bytecap_(size)

#define _Deref_prepost_cap_x_(size)
#define _Deref_prepost_opt_cap_x_(size)
#define _Deref_prepost_bytecap_x_(size)
#define _Deref_prepost_opt_bytecap_x_(size)

#define _Deref_prepost_z_cap_(size)
#define _Deref_prepost_opt_z_cap_(size)
#define _Deref_prepost_z_bytecap_(size)
#define _Deref_prepost_opt_z_bytecap_(size)

#define _Deref_prepost_valid_cap_(size)
#define _Deref_prepost_opt_valid_cap_(size)
#define _Deref_prepost_valid_bytecap_(size)
#define _Deref_prepost_opt_valid_bytecap_(size)

#define _Deref_prepost_valid_cap_x_(size)
#define _Deref_prepost_opt_valid_cap_x_(size)
#define _Deref_prepost_valid_bytecap_x_(size)
#define _Deref_prepost_opt_valid_bytecap_x_(size)

#define _Deref_prepost_count_(size)
#define _Deref_prepost_opt_count_(size)
#define _Deref_prepost_bytecount_(size)
#define _Deref_prepost_opt_bytecount_(size)

#define _Deref_prepost_count_x_(size)
#define _Deref_prepost_opt_count_x_(size)
#define _Deref_prepost_bytecount_x_(size)
#define _Deref_prepost_opt_bytecount_x_(size)

#define _Deref_prepost_valid_
#define _Deref_prepost_opt_valid_

#define _Deref_out_z_cap_c_(size)
#define _Deref_inout_z_cap_c_(size)
#define _Deref_out_z_bytecap_c_(size)
#define _Deref_inout_z_bytecap_c_(size)
#define _Deref_inout_z_

#endif

#endif //_H_SuppressSAL
