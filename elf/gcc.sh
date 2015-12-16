export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.
gcc -shared -fPIC -o libf2.so foo2.c
gcc -shared -fPIC -o libf1.so foo1.c -L. -lf2
gcc -o hello hello.c -L. -lf1
