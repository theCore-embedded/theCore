# theCore _Hello World_ host example

 ```

 target:            host
 external HW:       none
 toolchain:         GNU v.5.2 or greather
 additional SW:     none
 ```


This application shows theCore running on the host target.
By _host_ target is understood an environment capable to compile and run
theCore applications.

Application demonstrates a console stream usage and a simple assertion.

## Building

```
mkdir build
cd build
cmake ..
make
```

## Running

```
./hello
```

## Expected output

```
Hello, Host World!
This is an assert demonstration. Application will abort.
hello: /home/user/projects/thecore/examples/host_hello/main.cpp:7: int main(): Assertion `1 == 0' failed.
Aborted (core dumped)
```
