#! /usr/bin/env python3
import os
import sys
import subprocess
import shutil
import platform

if platform.system() == "Windows":
    subfolders = ["build-x32", "build-x64"]
    gen = ["Visual Studio 14 2015"]
    gen.append(gen[0] + " Win64")

elif platform.system() == "Darwin":
    
    subfolders = ["build-x64", "build-arm64"]
    gen = ["Xcode", "Xcode"]

elif platform.system() == "Linux":
    
    subfolders = ["build-x64"]
    gen = ["Unix Makefiles"]

else:
    print("Unsupported os:", os.name)
    sys.exit(-1)

for n in range(len(subfolders)):
    path = os.path.abspath(os.path.join(os.path.realpath(__file__), "..", subfolders[n]))
    if os.path.exists(path):
        shutil.rmtree(path)
    os.mkdir(path)

    print("Config:", path)
    p = subprocess.run(["cmake", "-G", gen[n], ".."], stdout=subprocess.PIPE, stderr=subprocess.STDOUT, cwd=path)
    if p.returncode:
        print("Error:", p.stdout.decode())
        sys.exit(-1)
    print("Build:", path)
    p = subprocess.run(["cmake", "--build", ".", "--config", "Release"], stdout=subprocess.PIPE, stderr=subprocess.STDOUT, cwd=path)
    if p.returncode:
        print("Error:", p.stdout.decode())
        sys.exit(-1)
