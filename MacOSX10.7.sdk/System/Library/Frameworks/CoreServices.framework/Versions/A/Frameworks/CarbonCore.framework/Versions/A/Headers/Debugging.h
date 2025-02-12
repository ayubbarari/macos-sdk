/*
     File:       CarbonCore/Debugging.h
 
     Contains:   Macros to handle exceptions and assertions.
 
     Version:    CarbonCore-960.24~1
 
     Copyright:  � 1989-2008 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __DEBUGGING__
#define __DEBUGGING__

#ifndef __MACTYPES__
#include <CarbonCore/MacTypes.h>
#endif

#ifndef __MIXEDMODE__
#include <CarbonCore/MixedMode.h>
#endif

#ifndef __MACERRORS__
#include <CarbonCore/MacErrors.h>
#endif


#include <AvailabilityMacros.h>

#if PRAGMA_ONCE
#pragma once
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
    This file supplies standard debugging routines and macros to Carbon and
    Classic Mac OS programs. Other C programs which wish to use the
    exception handling and assertion macros should include AssertMacros.h
    instead of this file.

    To activate debugger breaks, #define DEBUG to 1 (one) before including this
    file. Five further levels of debugging are available, selected by #defining
    one of the following conditionals to 1 after DEBUG is defined to 1.

        DEBUG_INTERNAL      the default; asserts include file and line number
                            information

        DEBUG_EXTERNAL      used for code which must ship to developers outside
                            your organization; no file or line number
                            information is included in asserts

        DEBUG_BREAK_ONLY    where an assertion string would normally be sent to
                            the debugger; call Debugger() instead.

        PRODUCTION          used for shipping code; no debugger breaks are
                            emitted

        PERFORMANCE         same as PRODUCTION

    #defining DEBUG to 0 is equivalent to #defining PRODUCTION 1 when
    DEBUG is 1. (No code for debugger breaks is emitted in either case.)
    
    #defining DEBUG to 1 without specifying a level is equivalent to #defining
    DEBUG_INTERNAL 1.

    In addition, these macros should also be #defined (they are described in detail below):
        kComponentSignatureString
        COMPONENT_SIGNATURE
*/
/*
 *  Before including this file, #define kComponentSignatureString to a C-string
 *  containing the name of your client and #define COMPONENT_SIGNATURE to your
 *  client's unique signature (i.e., your program's registered creator type).
 *  For example:
 *
 *      #define kComponentSignatureString "SurfWriter"
 *      #define COMPONENT_SIGNATURE 'WAVE'
 *
 *  If you don't define kComponentSignatureString and COMPONENT_SIGNATURE, the
 *  default kComponentSignatureString and COMPONENT_SIGNATURE values will be
 *  used by the DEBUGASSERTMSG macros below.
 */

#ifndef kComponentSignatureString
 #define kComponentSignatureString "Third Party Client"
#endif
#ifndef COMPONENT_SIGNATURE
  #define COMPONENT_SIGNATURE '?*?*'
#endif
#define QuoteExceptionString(x) #x


/*
 *  The DEBUGLEVEL's
 */
#define DEBUG_LEVEL_PRODUCTION 0
#define DEBUG_LEVEL_BREAK_ONLY    1
#define DEBUG_LEVEL_EXTERNAL  3
#define DEBUG_LEVEL_INTERNAL  4
#define DEBUGFULL             DEBUG_LEVEL_INTERNAL

/*
 * define DEBUGLEVEL
 */
#if DEBUG
    #if PRODUCTION
     #define DEBUGLEVEL DEBUG_LEVEL_PRODUCTION
  #elif PERFORMANCE
      #define DEBUGLEVEL DEBUG_LEVEL_PRODUCTION
  #elif DEBUG_BREAK_ONLY
     #define DEBUGLEVEL DEBUG_LEVEL_BREAK_ONLY
  #elif DEBUG_EXTERNAL
       #define DEBUGLEVEL DEBUG_LEVEL_EXTERNAL
    #elif DEBUG_INTERNAL
       #define DEBUGLEVEL DEBUG_LEVEL_INTERNAL
    #else
      /* default to DEBUG_LEVEL_INTERNAL */
      #define DEBUGLEVEL DEBUG_LEVEL_INTERNAL
    #endif
#endif
#ifndef DEBUGLEVEL
#define DEBUGLEVEL DEBUG_LEVEL_PRODUCTION
#endif



/*
 *  The options parameter to DebugAssert and DEBUGASSERTMSG is currently reserved (must be zero).
 */
#define DEBUG_NO_OPTIONS      0

/*
 *  DEBUGASSERTMSG()
 *
 *  Summary:
 *    All error reporting is routed through this macro, which calls the system
 *    routine DebugAssert(). If you wish to use your own assertion break
 *    routine, you can override DEBUGASSERTMSG by defining it before including
 *    this file.
 *
 *  Parameters:
 *
 *    componentSignature:
 *      The unique signature of component causing the assertion.
 *    
 *    options:
 *      reserved.
 *    
 *    assertionString:
 *      A pointer to a string constant containing the assertion.
 *      This must be a string constant (and not a string variable or
 *      NULL) because the Proeprocessor concatenates it with other
 *      string constants
 *    
 *    exceptionLabelString:
 *      A pointer to a string containing the exceptionLabel, or NULL.
 *    
 *    errorString:
 *      A pointer to the error string, or NULL. DEBUGASSERTMSG macros
 *      must not attempt to concatenate this string with constant
 *      character strings.
 *    
 *    fileName:
 *      A pointer to the fileName or pathname (generated by the
 *      preprocessor __FILE__ identifier), or NULL.
 *    
 *    lineNumber:
 *      The line number in the file (generated by the preprocessor
 *      __LINE__ identifier), or 0 (zero).
 *    
 *    value:
 *      A value associated with the assertion, or NULL.
 */

#ifndef DEBUGASSERTMSG
    #if DEBUGLEVEL == DEBUG_LEVEL_PRODUCTION
       /* no debugger message */
      #define DEBUGASSERTMSG(componentSignature, options, assertionString, exceptionLabelString, errorString, fileName, lineNumber, value)
   #elif DEBUGLEVEL == DEBUG_LEVEL_BREAK_ONLY
     /* call Debugger() if it is available */
       #define DEBUGASSERTMSG(componentSignature, options, assertionString, exceptionLabelString, errorString, fileName, lineNumber, value) \
         Debugger()
 #elif DEBUGLEVEL == DEBUG_LEVEL_EXTERNAL
       /* exclude fileName and lineNumber */
      #define DEBUGASSERTMSG(componentSignature, options, assertionString, exceptionLabelString, errorString, fileName, lineNumber, value) \
         DebugAssert(componentSignature, options, assertionString, exceptionLabelString, errorString, 0, 0, (void*)value)
   #else
      /* include all info */
     #define DEBUGASSERTMSG(componentSignature, options, assertionString, exceptionLabelString, errorString, fileName, lineNumber, value) \
         DebugAssert(componentSignature, options, assertionString, exceptionLabelString, errorString, fileName, lineNumber, (void*)value)
   #endif
#endif

/*
 *  Define the three inputs to AssertMacros.h
 */

#ifndef DEBUG_ASSERT_COMPONENT_NAME_STRING
    #define DEBUG_ASSERT_COMPONENT_NAME_STRING kComponentSignatureString
#endif
#ifndef DEBUG_ASSERT_PRODUCTION_CODE
   #define DEBUG_ASSERT_PRODUCTION_CODE (DEBUGLEVEL==0)
#endif
#ifndef DEBUG_ASSERT_MESSAGE
   #define DEBUG_ASSERT_MESSAGE(componentNameString, assertionString, exceptionLabelString, errorString, fileName, lineNumber, value) \
       DEBUGASSERTMSG(COMPONENT_SIGNATURE, DEBUG_NO_OPTIONS, componentNameString ": " assertionString, exceptionLabelString, errorString, fileName, lineNumber, value)
#endif

/*
 *  Include AssertMacros.h where all of the check, verify, and require macros are defined
 */
#include <AssertMacros.h>
/*
 *  The following check, verify, and require macros assert that TaskLevel is 0.
 */

#define ATTASKLEVEL0()                                                         \
  (TaskLevel() == 0)


#define check_tasklevel0()                                                     \
  check(ATTASKLEVEL0())


#define check_tasklevel0_string(cstring)                                       \
  __Check_string(ATTASKLEVEL0(), cstring)


#define verify_tasklevel0()                                                        \
  __Verify(ATTASKLEVEL0())


#define verify_tasklevel0_string(cstring)                                      \
  verify_string(ATTASKLEVEL0(), cstring)


#define require_tasklevel0(exceptionLabel)                                     \
  __Require(ATTASKLEVEL0(), exceptionLabel)


#define require_tasklevel0_action(exceptionLabel, action)                      \
  __Require_action(ATTASKLEVEL0(), exceptionLabel, action)


#define require_tasklevel0_quiet(exceptionLabel)                               \
  __Require_quiet(ATTASKLEVEL0(), exceptionLabel)


#define require_tasklevel0_action_quiet(exceptionLabel, action)                    \
  __Require_action_quiet(ATTASKLEVEL0(), exceptionLabel, action)


#define require_tasklevel0_string(exceptionLabel, cstring)                     \
  __Require_string(ATTASKLEVEL0(), exceptionLabel, cstring)


#define require_tasklevel0_action_string(exceptionLabel, action, cstring)      \
  __Require_action_string(ATTASKLEVEL0(), exceptionLabel, action, cstring)

/*
 *  You can use DPRINTF as a dprintf which goes away in production builds.
 *  DPRINTF is not supported by Carbon because dprintf
 *  is not supported by Carbon.
 *
 *  To use it, double-parenthesize the argument - i.e., use:
 *
 *      DPRINTF(("formatString %d", 5 ));
 *
 *  This is sadly necessary because a macro can not have a variable number
 *  of arguments.
 *
 *  DPRINTF is defined only if it is not already defined to
 *  prevent conflicts with developer code.
 */

#if defined(DEBUG) && DEBUG
    #define DPRINTF(x) dprintf x
#else
    #define DPRINTF(x)  { }
#endif


/*
 *  kBlessedBusErrorBait is an address that will never be mapped by
 *  Mac OS 8 or 9. It is close to the middle of the 64K range from 0x68F10000
 *  to 0x68F1FFFF that is unmapped and cannot be accessed without causing an
 *  exception. Thus, it's a good value to use for filling uninitialized
 *  pointers, etc.
 *
 *  Mac OS X programs should NOT use kBlessedBusErrorBait and should use
 *  zero (0) instead, since by default, page 0 is read and write protected
 *  for user code.
 */
enum {
  kBlessedBusErrorBait          = 0x68F168F1
};

/*
 *  DebugAssert()
 *  
 *  Summary:
 *    DebugAssert is the system routine that the DEBUGASSERTMSG macro
 *    calls (by default) to display assertion messsages. The output
 *    from DebugAssert can be redirected by installing a
 *    DebugAssertOutputHandler with InstallDebugAssertOutputHandler.
 *  
 *  Mac OS X threading:
 *    Thread safe
 *    This call is thread safe if no debug components are registered,
 *    and may be thread unsafe if there are debug components being
 *    installed or removed.
 *  
 *  Parameters:
 *    
 *    componentSignature:
 *      The unique signature of component causing the assertion.
 *    
 *    options:
 *      reserved.
 *    
 *    assertionString:
 *      A pointer to a string containing the assertion, or NULL.
 *    
 *    exceptionLabelString:
 *      A pointer to a string containing the exceptionLabel, or NULL.
 *    
 *    errorString:
 *      A pointer to the error string, or NULL.
 *    
 *    fileName:
 *      A pointer to the fileName or pathname (generated by the
 *      preprocessor __FILE__ identifier), or NULL.
 *    
 *    lineNumber:
 *      The line number in the file (generated by the preprocessor
 *      __LINE__ identifier), or 0 (zero).
 *    
 *    value:
 *      A value associated with the assertion, or NULL.
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in CoreServices.framework
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Non-Carbon CFM:   in DebugLib 1.0 and later
 */
extern void 
DebugAssert(
  OSType        componentSignature,
  UInt32        options,
  const char *  assertionString,
  const char *  exceptionLabelString,
  const char *  errorString,
  const char *  fileName,
  long          lineNumber,
  void *        value)                                        AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;


/*
 *  TaskLevel masks
 */
enum {
  k68kInterruptLevelMask        = 0x00000007, /* 68K interrupt levels 0 through 7 */
  kInVBLTaskMask                = 0x00000010, /* VBLs are executing */
  kInDeferredTaskMask           = 0x00000020, /* Deferred tasks are executing */
  kInSecondaryIntHandlerMask    = 0x00000040, /* Secondary interrupt handlers are executing */
  kInNestedInterruptMask        = 0x00000080 /* The system is handling an interrupt */
};

/*
 *  TaskLevel()
 *  
 *  Summary:
 *    TaskLevel returns 0 if we're (probably) running at non-interrupt
 *    time. There's no way to make this perfect, but this is as close
 *    as we can get. If TaskLevel doesn't return 0, then one of the
 *    TaskLevel masks can be used to learn more. Mac OS X has no
 *    concept of "TaskLevel"; and so it will always return the value 0.
 *  
 *  Mac OS X threading:
 *    Thread safe
 *  
 *  Result:
 *    The current task level.
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in CoreServices.framework
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Non-Carbon CFM:   in DebugLib 1.0 and later
 */
extern UInt32 
TaskLevel(void)                                               AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;


/*
 *  Constants used by the DebugComponent functions
 */
enum {
  kComponentDebugOption         = 0     /* optionSelectorNum to turn breaks for component On or Off*/
};

enum {
  kGetDebugOption               = 1,    /* get current debug option setting*/
  kSetDebugOption               = 2     /* set debug option*/
};


/*
 *  DebugComponentCallbackProcPtr
 *  
 *  Discussion:
 *    DebugComponentCallback is the callback into a component that
 *    registers with DebugLib. It is called to get the debug option
 *    setting, or to turn a debug option on or off.
 *  
 *  Parameters:
 *    
 *    optionSelectorNum:
 *      The component debug option to set.
 *    
 *    command:
 *      The command the DebugComponentCallbackProc must handle:
 *       kGetDebugOption - get current debug option setting
 *       kSetDebugOption - set debug option
 *    
 *    optionSetting:
 *      A pointer to a Boolean where the DebugComponentCallbackProc
 *      must return the option setting: the current setting if command
 *      is kGetDebugOption; the new debug option if command is 
 *      kSetDebugOption
 */
typedef CALLBACK_API( void , DebugComponentCallbackProcPtr )(SInt32 optionSelectorNum, UInt32 command, Boolean *optionSetting);
typedef STACK_UPP_TYPE(DebugComponentCallbackProcPtr)           DebugComponentCallbackUPP;
/*
 *  NewDebugComponent()
 *  
 *  Summary:
 *    NewDebugComponent registers a component with DebugLib.
 *  
 *  Mac OS X threading:
 *    Not thread safe
 *  
 *  Parameters:
 *    
 *    componentSignature:
 *      The unique signature of component.
 *    
 *    componentName:
 *      The displayable string naming the component.
 *    
 *    componentCallback:
 *      The callback into component for working with options.
 *  
 *  Result:
 *    An operating system result code: noErr, memFullErr,
 *    debuggingExecutionContextErr, debuggingDuplicateSignatureErr, or
 *    debuggingInvalidNameErr.
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in CoreServices.framework
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Non-Carbon CFM:   in DebugLib 1.0 and later
 */
extern OSStatus 
NewDebugComponent(
  OSType                      componentSignature,
  ConstStr255Param            componentName,
  DebugComponentCallbackUPP   componentCallback)              AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;


/*
 *  NewDebugOption()
 *  
 *  Summary:
 *    NewDebugOption registers a debug option with DebugLib.
 *  
 *  Mac OS X threading:
 *    Not thread safe
 *  
 *  Parameters:
 *    
 *    componentSignature:
 *      The signature of component to register a debug option for.
 *    
 *    optionSelectorNum:
 *      The selector number of this debug option.
 *    
 *    optionName:
 *      The displayable string naming this debug option.
 *  
 *  Result:
 *    An operating system result code: noErr, memFullErr,
 *    debuggingExecutionContextErr, debuggingDuplicateOptionErr,
 *    debuggingInvalidSignatureErr, debuggingInvalidNameErr, or
 *    debuggingNoCallbackErr.
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in CoreServices.framework
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Non-Carbon CFM:   in DebugLib 1.0 and later
 */
extern OSStatus 
NewDebugOption(
  OSType             componentSignature,
  SInt32             optionSelectorNum,
  ConstStr255Param   optionName)                              AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;


/*
 *  DisposeDebugComponent()
 *  
 *  Summary:
 *    DisposeDebugComponent removes a component registration and all
 *    related debug options from DebugLib.
 *  
 *  Mac OS X threading:
 *    Not thread safe
 *  
 *  Parameters:
 *    
 *    componentSignature:
 *      The unique signature of a component.
 *  
 *  Result:
 *    An operating system result code: noErr,
 *    debuggingExecutionContextErr, or debuggingInvalidSignatureErr.
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in CoreServices.framework
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Non-Carbon CFM:   in DebugLib 1.0 and later
 */
extern OSStatus 
DisposeDebugComponent(OSType componentSignature)              AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;


/*
 *  GetDebugComponentInfo()
 *  
 *  Summary:
 *    GetDebugComponentInfo returns a component registered with
 *    DebugLib.
 *  
 *  Mac OS X threading:
 *    Not thread safe
 *  
 *  Parameters:
 *    
 *    itemIndex:
 *      The index into the list of registered components (1-based).
 *    
 *    componentSignature:
 *      A pointer to an OSType where the unique signature of a
 *      component is returned.
 *    
 *    componentName:
 *      A pointer to an Str255 where the displayable string naming a
 *      component is returned.
 *  
 *  Result:
 *    An operating system result code: noErr or debuggingNoMatchErr.
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in CoreServices.framework
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Non-Carbon CFM:   in DebugLib 1.0 and later
 */
extern OSStatus 
GetDebugComponentInfo(
  UInt32    itemIndex,
  OSType *  componentSignature,
  Str255    componentName)                                    AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;


/*
 *  GetDebugOptionInfo()
 *  
 *  Summary:
 *    GetDebugOptionInfo returns a debug option registered with
 *    DebugLib.
 *  
 *  Mac OS X threading:
 *    Not thread safe
 *  
 *  Parameters:
 *    
 *    itemIndex:
 *      The index into the list of registered debug options (0-based);
 *      0 = kComponentDebugOption.
 *    
 *    componentSignature:
 *      The unique signature of a component.
 *    
 *    optionSelectorNum:
 *      A pointer to an SInt32 where the selector number of this debug
 *      option is returned.
 *    
 *    optionName:
 *      A pointer to an Str255 where the displayable string naming this
 *      debug option is returned.
 *    
 *    optionSetting:
 *      A pointer to an Boolean where the current debug option setting
 *      is returned.
 *  
 *  Result:
 *    An operating system result code: noErr,
 *    debuggingInvalidSignatureErr, or debuggingNoMatchErr.
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in CoreServices.framework
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Non-Carbon CFM:   in DebugLib 1.0 and later
 */
extern OSStatus 
GetDebugOptionInfo(
  UInt32     itemIndex,
  OSType     componentSignature,
  SInt32 *   optionSelectorNum,
  Str255     optionName,
  Boolean *  optionSetting)                                   AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;


/*
 *  SetDebugOptionValue()
 *  
 *  Summary:
 *    SetDebugOptionValue sets a debug option registered with DebugLib.
 *  
 *  Mac OS X threading:
 *    Not thread safe
 *  
 *  Parameters:
 *    
 *    componentSignature:
 *      The unique signature of a component.
 *    
 *    optionSelectorNum:
 *      The selector number of this debug option.
 *    
 *    newOptionSetting:
 *      The new debug option setting.
 *  
 *  Result:
 *    An operating system result code: noErr,
 *    debuggingInvalidSignatureErr, or debuggingInvalidOptionErr.
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in CoreServices.framework
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Non-Carbon CFM:   in DebugLib 1.0 and later
 */
extern OSStatus 
SetDebugOptionValue(
  OSType    componentSignature,
  SInt32    optionSelectorNum,
  Boolean   newOptionSetting)                                 AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;



/*
 *  DebugAssertOutputHandlerProcPtr
 *  
 *  Discussion:
 *    DebugAssertOutputHandler is the callback that registers with
 *    DebugLib to handle the output from DebugAssert. The
 *    "componentSignature" through "value" parameters are the raw
 *    values passed to DebugAssert when an exception occurs.
 *  
 *  Parameters:
 *    
 *    componentSignature:
 *      The unique signature of component causing the assertion.
 *    
 *    options:
 *      reserved.
 *    
 *    assertionString:
 *      A pointer to a string containing the assertion, or NULL.
 *    
 *    exceptionLabelString:
 *      A pointer to a string containing the exceptionLabel, or NULL.
 *    
 *    errorString:
 *      A pointer to the error string, or NULL.
 *    
 *    fileName:
 *      A pointer to the fileName or pathname (generated by the
 *      preprocessor __FILE__ identifier), or NULL.
 *    
 *    lineNumber:
 *      The line number in the file (generated by the preprocessor
 *      __LINE__ identifier), or 0 (zero).
 *    
 *    value:
 *      A value associated with the assertion, or NULL.
 *    
 *    outputMsg:
 *      The string DebugAssert build which would normally be passed to
 *      DebugStr if a DebugAssertOutputHandler isn't installed.
 */
typedef CALLBACK_API( void , DebugAssertOutputHandlerProcPtr )(OSType componentSignature, UInt32 options, const char *assertionString, const char *exceptionLabelString, const char *errorString, const char *fileName, long lineNumber, void *value, ConstStr255Param outputMsg);
typedef STACK_UPP_TYPE(DebugAssertOutputHandlerProcPtr)         DebugAssertOutputHandlerUPP;
/*
 *  InstallDebugAssertOutputHandler()
 *  
 *  Summary:
 *    InstallDebugAssertOutputHandler installs a
 *    DebugAssertOutputHandler which DebugAssert calls instead of
 *    DebugStr.
 *  
 *  Parameters:
 *    
 *    handler:
 *      The DebugAssertOutputHandler to install or NULL to switch back
 *      to the default handler (DebugStr).
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in CoreServices.framework
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Non-Carbon CFM:   in DebugLib 1.0 and later
 */
extern void 
InstallDebugAssertOutputHandler(DebugAssertOutputHandlerUPP handler) AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;


/*
 *  dprintf()
 *  
 *  Summary:
 *    printf takes a variable argument list and 'prints' that to the
 *    debugging output handler.  Calling dprintf() from anything but C
 *    or C++ is tricky.
 *  
 *  Parameters:
 *    
 *    format:
 *      The format string.
 *    
 *    ...:
 *      The arguments to the format string.
 *  
 *  Availability:
 *    Mac OS X:         not available
 *    CarbonLib:        not available
 *    Non-Carbon CFM:   in DebugLib 1.1 and later
 */


/*
 *  vdprintf()
 *  
 *  Summary:
 *    vdprintf takes a va_args list and 'prints' that to the debugging
 *    output handler.
 *  
 *  Parameters:
 *    
 *    format:
 *      The format string.
 *    
 *    va_args_list:
 *      The va_args list.
 *  
 *  Availability:
 *    Mac OS X:         not available
 *    CarbonLib:        not available
 *    Non-Carbon CFM:   in DebugLib 1.1 and later
 */


/*
 *  GetMacOSStatusErrorString()
 *  
 *  Summary:
 *    Returns a const char* string which corresponds to the textual
 *    constant for the given OSStatus code.
 *  
 *  Discussion:
 *    This function returns a text string which corresponds to the
 *    given OSStatus code, based on the errors in MacErrors.h.  For
 *    example, GetMacOSStatusErrorString( -43 ) returns "fnfErr", which
 *    is the text representation for the error constant -43.  This
 *    function is useful if you want to get or print out ( for
 *    debugging purposes only ) a useful description for a given
 *    OSStatus error.  If no string is available for the given
 *    constant, then the empty string "" is returned. Some error values
 *    have multiple meanings; in those cases the multiple meanings are
 *    all returned seperated by '/'es.
 *  
 *  Mac OS X threading:
 *    Thread safe
 *  
 *  Parameters:
 *    
 *    err:
 *      The OSStatus to return a text string for.
 *  
 *  Result:
 *    A const char* string corresponding to the given OSStatus
 *  
 *  Availability:
 *    Mac OS X:         in version 10.4 and later in CoreServices.framework
 *    CarbonLib:        not available
 *    Non-Carbon CFM:   not available
 */
extern const char * 
GetMacOSStatusErrorString(OSStatus err)                       AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;


/*
 *  GetMacOSStatusCommentString()
 *  
 *  Summary:
 *    Returns a const char* string which corresponds to the descriptive
 *    string for the given OSStatus code.
 *  
 *  Discussion:
 *    This function returns a text string which corresponds to a
 *    comment for the given OSStatus code, based on the errors in
 *    MacErrors.h.  For example, GetMacOSStatusConstantString( -43 )
 *    returns "File not found", which is the text representation for
 *    the error constant -43.  This function is useful if you want to
 *    get or print out ( for debugging purposes only ) a useful
 *    description for a given OSStatus error.  If no string is
 *    available for the given constant, then the empty string "" is
 *    returned. If no string is available for the given constant, then
 *    the empty string "" is returned.  Some error values have multiple
 *    meanings; in those cases the multiple meanings are all returned
 *    seperated by '/'es.
 *  
 *  Mac OS X threading:
 *    Thread safe
 *  
 *  Parameters:
 *    
 *    err:
 *      The OSStatus to return a text string for.
 *  
 *  Result:
 *    A const char* string corresponding to the given OSStatus
 *  
 *  Availability:
 *    Mac OS X:         in version 10.4 and later in CoreServices.framework
 *    CarbonLib:        not available
 *    Non-Carbon CFM:   not available
 */
extern const char * 
GetMacOSStatusCommentString(OSStatus err)                     AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;



/*
   pascal < threadsafe, exportset=fw_CarbonCore_XMerlot >
   Boolean IsDebuggerAttachedToProcess ( );
*/
/*
 *  NewDebugComponentCallbackUPP()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in CoreServices.framework
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Non-Carbon CFM:   available as macro/inline
 */
extern DebugComponentCallbackUPP
NewDebugComponentCallbackUPP(DebugComponentCallbackProcPtr userRoutine) AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;

/*
 *  NewDebugAssertOutputHandlerUPP()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in CoreServices.framework
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Non-Carbon CFM:   available as macro/inline
 */
extern DebugAssertOutputHandlerUPP
NewDebugAssertOutputHandlerUPP(DebugAssertOutputHandlerProcPtr userRoutine) AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;

/*
 *  DisposeDebugComponentCallbackUPP()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in CoreServices.framework
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Non-Carbon CFM:   available as macro/inline
 */
extern void
DisposeDebugComponentCallbackUPP(DebugComponentCallbackUPP userUPP) AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;

/*
 *  DisposeDebugAssertOutputHandlerUPP()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in CoreServices.framework
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Non-Carbon CFM:   available as macro/inline
 */
extern void
DisposeDebugAssertOutputHandlerUPP(DebugAssertOutputHandlerUPP userUPP) AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;

/*
 *  InvokeDebugComponentCallbackUPP()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in CoreServices.framework
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Non-Carbon CFM:   available as macro/inline
 */
extern void
InvokeDebugComponentCallbackUPP(
  SInt32                     optionSelectorNum,
  UInt32                     command,
  Boolean *                  optionSetting,
  DebugComponentCallbackUPP  userUPP)                         AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;

/*
 *  InvokeDebugAssertOutputHandlerUPP()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in CoreServices.framework
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Non-Carbon CFM:   available as macro/inline
 */
extern void
InvokeDebugAssertOutputHandlerUPP(
  OSType                       componentSignature,
  UInt32                       options,
  const char *                 assertionString,
  const char *                 exceptionLabelString,
  const char *                 errorString,
  const char *                 fileName,
  long                         lineNumber,
  void *                       value,
  ConstStr255Param             outputMsg,
  DebugAssertOutputHandlerUPP  userUPP)                       AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;

#if __MACH__
  #ifdef __cplusplus
    inline DebugComponentCallbackUPP                            NewDebugComponentCallbackUPP(DebugComponentCallbackProcPtr userRoutine) { return userRoutine; }
    inline DebugAssertOutputHandlerUPP                          NewDebugAssertOutputHandlerUPP(DebugAssertOutputHandlerProcPtr userRoutine) { return userRoutine; }
    inline void                                                 DisposeDebugComponentCallbackUPP(DebugComponentCallbackUPP) { }
    inline void                                                 DisposeDebugAssertOutputHandlerUPP(DebugAssertOutputHandlerUPP) { }
    inline void                                                 InvokeDebugComponentCallbackUPP(SInt32 optionSelectorNum, UInt32 command, Boolean * optionSetting, DebugComponentCallbackUPP userUPP) { (*userUPP)(optionSelectorNum, command, optionSetting); }
    inline void                                                 InvokeDebugAssertOutputHandlerUPP(OSType componentSignature, UInt32 options, const char * assertionString, const char * exceptionLabelString, const char * errorString, const char * fileName, long lineNumber, void * value, ConstStr255Param outputMsg, DebugAssertOutputHandlerUPP userUPP) { (*userUPP)(componentSignature, options, assertionString, exceptionLabelString, errorString, fileName, lineNumber, value, outputMsg); }
  #else
    #define NewDebugComponentCallbackUPP(userRoutine)           ((DebugComponentCallbackUPP)userRoutine)
    #define NewDebugAssertOutputHandlerUPP(userRoutine)         ((DebugAssertOutputHandlerUPP)userRoutine)
    #define DisposeDebugComponentCallbackUPP(userUPP)
    #define DisposeDebugAssertOutputHandlerUPP(userUPP)
    #define InvokeDebugComponentCallbackUPP(optionSelectorNum, command, optionSetting, userUPP) (*userUPP)(optionSelectorNum, command, optionSetting)
    #define InvokeDebugAssertOutputHandlerUPP(componentSignature, options, assertionString, exceptionLabelString, errorString, fileName, lineNumber, value, outputMsg, userUPP) (*userUPP)(componentSignature, options, assertionString, exceptionLabelString, errorString, fileName, lineNumber, value, outputMsg)
  #endif
#endif


#ifdef __cplusplus
}
#endif

#endif /* __DEBUGGING__ */

