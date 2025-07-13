# CMake generated Testfile for 
# Source directory: C:/Users/kohsa/OneDrive/Documents/自己啓発活動/スマホ忘れ外出検知システム/開発/device_reminder
# Build directory: C:/Users/kohsa/OneDrive/Documents/自己啓発活動/スマホ忘れ外出検知システム/開発/device_reminder/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(AllTests "C:/Users/kohsa/OneDrive/Documents/自己啓発活動/スマホ忘れ外出検知システム/開発/device_reminder/build/Debug/test_app.exe")
  set_tests_properties(AllTests PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/kohsa/OneDrive/Documents/自己啓発活動/スマホ忘れ外出検知システム/開発/device_reminder/CMakeLists.txt;38;add_test;C:/Users/kohsa/OneDrive/Documents/自己啓発活動/スマホ忘れ外出検知システム/開発/device_reminder/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(AllTests "C:/Users/kohsa/OneDrive/Documents/自己啓発活動/スマホ忘れ外出検知システム/開発/device_reminder/build/Release/test_app.exe")
  set_tests_properties(AllTests PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/kohsa/OneDrive/Documents/自己啓発活動/スマホ忘れ外出検知システム/開発/device_reminder/CMakeLists.txt;38;add_test;C:/Users/kohsa/OneDrive/Documents/自己啓発活動/スマホ忘れ外出検知システム/開発/device_reminder/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test(AllTests "C:/Users/kohsa/OneDrive/Documents/自己啓発活動/スマホ忘れ外出検知システム/開発/device_reminder/build/MinSizeRel/test_app.exe")
  set_tests_properties(AllTests PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/kohsa/OneDrive/Documents/自己啓発活動/スマホ忘れ外出検知システム/開発/device_reminder/CMakeLists.txt;38;add_test;C:/Users/kohsa/OneDrive/Documents/自己啓発活動/スマホ忘れ外出検知システム/開発/device_reminder/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test(AllTests "C:/Users/kohsa/OneDrive/Documents/自己啓発活動/スマホ忘れ外出検知システム/開発/device_reminder/build/RelWithDebInfo/test_app.exe")
  set_tests_properties(AllTests PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/kohsa/OneDrive/Documents/自己啓発活動/スマホ忘れ外出検知システム/開発/device_reminder/CMakeLists.txt;38;add_test;C:/Users/kohsa/OneDrive/Documents/自己啓発活動/スマホ忘れ外出検知システム/開発/device_reminder/CMakeLists.txt;0;")
else()
  add_test(AllTests NOT_AVAILABLE)
endif()
subdirs("external/googletest")
