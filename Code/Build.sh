if [ ! -d "../Bin/" ]; then
    mkdir ../Bin/
fi
pushd ../Bin/

g++ ../Code/Dqn_UnitTests.cpp -std=c++17 -o Dqn_UnitTests
