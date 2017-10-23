#!/usr/bin/python2.7
import os

from argparse import ArgumentParser

used = set()

def go(old_file, library_path, path):
    relative = os.path.join(os.path.dirname(old_file), path)
    relative_to_library = os.path.join(library_path, path)
    if os.path.exists(relative):
        return relative
    elif os.path.exists(relative_to_library):
        return relative_to_library
    else:
        return path

def dfs(file, library_path):
    if file in used:
        return
    used.add(file)
    if os.path.isdir(file):
        for inner in os.listdir(file):
            dfs(file + '/' + inner, library_path)
    else:
        with open(file, "r") as f:
            for line in f:
                if line.startswith("#pragma once"):
                    continue

                if line.startswith("#include <"):
                    name = line[8:].strip("\n\r \"")
                    if name in used:
                        continue
                    used.add(name)
                    print line,
                elif line.startswith("#include \""):
                    dfs(go(file, library_path, line[8:].strip("\n\r \"")), library_path)
                else:
                    print line,


def main():
    parser = ArgumentParser()
    parser.add_argument("source", type=str, help="Path to source file")
    parser.add_argument("library", type=str, help="Path to included library")
    args = parser.parse_args()

    dfs(args.source, args.library)

if __name__ == "__main__":
    main()
