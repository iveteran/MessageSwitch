g++ -D__UNITTEST__ -o crypto crypto.cpp -lcrypto
g++ -D__UNITTEST__ -o random random.cpp
g++ -D__UNITTEST__ -o time time.cpp
g++ -D__UNITTEST__ -o md5_test md5_test.cpp md5.cpp
