/**
 * system/src/ui/ambsh_priv.h
 *
 * Simple shell.
 *
 * History:
 *    2005/11/19 - [Charles Chiou] created file
 *
 * Copyright (C) 2004-2005, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */

#ifndef __UI__AMBSH_PRIV_H__
#define __UI__AMBSH_PRIV_H__

/* __BEGIN_C_PROTO__ */

extern AMBA_SHELL_CMD_s *ambsh_yyparse_file(const char *file);
extern AMBA_SHELL_CMD_s *ambsh_yyparse_rom_file(const char *file);
extern AMBA_SHELL_CMD_s *ambsh_yyparse_text(const char *s, int len);
extern const char *ambsh_yyparse_err(void);

extern int ambsh_lex_load_file(const char *file);
extern int ambsh_lex_load_rom_file(const char *file);
extern int ambsh_lex_load_text(const char *s, int len);
extern int ambsh_lex_unload(void);

extern AMBA_SHELL_CMD_s *AmbaShell_CmdMalloc(int type);
extern int AmbaShell_CmdFree(AMBA_SHELL_CMD_s *this);
extern int AmbaShell_CmdAddPipe(AMBA_SHELL_CMD_s *this, AMBA_SHELL_CMD_s *that);
extern int AmbaShell_CmdAddNext(AMBA_SHELL_CMD_s *this, AMBA_SHELL_CMD_s *that);
extern AMBA_SHELL_CMD_s *AmbaShell_CmdAddv(AMBA_SHELL_CMD_s *this, const char *s);
extern int AmbaShell_CmdAddArg(AMBA_SHELL_CMD_s *this, const char *s);
extern int ambsh_cmd_set_infile(AMBA_SHELL_CMD_s *this, const char *s);
extern int ambsh_cmd_set_outfile(AMBA_SHELL_CMD_s *this, const char *s);
extern int ambsh_cmd_set_apdfile(AMBA_SHELL_CMD_s *this, const char *s);
extern int ambsh_cmd_print(AMBA_SHELL_ENV_s *env, AMBA_SHELL_CMD_s *this, int level);

/* __END_C_PROTO__ */

#endif
