import pathlib
import shutil
import subprocess
import sys
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[1]


class EuLanguageNativeTests(unittest.TestCase):
    @unittest.skipUnless(sys.platform.startswith('linux') and shutil.which('c++'),
                         'requires Linux and a C++ compiler')
    def test_synthetic_hooks(self):
        with tempfile.TemporaryDirectory(prefix='eu-language-native-') as directory:
            executable = pathlib.Path(directory) / 'test'
            subprocess.run(['c++', '-std=c++20', '-O2', '-Wall', '-Wextra', '-Werror',
                            '-Wno-unused-function', '-pthread', '-I' + str(ROOT / 'gpu/shadps4'),
                            str(ROOT / 'tests/native_eu_language_test.cpp'), '-o', str(executable)], check=True)
            subprocess.run([str(executable)], check=True)
