/**                                                                            *
 * Copyright (C) 2012-2015 Yecheng Fu <cofyc.jackson at gmail dot com>
 * All rights reserved.
 *
 * Use of this source code is governed by a MIT-style license that can be found
 * in the LICENSE file.
 */
#ifndef ARGPARSE_S_H
#define ARGPARSE_S_H

#include <stdint.h>

#include <argparse_s_fwd.h>

//struct argparse;
//struct argparse_option;

typedef int argparse_callback (struct argparse *self
                              ,const struct argparse_option *option
                              ,int flags
                              ,void *stage1Value);

enum argparse_flag {
    ARGPARSE_STOP_AT_NON_OPTION = 1,
};

enum argparse_option_type {
    /* special */
    ARGPARSE_OPT_END,
    ARGPARSE_OPT_GROUP,
    /* options with no arguments */
    ARGPARSE_OPT_BOOLEAN,
    ARGPARSE_OPT_BIT,
    /* options with arguments (optional or required) */
    ARGPARSE_OPT_INTEGER,
    ARGPARSE_OPT_FLOAT,
    ARGPARSE_OPT_STRING,

    ARGPARSE_OPT_EXPERIMENTAL,
};

enum argparse_option_flags {
    OPT_NONEG = 1,              /* disable negation */
};



/**
 *  argparse option
 *
 *  `type`:
 *    holds the type of the option, you must have an ARGPARSE_OPT_END last in your
 *    array.
 *
 *  `short_name`:
 *    the character to use as a short option name, '\0' if none.
 *
 *  `long_name`:
 *    the long option name, without the leading dash, NULL if none.
 *
 *  `value`:
 *    stores pointer to the value to be filled.
 *
 *  `help`:
 *    the short help message associated to what the option does.
 *    Must never be NULL (except for ARGPARSE_OPT_END).
 *
 *  `callback`:
 *    function is called when corresponding argument is parsed.
 *
 *  `data`:
 *    associated data. Callbacks can use it like they want.
 *
 *  `flags`:
 *    option flags.
 */
struct argparse_option {
    enum argparse_option_type type;
    const char short_name;
    const char *long_name;
    void *value;
    const char *help;
    argparse_callback *callback;
    intptr_t data;
    int flags;
};







/**
 *  argparse
 *
 *  `options`:
 *    array with argparse options
 *
 *  `usages`:
 *    array with usage text, NULL marks end of usages.
 *
 *  `flags`:
 *    ?.
 *
 *  `description`:
 *    stores the prolog shown before usage.
 *
 *  `epilog`:
 *    stores the epilog shown after usage.
 */
struct argparse {
    // user supplied
    const struct argparse_option *options;
    const char *const *usages;
    
    // überlegungen ...
	uint64_t parsedargsbf; // parsed arguments bitfield -> callbacks can mark sucessfuly parsed arguments

//  uint64_t requiredKVBF;	// Bit-Field set by user -> keys required for a successful parse
//  uint64_t forbiddenKVBF;	// Bit-Field set by user -> keys forbidden for a successful parse
//  uint64_t keysFoundBF;		// Bit-Field set by parseKV -> stores the keys found (1 << XX_IK_*)
//  uint32_t affectedReadingsBF;	// Bit-Field set by parseKV -> stores the affected readings

    int flags;
    const char *description;    // prolog usage
    const char *epilog;         // epilog usage
    // internal context
    int argc;
    const char **argv;
    const char **out;
    int cpidx;
    const char *optvalue;       // current option value
    Entry_String_t *retMsg;
};



// built-in callbacks
int argparse_help_cb(struct argparse *self
                    ,const struct argparse_option *option
                    ,int flags
                    ,void *stage1Value);



// built-in option macros
#define OPT_END()        { ARGPARSE_OPT_END, 0, NULL, NULL, 0, NULL, 0, 0 }
#define OPT_BOOLEAN(...) { ARGPARSE_OPT_BOOLEAN, __VA_ARGS__ }
#define OPT_BIT(...)     { ARGPARSE_OPT_BIT, __VA_ARGS__ }
#define OPT_INTEGER(...) { ARGPARSE_OPT_INTEGER, __VA_ARGS__ }
#define OPT_FLOAT(...)   { ARGPARSE_OPT_FLOAT, __VA_ARGS__ }
#define OPT_STRING(...)  { ARGPARSE_OPT_STRING, __VA_ARGS__ }
#define OPT_GROUP(h)     { ARGPARSE_OPT_GROUP, 0, NULL, NULL, h, NULL, 0, 0 }
// default help ...
#define OPT_HELP()       OPT_BOOLEAN('h', "help", NULL, "show this help message", argparse_help_cb, 0, OPT_NONEG)






/* example as reference:

#define PERM_READ  (1<<0)
#define PERM_WRITE (1<<1)
#define PERM_EXEC  (1<<2)

static const char *const usages[] = {
  "get definition [options] [[--] args]",
  "get definition [options]",
  NULL,
};

  // opt-group 1
  int force = 0;
  int test = 0;
  const char *path = NULL;
  int int_num = 0;
  float flt_num = 0.f;

  // opt-group 2
  int perms = 0;

  // opt-group 3
  float flt_test2 = 0.f;
  const char *path_test2 = NULL;

  struct argparse_option options[] = {
        OPT_HELP(), // is 'h', "help", NULL, "show this help message and exit", argparse_help_cb, 0, OPT_NONEG)

        OPT_BOOLEAN  ('?', "?",      NULL,        "get for hctrl",  argparse_help_cb, 0, OPT_NONEG),


        OPT_GROUP  ("Basic options"),
        OPT_BOOLEAN  ('f', "force",  &force,      "force to do", NULL, 0, 0),
        OPT_BOOLEAN  ('t', "test",   &test,       "test only", NULL, 0, 0),
        OPT_STRING   ('p', "path",   &path,       "path to read", NULL, 0, 0),
        OPT_INTEGER  ('i', "int",    &int_num,    "selected integer", NULL, 0, 0),
        OPT_FLOAT    ('s', "float",  &flt_num,    "selected float", NULL, 0, 0),
        OPT_GROUP  ("Bits options"),
        OPT_BIT      ( 0 , "read",   &perms,      "read perm", NULL, PERM_READ, OPT_NONEG),
        OPT_BIT      ( 0 , "write",  &perms,      "write perm", NULL, PERM_WRITE, 0),
        OPT_BIT      ( 0 , "exec",   &perms,      "exec perm", NULL, PERM_EXEC, 0),
        OPT_GROUP  ("Test options"),
        OPT_FLOAT    ('x', "float2", &flt_test2,  "selected float2", NULL, 0, 0),
        OPT_STRING   ('y', "path2",  &path_test2, "path to read2", NULL, 0, 0),
        OPT_END(),
  };

*/




// Argument Parser - (helpers)

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



#endif //ARGPARSE_S_H
