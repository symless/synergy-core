#!/usr/bin/env python3

import lib.env as env

env.ensure_in_venv(__file__)

import argparse, os, sys
import lib.cmd_utils as cmd_utils
import lib.colors as colors

valgrind_bin = "valgrind"


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--unit-tests", action="store_true")
    parser.add_argument("--integ-tests", action="store_true")
    parser.add_argument(
        "--filter-file",
        type=str,
        help="Takes the base filename without extension and uses it as a filter",
    )
    parser.add_argument(
        "--ignore-return-code",
        action="store_true",
        help="Ignore the return code of the test command",
    )
    parser.add_argument(
        "--valgrind",
        action="store_true",
        help="Run the test command with valgrind",
    )

    args = parser.parse_args()
    binary = get_binary_path(args)

    if args.filter_file:
        file_base = os.path.basename(args.filter_file)
        without_ext = os.path.splitext(file_base)[0]
        command = [binary, f"--gtest_filter={without_ext}*"]
    else:
        command = [binary]

    if args.valgrind:
        if not cmd_utils.has_command(valgrind_bin):
            print(f"{colors.ERROR_TEXT} {valgrind_bin} not found")
            sys.exit(1)

        command = [valgrind_bin] + command

    result = cmd_utils.run(command, print_cmd=True, check=False)
    if not args.ignore_return_code:
        sys.exit(result.returncode)


def get_binary_path(args):
    base_dir = "./build/bin"
    if args.unit_tests:
        return f"{base_dir}/unittests"
    elif args.integ_tests:
        return f"{base_dir}/integtests"
    else:
        raise RuntimeError("No test type specified")


if __name__ == "__main__":
    main()
