#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/samanthahuang/p4-clothsim-sp23-rendering-rebels/xcode
  make -f /Users/samanthahuang/p4-clothsim-sp23-rendering-rebels/xcode/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/samanthahuang/p4-clothsim-sp23-rendering-rebels/xcode
  make -f /Users/samanthahuang/p4-clothsim-sp23-rendering-rebels/xcode/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/samanthahuang/p4-clothsim-sp23-rendering-rebels/xcode
  make -f /Users/samanthahuang/p4-clothsim-sp23-rendering-rebels/xcode/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/samanthahuang/p4-clothsim-sp23-rendering-rebels/xcode
  make -f /Users/samanthahuang/p4-clothsim-sp23-rendering-rebels/xcode/CMakeScripts/ReRunCMake.make
fi

