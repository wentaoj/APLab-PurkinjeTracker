# DDPI Algorithm API
This repository contains the standalone DDPI algorithm implementation. Several dependencies (namely, `gstreamer` and `spdlog`) have been removed to create and more portable API. 

## Recommended Usage
1. Add this repository as a git submodule or clone it to a directory within your project working directory
2. In `CMakeLists.txt` of your project, add the directory of this library as a subdirectory:

    ```add_subdirectory(...path/to/this_library)```
3. Make the headers available to your project
    ```include_directories(...paht/to/this_library/include)```