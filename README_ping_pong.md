# Task 1
To run:

```
mpic++ ping_pong.cpp -o ping_pong
```

```
mpirun --allow-run-as-root -n 5 ./ping_pong
```

# Task 2
We assume that initialization time is approximately the same for different array sizes.
So we measure the time of program execution

To run:

```
mpic++ -std=c++11  ping_pong_2.cpp -o ping_pong_2
```

```
mpirun --allow-run-as-root -n 4 ./ping_pong_2 <size of vector [int]>
```

To run experiment:

```
python3 ping_pong_2_runner.py
```

