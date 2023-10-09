#! /usr/bin/env python3
import os
import sys
import subprocess
import shutil
import platform

arch = []
configs = ["Debug", "Release"]
singleConfig = True
if platform.system() == "Windows":
    subfolders = ["build-x64"]
    gen = ["Visual Studio 16 2019"]
    arch = ["-A", "x64"]
    singleConfig = False
elif platform.system() == "Darwin":    
    subfolders = ["build"]
    gen = ["Ninja"]
elif platform.system() == "Linux":    
    subfolders = ["build-x64"]
    gen = ["Unix Makefiles"]
elif platform.system() == "Haiku":
    subfolders = ["build-x64"]
    gen = ["Ninja"]
else:
    print("Unsupported os:", os.name, platform.system())
    sys.exit(-1)

first = True
clean = len(sys.argv) > 1 and sys.argv[1].lower() == 'clean'

for n in range(len(subfolders)):
    for config in configs:
        path = os.path.abspath(os.path.join(os.path.realpath(__file__), "..", subfolders[n]))
        if singleConfig:
            path = path + "-" + config.lower()
        
        if (first or singleConfig or clean) and os.path.exists(path):
            if clean:
                print("removing:", path)
            shutil.rmtree(path)
        if clean:
            continue

        os.mkdir(path)

        if first or singleConfig:
            first = False
            print("Configuring:", path)
            args = ["cmake", "-G", gen[n]] + arch + [".."]
            if singleConfig:
                args += ["-DCMAKE_BUILD_TYPE="+config]

            print("args:", " ".join(args))
            p = subprocess.run(args, cwd=path) # stdout=subprocess.PIPE, stderr=subprocess.STDOUT, 
            if p.returncode:
                print("Error: cmake failed.")
                sys.exit(-1)

        print(config, "Build:", path)
        args = ["cmake", "--build", "."]
        if not singleConfig:
            args += ["--config", config]
        if 0:
            # Debug: don't hide output
            print("args:", " ".join(args))
            p = subprocess.run(args, cwd=path)
        else:
            p = subprocess.run(args, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, cwd=path)
        if p.returncode:
            print(p.stdout.decode())
            print("Error: build failed.")
            sys.exit(-1)
