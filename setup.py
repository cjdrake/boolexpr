#!/usr/bin/env python3
#
# Copyright 2016 Chris Drake
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


from setuptools import setup


with open("VERSION") as fin:
    VERSION = fin.read().strip()

with open("README.rst") as fin:
    README = fin.read()

CLASSIFIERS = [
    "Development Status :: 1 - Planning",
    "License :: OSI Approved :: Apache Software License",
    "Programming Language :: C++",
    "Programming Language :: Python",
    "Programming Language :: Python :: 3.4",
    "Programming Language :: Python :: 3.5",
    "Programming Language :: Python",
    "Topic :: Scientific/Engineering :: Mathematics",
]

setup(
    name="boolexpr",
    version=VERSION,
    author="Chris Drake",
    author_email="cjdrake@gmail.com",
    url="http://www.boolexpr.org",
    description="Boolean Expressions",
    long_description=README,
    classifiers=CLASSIFIERS,
    license="Apache 2.0",
    install_requires=["cffi>=1.5.0"],
    setup_requires=["cffi>=1.5.0"],
    cffi_modules=["boolexpr_build.py:ffi"],
    packages=["boolexpr"],
)
