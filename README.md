# MAKE SURE YOU HAVE 7-ZIP INSTALLED ON YOUR DEVICE!!!

# Growtopia Dataminer

This project provides a tool to download the latest Growtopia version, extract data, and compare item names between versions. Below you'll find instructions for building the project and using the tool. Precompiled version is also available for users who prefer not to compile the code themselves.

## Table of Contents

1. [Project Overview](#project-overview)
2. [Building the Project](#building-the-project)
3. [Using the Tool](#using-the-tool)
4. [Releases](#releases)
5. [Contributing](#contributing)
6. [License](#license)

## Project Overview

The Growtopia Data Extraction Project is a C++ tool that:

- Downloads the latest Growtopia.
- Extracts item data from the binary.
- Compares item names with those from a previous version.
- Outputs new or updated items.

## Building the Project

To build the project, you'll need `g++` and `libcurl` installed. Here’s how you can compile the code:

1. **Navigate to the Project Directory:**
   - Open a Command Prompt and navigate to the directory where the project is located.

2. **Compile the Project:**
   - Run the following command to compile the source code and link against `libcurl`:
     ```sh
     g++ src/dataminer.cpp -I"C:\libcurl\include" -L"C:\libcurl\lib" -lcurl
     ```
   - Adjust the `-I` and `-L` paths as necessary to point to your `libcurl` installation directories.

3. **Verify the Build:**
   - Ensure that the executable `dataminer.exe` is created in the project directory.

## Using the Tool

1. **Run the Tool:**
   - Open a Command Prompt in the project directory.
   - Execute the compiled binary:
     ```sh
     dataminer.exe
     ```
   - Follow the prompts to input the previous version number.

2. **Review Results:**
   - The tool will download the latest Growtopia version, extract the relevant data, compare items with the previous version, and display new or updated items.

## Releases

For users who prefer not to compile the code, precompiled version is available. You can download it from the [Releases page](https://github.com/Bolwl/Dataminer/releases). 

## Contributing

Contributions are welcome! If you would like to contribute to the project:

- Fork the repository on GitHub.
- Create a new branch for your changes.
- Commit your changes and push to your fork.
- Submit a pull request with a description of your changes.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for more details.
