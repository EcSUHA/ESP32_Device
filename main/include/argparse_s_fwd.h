
#ifndef ARGPARSE_S_FWD_H
#define ARGPARSE_S_FWD_H



struct argparse;
struct argparse_option;


// Argument Parser - (helpers)
/*
  // typedef for Argument Parser - Split Arguments To Allocated Mememory Fn
  typedef char ** (*ArgParse_SplitArgsToAllocatedMemFn_t) (int *argc, uint8_t *argsText,  size_t argsTextLen);

  // typedef for Argument Parser - Split URL Encoded Arguments To Allocated Memory Fn
  typedef char ** (*ArgParse_SplitURLEncodedArgsToAllocatedMemFn_t) (int *argc, char *argsText);

  // typedef for Argument Parser - Free Splitted Arguments In Allocated Memory Fn
  typedef void (*ArgParse_FreeSplittedArgsInAllocatedMemFn_t) (char **argv);

  // typedef for Argument Parser - Init Fn
  typedef Entry_String_t* (*ArgParse_InitFn_t) (struct argparse *self, struct argparse_option *options,
	const char *const *usages, int flags, const char *description, const char *epilog);

  // typedef for Argument Parser - Parse Fn
  typedef Entry_String_t* (*ArgParse_ParseFn_t) (struct argparse *self, int argc, const char **argv);

  // typedef for Argument Parser - Usage Fn
  typedef void (*ArgParse_UsageFn_t) (struct argparse *self);

  // typedef for Argument Parser - Prepare Leadin Error Message Fn
  typedef void (*ArgParse_PrepareLeadinErrorMsgFn_t) (struct argparse *self, const struct argparse_option *opt,
		const char *reason, int flags);
*/

#endif //ARGPARSE_S_FWD_H
