
// -*- C++ -*-
// $Id$
// Definition for Win32 Export directives.
// This file is generated automatically by generate_export_file.pl device_state
// ------------------------------
#ifndef DEVICE_STATE_EXPORT_H
#define DEVICE_STATE_EXPORT_H

#include "ace/config-all.h"

#if defined (ACE_AS_STATIC_LIBS) && !defined (DEVICE_STATE_HAS_DLL)
#  define DEVICE_STATE_HAS_DLL 0
#endif /* ACE_AS_STATIC_LIBS && DEVICE_STATE_HAS_DLL */

#if !defined (DEVICE_STATE_HAS_DLL)
#  define DEVICE_STATE_HAS_DLL 1
#endif /* ! DEVICE_STATE_HAS_DLL */

#if defined (DEVICE_STATE_HAS_DLL) && (DEVICE_STATE_HAS_DLL == 1)
#  if defined (DEVICE_STATE_BUILD_DLL)
#    define device_state_Export ACE_Proper_Export_Flag
#    define DEVICE_STATE_SINGLETON_DECLARATION(T) ACE_EXPORT_SINGLETON_DECLARATION (T)
#    define DEVICE_STATE_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_EXPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  else /* DEVICE_STATE_BUILD_DLL */
#    define device_state_Export ACE_Proper_Import_Flag
#    define DEVICE_STATE_SINGLETON_DECLARATION(T) ACE_IMPORT_SINGLETON_DECLARATION (T)
#    define DEVICE_STATE_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_IMPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  endif /* DEVICE_STATE_BUILD_DLL */
#else /* DEVICE_STATE_HAS_DLL == 1 */
#  define device_state_Export
#  define DEVICE_STATE_SINGLETON_DECLARATION(T)
#  define DEVICE_STATE_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#endif /* DEVICE_STATE_HAS_DLL == 1 */

// Set DEVICE_STATE_NTRACE = 0 to turn on library specific tracing even if
// tracing is turned off for ACE.
#if !defined (DEVICE_STATE_NTRACE)
#  if (ACE_NTRACE == 1)
#    define DEVICE_STATE_NTRACE 1
#  else /* (ACE_NTRACE == 1) */
#    define DEVICE_STATE_NTRACE 0
#  endif /* (ACE_NTRACE == 1) */
#endif /* !DEVICE_STATE_NTRACE */

#if (DEVICE_STATE_NTRACE == 1)
#  define DEVICE_STATE_TRACE(X)
#else /* (DEVICE_STATE_NTRACE == 1) */
#  if !defined (ACE_HAS_TRACE)
#    define ACE_HAS_TRACE
#  endif /* ACE_HAS_TRACE */
#  define DEVICE_STATE_TRACE(X) ACE_TRACE_IMPL(X)
#  include "ace/Trace.h"
#endif /* (DEVICE_STATE_NTRACE == 1) */

#endif /* DEVICE_STATE_EXPORT_H */

// End of auto generated file.
