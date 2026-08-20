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

| N         | Task 1  | Task 2   | Task 3   |
| --------- | ------- | -------- | -------- |
| 1000      | 0.0001  | 0.0002   | 0.0144   |
| 10000     | 0.0019  | 0.0006   | 0.0013   |
| 100000    | 0.0356  | 0.0029   | 0.0034   |
| 1000000   | 0.9667  | 0.0644   | 0.0600   |
| 10000000  | 29.4885 | 1.5517   | 0.9702   |
| 100000000 | NaN     | 40.7020s | 26.3943s |

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
