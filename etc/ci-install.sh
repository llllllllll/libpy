# I hate yaml-as-code so damn much. Just use a bash script

set +e


if [ -n "$GCC" ];then
    export CXX="g++-$GCC" CC="gcc-$GCC"
elif [ -n "$CLANG" ];then
    export CXX="clang++-$CLANG" CC="clang-$CLANG"
fi

if [[ "$TRAVIS_OS_NAME" = "osx" ]];then
    wget "https://www.python.org/ftp/python/$PYTHON_VER/python-$PYTHON_VER-macosx10.6.pkg"
    sudo installer -pkg "python-$PYTHON_VER-macosx10.6.pkg" -target /
fi
