/***************************************************************************
 *   Copyright 2017 by Davide Bettio <davide@uninstall.it>                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

#include <assert.h>
#include <libgen.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "atom.h"
#include "mapped_file.h"
#include "Context.h"
#include "Module.h"
#include "Term.h"

#include "bif.h"
#include "iff.h"
#include "utils.h"

struct Test{
    const char *test_file;
    int32_t expected_value;
};

struct Test tests[] =
{
    {"erlang_tests/add.beam", 17},
    {"erlang_tests/fact.beam", 120},
    {"erlang_tests/mutrec.beam", 6},
    {"erlang_tests/morelabels.beam", 6},
    {NULL, 0}
};

void test_modules_execution()
{
    struct Test *test = tests;

    do {
        MappedFile *beam_file = mapped_file_open_beam(test->test_file);
        assert(beam_file != NULL);

        Module *mod = module_new_from_iff_binary(beam_file->mapped, beam_file->size);
        Context *ctx = context_new();

        context_execute_loop(ctx, mod, beam_file->mapped);

        int32_t value = term_to_int32(ctx->x[0]);
        if (value != test->expected_value) {
            fprintf(stderr, "\x1b[1;31mFailed test module %s, got value: %i\x1b[0m\n", test->test_file, value);
        }

        mapped_file_close(beam_file);

        test++;
    } while (test->test_file);
}

int main(int argc, char **argv)
{
    UNUSED(argc)

    chdir(dirname(argv[0]));

    test_modules_execution();

    return EXIT_SUCCESS;
}