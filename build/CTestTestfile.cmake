# CMake generated Testfile for 
# Source directory: /Users/kohsaku/Library/CloudStorage/OneDrive-個人用/Documents/workspace/ローカル開発/device_reminder
# Build directory: /Users/kohsaku/Library/CloudStorage/OneDrive-個人用/Documents/workspace/ローカル開発/device_reminder/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(AllTests "/Users/kohsaku/Library/CloudStorage/OneDrive-個人用/Documents/workspace/ローカル開発/device_reminder/build/test_app")
set_tests_properties(AllTests PROPERTIES  _BACKTRACE_TRIPLES "/Users/kohsaku/Library/CloudStorage/OneDrive-個人用/Documents/workspace/ローカル開発/device_reminder/CMakeLists.txt;38;add_test;/Users/kohsaku/Library/CloudStorage/OneDrive-個人用/Documents/workspace/ローカル開発/device_reminder/CMakeLists.txt;0;")
subdirs("external/googletest")
