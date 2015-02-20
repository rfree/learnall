#!/bin/bash 

cd $HOME/

option_log="$HOME/log.txt"

function wget_sha() {
	wget "$1/$2" -O $2
	sha512sum "$2" | tee -a "$option_log" # save to log
}

function git_sha() {
	rm -rf "./$2"
	git clone "$1"
	(
		cd $2
		git log HEAD | head -n 1 | tee -a "$option_log" # save to log
	)
}

function clang3_4() { 
        cd $HOME/
        mkdir clang3_4 
        cd clang3_4/

        wget_sha http://www.llvm.org/releases/3.4.1 cfe-3.4.1.src.tar.gz
        wget_sha http://www.llvm.org/releases/3.4.1 llvm-3.4.1.src.tar.gz

        tar -xzf llvm-3.4.1.src.tar.gz
        tar -xzf cfe-3.4.1.src.tar.gz 

        mv cfe-3.4.1.src llvm-3.4.1.src/tools/clang
        mkdir build
        cd build

        CC=/usr/bin/clang CXX=/usr/bin/clang++ ../llvm-3.4.1.src/configure --prefix=$HOME/.local
        ionice -c3 make -j2
        make install 

				echo "=== MODIFY the bashrc ==="

				echo '# settings for Open-Transactions / and newcli' >> ~/.bashrc
        echo 'export CC="$HOME/.local/bin/clang" ; export CXX="$HOME/.local/bin/clang++" ; export CPP="$HOME/.local/bin/clang -E"' >> ~/.bashrc
        echo 'export PKG_CONFIG_PATH=$HOME/.local/lib/pkgconfig:$PKG_CONFIG_PATH' >> ~/.bashrc
        source ~/.bashrc # re-read the settings to use them right now
} 


function newer_cmake() { 
        mkdir $HOME/cmake_3 
        cd $HOME/cmake_3

        wget_sha http://www.cmake.org/files/v3.1 cmake-3.1.3.tar.gz  
        tar -xf cmake-3.1.3.tar.gz
        #sha256sum cmake-3.1.3.tar.gz 
        mkdir build && cd build/ 
        ../cmake-3.1.3/configure --prefix=$HOME/.local 
        make && make install 

        echo "export CMAKE_PREFIX_PATH=$HOME/.local/ " >> ~/.bashrc
        echo "export LD_LIBRARY_PATH=$HOME/.local/lib/x86_64-linux-gnu/:$HOME/.local/lib:$HOME/.local/lib64:$HOME/.local/include " >> ~/.bashrc 
#       echo "export CC=$HOME/.local/bin/clang ; export CXX=$HOME/.local/bin/clang++ ; export CPP=$HOME/.local/bin/clang -E " >> ~/.bashrc
        echo "export PKG_CONFIG_PATH="$HOME/.local/lib/x86_64-linux-gnu/pkgconfig/:$HOME/.local/lib/pkgconfig:$PKG_CONFIG_PATH" " >> ~/.bashrc
        echo "export PATH=$HOME/.local/bin:$PATH" >> ~/.bashrc 

        source ~/.bashrc 
        cmake --version 
        read _
} 

function libsodium() { 
        cd $HOME/
        git_sha https://github.com/jedisct1/libsodium libsodium
        cd libsodium 
        ./autogen.sh  
        ./configure --prefix=$HOME/.local  
        make && make install
}

function zeromq4() { 
        cd $HOME/ 
        git_sha https://github.com/zeromq/zeromq4-x.git zeromq4-x
        cd zeromq4-x/
				echo "WARNING: this must use gcc, disabling local compiler for now"
        unset CC CXX CPP
        C_INCLUDE_PATH=$HOME/.local/include 
        CPLUS_INCLUDE_PATH=$HOME/.local/include
        ./autogen.sh  
        ./configure --with-libsodium --prefix=$HOME/.local 
        make && make install 
        source ~/.bashrc 
} 

function clang3_5() { 
        cd $HOME
         mkdir llvm-3.5 && cd llvm-3.5
         wget_sha http://www.llvm.org/releases/3.5.0 cfe-3.5.0.src.tar.xz
         wget_sha http://www.llvm.org/releases/3.5.0 llvm-3.5.0.src.tar.xz
         unxz *.xz 
         tar -xf cfe-3.5.0.src.tar 
         tar -xf llvm-3.5.0.src.tar 
         mv cfe-3.5.0.src llvm-3.5.0.src/tools/clang 
         mkdir build && cd build 
         ../llvm-3.5.0.src/configure --prefix=$HOME/.local
         make && make install
         clang -v
        read _
} 

function libedit() { 
				echo "TODO use newer version? (2015 version works)"  # TODO

        cd $HOME
        wget_sha http://thrysoee.dk/editline libedit-20141030-3.1.tar.gz 
        tar -xzf  libedit-20141030-3.1.tar.gz
        cd libedit-20141030-3.1
        ./configure --prefix=$HOME/.local 
        make && make install 
}

echo "Selecting projects to build:"

read -r -e -i"y" -p"* build clang 3.4 (needed unless you have system clang >= 3.4) (y/n): " option_clang3_4
read -r -e -i"y" -p"* build clang 3.5 (needed unless you have system clang >= 3.5. Build it e.g. in addition to 3.4 - it works better) (y/n): " option_clang3_5
read -r -e -i"y" -p"* build cmake (y/n) " option_cmake
read -r -e -i"y" -p"* build libsodium (y/n) " option_libsodium
read -r -e -i"y" -p"* build zeromq4 (y/n) " option_zeromq4
read -r -e -i"y" -p"* build libedit (y/n) " option_libedit

[[ "$option_clang3_4" == "y" ]] && clang3_4 
[[ "$option_clang3_5" == "y" ]] && clang3_5
[[ "$option_cmake" == "y" ]] && newer_cmake
[[ "$option_libsodium" == "y" ]] && libsodium 
[[ "$option_zeromq4" == "y" ]] && zeromq4 
[[ "$option_libedit" == "y" ]] && libedit

echo "Entire build process is done"

echo "Testing versions:"
source ~/.bashrc
clang -v
cmake --version
pkg-config --libs libsodium
pkg-config --libs libzeromq4
pkg-config --libs libedit


