mkdir -p ../Bin/
pushd ../Bin/
g++ ../Code/Dqn_UnitTests.cpp -std=c++17 -o Dqn_UnitTests
g++ ../Code/DqnHeader.h -D DQN_HEADER_IMPLEMENTATION -std=c++17 -o DqnHeader
