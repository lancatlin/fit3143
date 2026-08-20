---
theme: gaia
_class: lead
paginate: true
backgroundColor: #fff
backgroundImage: url('https://marp.app/assets/hero-background.svg')
---

<!--![bg left:40% 80%](https://marp.app/assets/marp.svg)-->

# Group 64: Lab 1 Report

Clement
Justin Lin

---

### Task 1

```c
IntSlice find_primes(int n) {
    IntSlice primes = make_slice(0, 10);
    if (n < 2)
        return primes;
    int count = 0;
    for (int i = 2; i < n; i++) {
        int sqroot = floor(sqrt(i));
        bool is_prime = true;
        for (int j = 2; j <= sqroot; j++) {
            if (i % j == 0) {
                is_prime = false;
            }
        }
        if (is_prime) {
            append_slice(&primes, i);
        }
    }
    return primes;
}
```

---

### Task 2

```c

```

---

### Task 3

```c
IntSlice find_primes(int n) {
    IntSlice primes = make_slice(0, 10);
    if (n < 2)
        return primes;

#pragma omp parallel for schedule(dynamic, 10) ordered
    for (int i = 2; i < n; i++) {
        int sqroot = floor(sqrt(i));
        bool is_prime = true;
        for (int j = 2; j <= sqroot; j++) {
            if (i % j == 0) {
                is_prime = false;
                break;
            }
        }
        if (is_prime) {
#pragma omp ordered
            {
                append_slice(&primes, i);
            }
        }
    }
    return primes;
}

```

---

| N         | Task 1   | Task 2  | Task 3  |
| --------- | -------- | ------- | ------- |
| 1000      | 0.0000   | 0.0006  | 0.0008  |
| 10000     | 0.0004   | 0.0008  | 0.0008  |
| 100000    | 0.0077   | 0.0015  | 0.0065  |
| 1000000   | 0.0964   | 0.0173  | 0.0176  |
| 10000000  | 2.2681   | 0.3513  | 0.3331  |
| 50000000  | 22.1405  | 3.3804  | 3.2969  |
| 100000000 | 60.2406  | 9.3780  | 9.8522  |
| 200000000 | 162.8525 | 25.5429 | 26.6051 |
