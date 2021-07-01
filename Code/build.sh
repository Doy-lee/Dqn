mkdir -p ../Bin/
pushd ../Bin/
g++ ../Code/Dqn_Tests.cpp -D DQN_TEST_WITH_MAIN -std=c++17 -o Dqn_UnitTests
