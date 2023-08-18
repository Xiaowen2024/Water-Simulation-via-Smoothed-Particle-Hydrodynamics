#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/samanthahuang/p4-clothsim-sp23-rendering-rebels/xcode/ext/nanogui
  /opt/homebrew/Cellar/cmake/3.25.2/bin/cmake -E cmake_symlink_library /Users/samanthahuang/p4-clothsim-sp23-rendering-rebels/xcode/ext/nanogui/Debug/libnanogui.dylib /Users/samanthahuang/p4-clothsim-sp23-rendering-rebels/xcode/ext/nanogui/Debug/libnanogui.dylib /Users/samanthahuang/p4-clothsim-sp23-rendering-rebels/xcode/ext/nanogui/Debug/libnanogui.dylib
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/samanthahuang/p4-clothsim-sp23-rendering-rebels/xcode/ext/nanogui
  /opt/homebrew/Cellar/cmake/3.25.2/bin/cmake -E cmake_symlink_library /Users/samanthahuang/p4-clothsim-sp23-rendering-rebels/xcode/ext/nanogui/Release/libnanogui.dylib /Users/samanthahuang/p4-clothsim-sp23-rendering-rebels/xcode/ext/nanogui/Release/libnanogui.dylib /Users/samanthahuang/p4-clothsim-sp23-rendering-rebels/xcode/ext/nanogui/Release/libnanogui.dylib
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/samanthahuang/p4-clothsim-sp23-rendering-rebels/xcode/ext/nanogui
  /opt/homebrew/Cellar/cmake/3.25.2/bin/cmake -E cmake_symlink_library /Users/samanthahuang/p4-clothsim-sp23-rendering-rebels/xcode/ext/nanogui/MinSizeRel/libnanogui.dylib /Users/samanthahuang/p4-clothsim-sp23-rendering-rebels/xcode/ext/nanogui/MinSizeRel/libnanogui.dylib /Users/samanthahuang/p4-clothsim-sp23-rendering-rebels/xcode/ext/nanogui/MinSizeRel/libnanogui.dylib
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/samanthahuang/p4-clothsim-sp23-rendering-rebels/xcode/ext/nanogui
  /opt/homebrew/Cellar/cmake/3.25.2/bin/cmake -E cmake_symlink_library /Users/samanthahuang/p4-clothsim-sp23-rendering-rebels/xcode/ext/nanogui/RelWithDebInfo/libnanogui.dylib /Users/samanthahuang/p4-clothsim-sp23-rendering-rebels/xcode/ext/nanogui/RelWithDebInfo/libnanogui.dylib /Users/samanthahuang/p4-clothsim-sp23-rendering-rebels/xcode/ext/nanogui/RelWithDebInfo/libnanogui.dylib
fi

