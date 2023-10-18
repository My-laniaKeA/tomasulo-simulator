# Tomasulo Simulator

2023 Computer System Architecture Tomasulo Simulator Experimental Framework

The instruction set uses RISC-V 32 IM

The experimental document is published [here](https://lab.cs.tsinghua.edu.cn/ca-lab-docs/labs/tomasulo/), where you can check the experimental requirements and the experimental guide.

If you have any questions, please contact the teaching assistant in the **course WeChat group / Web Learning answer area**.

## Directory Structure

```
.
├── backend             # Backend files
├── checkfiles          # Test case files
├── CMakeLists.txt
├── common              # Common source code
├── frontend            # Frontend files
├── include             # Header files
├── program             # Executable program
├── readme.md 
├── test                # Test user program
└── thirdparty          # Third-party code
```

## Precautions

1. ICache address is mapped to `0x80000000 ~ 0x803FFFFF`, DCache address is mapped to `0x80400000 ~ 0x807FFFFF`
2. sp has been initialized to 0x80800000, gp to 0x80400000
3. Currently using `0x0000000b` as exit mark

## Experiment Preparation

Before the experiment, you need to prepare the following tools:

1. cmake, g++: The minimum version of cmake is 3.15, and the minimum version of g++ is 9.0.0

2. (Optional) riscv toolchain: Both riscv-gnu-linux- and riscv-unknown-elf- toolchains are available. It is recommended to use the linux environment, and the toolchain can be installed directly through apt.

   ```bash
   sudo apt install g++-riscv64-linux-gnu
   sudo apt install g++-riscv64-unknown-elf
   ```

3. If there is no toolchain, you need to prepare the pre-issued test case elf file for testing

## Usage

This experimental framework uses cmake for compilation, and the compilation method is as follows:

```bash
mkdir build
cd build
cmake ..
make
```

If using riscv64-linux-gnu-toolchain, modify the Toolchain prefix in `./test/CMakeLists.txt` before compiling

The `test` folder is used to store test cases. If new test cases are added, please manually re-cmake.

If there is no toolchain, please comment out the last line of `./CMakeLists.txt`, so that the test case file will not be compiled.

Afterwards, you can run checker to check the correctness of the implementation, such as:

For riscv64-unknown-elf-toolchain:

```bash
./checker -f ./test/selection_sort -c ../checkfiles/selection_sort.chk 
```

For riscv64-linux-gnu-toolchain:

```bash
./checker -f ./test/selection_sort -c ../checkfiles/selection_sort_linux.chk
```

If the last line shows: `[ OK ] 16 testcase(s) passed`, it means that the current test case has passed and you can continue testing other test cases.

All places that need to be implemented / modified have been marked with `TODO:`, and you can search globally for positioning.

Please do not modify any files that are not marked with `TODO:`, this may cause you to fail subsequent tests.

## About Gitlab CI **(Important!)**

> Due to resource limitations, the GitLab CI/CD has not been migrated to GitHub Actions.

Normally, when you execute `git push` to push your local repository to the remote end, gitlab will add your evaluation task to the execution queue.

To ensure the normal operation of `Gitlab CI`, **please do not modify `.gitlab-ci.yml`, please do not modify `.gitlab-ci.yml`, please do not modify `.gitlab-ci.yml`.**

### CI Task Description

The running tasks include two, namely `job 1` and `job 2`. `job 1` corresponds to Tomasulo scheduling test (excluding branch prediction). `job 2` corresponds to tests including branch prediction.

Depending on whether you have completed or not completed the optional content of branch prediction, you should make the following modifications to the files in your repository.

#### I have completed Tomasulo backend, excluding branch prediction

Please change `export CHECK=true` in the first line of `check_predict.sh` file to `export CHECK=false`.

#### I have completed optional content (branch prediction)

Please make sure that `export CHECK=true` is in the first line of `check_predict.sh` file.
