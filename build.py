#! /usr/bin/env python3
import os
import sys
import subprocess
import shutil
import platform

arch = []
configs = ["Debug", "Release"]
singleConfig = True
universalCheck = []
universalArchs = []
if platform.system() == "Windows":
    subfolders = ["build-x64"]
    gen = ["Visual Studio 16 2019"]
    arch = ["-A", "x64"]
    singleConfig = False
elif platform.system() == "Darwin":    
    subfolders = ["build"]
    gen = ["Ninja"]
    universalCheck.append("aspell-0.60.6.1/libaspell-dist-0.60$tag.dylib")
    universalCheck.append("libchardet/libchardet.dylib")
    universalCheck.append("libiconv-1.17/libiconv.dylib")
    universalCheck.append("Btree/libbtree.dylib")
    universalCheck.append("bzip2-1.0.6/libbzip2.dylib")
    universalCheck.append("lib/libjpeg.1.58.dylib")
    universalCheck.append("lib/libpng16.16.dylib")
    universalArchs.append('x86_64')
    universalArchs.append('arm64')
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

        if config.lower() == "debug":
            tag = "d"
        else:
            tag = ""
        
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
        args = ["cmake", "--build", ".", "--target", "install"]
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
        else:
            for file in universalCheck:
                fileName = file.replace("$tag", tag);
                check = os.path.join(path, fileName)
                p = subprocess.run(["file", check], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
                parts = p.stdout.decode().split()
                found = []
                for p in parts:
                    if p in universalArchs:
                        found.append(p)
                if len(found) == len(universalArchs):
                    print("Universal check:", check, "ok")
                else:
                    print("Universal check:", check, "ERROR: missing architectures, found:", found)
                    sys.exit(-1)

