#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/samanthahuang/p4-clothsim-sp23-rendering-rebels/xcode/ext/nanogui
  /opt/homebrew/Cellar/cmake/3.25.2/bin/cmake -DOUTPUT_C=nanogui_resources.cpp -DOUTPUT_H=nanogui_resources.h -DINPUT_FILES=/Users/samanthahuang/p4-clothsim-sp23-rendering-rebels/ext/nanogui/resources/Roboto-Bold.ttf,/Users/samanthahuang/p4-clothsim-sp23-rendering-rebels/ext/nanogui/resources/Roboto-Regular.ttf,/Users/samanthahuang/p4-clothsim-sp23-rendering-rebels/ext/nanogui/resources/entypo.ttf -P /Users/samanthahuang/p4-clothsim-sp23-rendering-rebels/ext/nanogui/resources/bin2c.cmake
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/samanthahuang/p4-clothsim-sp23-rendering-rebels/xcode/ext/nanogui
  /opt/homebrew/Cellar/cmake/3.25.2/bin/cmake -DOUTPUT_C=nanogui_resources.cpp -DOUTPUT_H=nanogui_resources.h -DINPUT_FILES=/Users/samanthahuang/p4-clothsim-sp23-rendering-rebels/ext/nanogui/resources/Roboto-Bold.ttf,/Users/samanthahuang/p4-clothsim-sp23-rendering-rebels/ext/nanogui/resources/Roboto-Regular.ttf,/Users/samanthahuang/p4-clothsim-sp23-rendering-rebels/ext/nanogui/resources/entypo.ttf -P /Users/samanthahuang/p4-clothsim-sp23-rendering-rebels/ext/nanogui/resources/bin2c.cmake
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/samanthahuang/p4-clothsim-sp23-rendering-rebels/xcode/ext/nanogui
  /opt/homebrew/Cellar/cmake/3.25.2/bin/cmake -DOUTPUT_C=nanogui_resources.cpp -DOUTPUT_H=nanogui_resources.h -DINPUT_FILES=/Users/samanthahuang/p4-clothsim-sp23-rendering-rebels/ext/nanogui/resources/Roboto-Bold.ttf,/Users/samanthahuang/p4-clothsim-sp23-rendering-rebels/ext/nanogui/resources/Roboto-Regular.ttf,/Users/samanthahuang/p4-clothsim-sp23-rendering-rebels/ext/nanogui/resources/entypo.ttf -P /Users/samanthahuang/p4-clothsim-sp23-rendering-rebels/ext/nanogui/resources/bin2c.cmake
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/samanthahuang/p4-clothsim-sp23-rendering-rebels/xcode/ext/nanogui
  /opt/homebrew/Cellar/cmake/3.25.2/bin/cmake -DOUTPUT_C=nanogui_resources.cpp -DOUTPUT_H=nanogui_resources.h -DINPUT_FILES=/Users/samanthahuang/p4-clothsim-sp23-rendering-rebels/ext/nanogui/resources/Roboto-Bold.ttf,/Users/samanthahuang/p4-clothsim-sp23-rendering-rebels/ext/nanogui/resources/Roboto-Regular.ttf,/Users/samanthahuang/p4-clothsim-sp23-rendering-rebels/ext/nanogui/resources/entypo.ttf -P /Users/samanthahuang/p4-clothsim-sp23-rendering-rebels/ext/nanogui/resources/bin2c.cmake
fi

