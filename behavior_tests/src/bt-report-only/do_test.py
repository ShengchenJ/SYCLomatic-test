# ====------ do_test.py---------- *- Python -* ----===##
#
# Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#
# ===----------------------------------------------------------------------===#
import subprocess
import platform
import os
import sys

from test_utils import *

def setup_test():
    change_dir(test_config.current_test)
    return True

def migrate_test():
    call_subprocess(test_config.CT_TOOL + " --report-only  vector_add.cu --out-root=out --cuda-include-path=" + test_config.include_path)
    if os.path.exists("out"):
        return False
    call_subprocess(test_config.CT_TOOL + " --report-only  vector_add.cu --cuda-include-path=" + test_config.include_path)
    if os.path.exists("dpct_output"):
        return False
    return True

def build_test():
    return True

def run_test():
    return True