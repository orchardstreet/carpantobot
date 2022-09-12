# An IRC Bot

## To compile and run: 

sudo apt install cmake build-essential git

git clone https://github.com/orchardstreet/carpantobot

cd carpantobot

cmake . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Debug

cd build

make

./carpantobot
