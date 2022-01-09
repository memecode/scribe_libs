#! /usr/bin/env python3
import os
import sys
import subprocess
import shutil

if os.name == "nt":
    subfolders = ["build32", "build64"]
    gen = ["Visual Studio 14 2015"]
    gen.append(gen[0] + " Win64")

elif os.name == "posix":
    
    subfolders = ["build"]
    gen = ["Xcode"]

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
