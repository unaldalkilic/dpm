import argparse
import os

PROGRAM = "dpm"
INCLUDE = "include"
SOURCE = "src"
TEST = "test"
MAIN = "main.cpp"

def file_traverser(path: str, extension: str) -> list[str]:
    result = []
    if os.path.isdir(path):
        for filepath in os.listdir(path):
            result += file_traverser(f"{path}/{filepath}", extension)
    else:
        return [path] if path.endswith(extension) else [""]
    
    return result

def output_for_debug(all_dependencies: str) -> str:
    result = ""
    for dependency in all_dependencies.split(" "):
        if dependency == "":
            continue
        else:
            result += f"\"{dependency}\",\n"
    return result

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--test", type=bool, default=False, help="Build with tests or not")
    args = parser.parse_args()
    is_test = args.test

    include_str = " ".join(file_traverser(INCLUDE, ".h"))
    src_str = " ".join(file_traverser(SOURCE, ".cpp"))
    test_str = " ".join(file_traverser(TEST, ".cpp")) + " " + " ".join(file_traverser(TEST, ".h")) if is_test else ""
    command = f"g++ {MAIN} {src_str} {include_str} {test_str} -o {PROGRAM}"
    print(output_for_debug(src_str+" "+include_str+" "+test_str))
    os.system(command)