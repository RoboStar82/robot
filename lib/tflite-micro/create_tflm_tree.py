#! /usr/bin/env python3

import os
import re
import sys
import subprocess

os.chdir(os.path.join(os.path.dirname(__file__), 'tflite-micro'))

if not os.path.exists('tensorflow/lite/micro/tools/make/downloads/gcc_embedded'):
    os.symlink(os.path.join(os.environ['HOME'], '.platformio/packages/toolchain-gccarmnoneeabi'), 'tensorflow/lite/micro/tools/make/downloads/gcc_embedded')

args = [
    sys.executable,
    "tensorflow/lite/micro/tools/project_generation/create_tflm_tree.py",
    "--makefile_options=TARGET=cortex_m_generic TARGET_ARCH=cortex-m7 OPTIMIZED_KERNEL_DIR=cmsis_nn",
    "--print_src_files",
    "--no_copy",
    "."
]

r = subprocess.run(args, stdout=subprocess.PIPE, text=True)

for file in re.split(r'\s+', r.stdout):
    if file == '':
        continue
    if '.h' in file or '.c' in file:
        print('      "+<' + file + '>",')
