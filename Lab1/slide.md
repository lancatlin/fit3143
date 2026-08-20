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

## Comparison

With 16 threads

| N         | Task 1  | Task 2 | Task 3  |
| --------- | ------- | ------ | ------- |
| 1000      | 0.0000  | 0.0009 | 0.0008  |
| 10000     | 0.0004  | 0.0008 | 0.0014  |
| 100000    | 0.0085  | 0.0021 | 0.0751  |
| 1000000   | 0.1043  | 0.0149 | 0.0719  |
| 10000000  | 2.4719  | 0.3507 | 1.0926  |
| 100000000 | 65.0777 | 9.9153 | 22.8316 |

---

## Task 1

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

## Task 2

```c

```
